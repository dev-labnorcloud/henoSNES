<!-- SPDX-FileCopyrightText: 2026 henoSNES contributors -->
<!-- SPDX-License-Identifier: GPL-3.0-or-later -->

# ADR-0004: Estrategia de Implementación del Intérprete de la CPU 65C816

## Estado

Propuesto

## Fecha

2026-10-02

## Historial de revisión

- **2026-10-03 (E5)**: Correcciones de revisión cruzada. Licencia mantenida en GPL-3.0-or-later por decisión humana. Ajuste de WAI, STP y MVN/MVP.
- **2026-10-03 (E3)**: Revisión y alineación completa con ADR-0005 (Scheduler Cooperativo). Ajuste de granularidad, modos de ejecución, perfiles y serialización.

## Contexto

El núcleo de la CPU 65C816 requiere una implementación estricta, ciclo a ciclo. Dado que su conjunto de instrucciones es complejo (256 opcodes, más modos dinámicos según el estado de los flags M/X de 8/16 bits y el modo de emulación E), la forma de despachar (dispatching) la ejecución de la CPU afecta directamente:

1. **Precisión de ciclos**: Necesaria para la emulación correcta y pase estricto de las suites de prueba.
2. **Determinismo y Serialización**: La ejecución debe ser determinista y volcada a disco en un formato binario versionado (esencial para run-ahead, rebobinado y netplay rollback).
3. **Rendimiento**: En el modelo de interpretación, la sobrecarga del despacho debe minimizarse.
4. **Legibilidad**: Facilidad para revisar, depurar e integrar nuevos contribuidores.

## Decisión

Se adopta la estrategia de **Tabla de Plantillas C++20 (Templates array)** para la CPU, alineada orgánicamente con la CPU maestra secuencial descrita en el ADR-0005.

El compilador de C++ se encarga de construir, en tiempo de compilación, las tablas de punteros a función de las instrucciones, resolviendo de antemano el código repetitivo de lectura de operandos, ejecución y escritura. Cada "handler" de instrucción emite sus propios `Bus::read8()`, `Bus::write8()` y `Bus::idle()`, y el control avanza de forma determinista y secuencial.

### 1. Granularidad

La CPU opera con una granularidad de despacho por instrucción. El bucle principal de la CPU simplemente invoca `active_table[opcode](this)` para cada instrucción. El handler invocado es código secuencial ordinario; cada ciclo de la instrucción pasa por `Bus::read8()`, `Bus::write8()` o `Bus::idle()`, que a su vez llaman a `scheduler.advance()` (ver ADR-0005 §3). La CPU nunca se suspende a mitad de instrucción.

### 2. Modos de Ejecución (M/X/E)

La CPU 65C816 altera el tamaño de sus operaciones (8 o 16 bits) según los flags M y X, y su comportamiento general con el flag E (emulación). Evaluar estos flags dinámicamente en cada instrucción añadiría un overhead innecesario.

Se generarán **cinco tablas de 256 handlers** mediante plantillas:
- Una tabla para E=1 (modo emulación, donde M=X=1 son forzados).
- Cuatro tablas para E=0 (modo nativo), cubriendo las combinaciones de (M, X) ∈ {0,1}².

La CPU mantendrá un puntero a la tabla activa (`active_table`). Este puntero se recalcula **únicamente** cuando los flags E, M o X cambian (instrucciones REP, SEP, XCE, PLP, RTI y reset).
El modo decimal (P.D) se evalúa y resuelve dinámicamente dentro de los handlers de ADC y SBC en lugar de requerir tablas adicionales, ya que duplicar las tablas por el modo decimal sería excesivo y aportaría poco beneficio.

**Precedente**: Snes9x utiliza tablas separadas por modo de ejecución [verificar].

**Impacto en tamaño**: Cada perfil (ver §3) instanciará 1280 handlers (5 tablas × 256 opcodes).

### 3. Perfiles `accuracy` y `performance`

Conforme a la SPEC y ADR-0005, el núcleo debe soportar dos perfiles. La CPU se implementará como una plantilla parametrizada por el tipo de Bus (`template <class BusT> class Cpu`). A su vez, el Bus estará parametrizado por el `Scheduler<Policy, Ppu, Apu>` (ADR-0005 §5).

Las tablas de handlers se instanciarán una vez por perfil. El código interno de los handlers de la CPU no depende de la política de sincronización del scheduler; simplemente llama a los métodos del Bus inyectado, logrando compatibilidad plena de código fuente para ambos perfiles sin pérdida de rendimiento.

### 4. Serialización del Estado

En cumplimiento con el ADR-0005 (§3 y §6), el estado de la CPU se encapsula completamente en un `struct State` trivialmente copiable. La serialización de este estado (o su captura para clonación en run-ahead) ocurre **exclusivamente en el primer límite de instrucción tras el inicio de V-blank**, y justo después de forzar la sincronización de todos los componentes subordinados mediante `sync_all()`.

El puntero a la tabla activa (`active_table`) **no se serializa**. Es una caché derivada de los flags P y E almacenados en el `State`, y se recalcula al finalizar la carga de un savestate (regla 5 de ADR-0005: cachés recalculables no se serializan).

### 5. Interrupciones y Casos Especiales

- **NMI e IRQ**: Se reconocen y atienden en el límite de instrucción. El instante exacto de muestreo dentro del último ciclo de la instrucción previa queda como detalle de implementación a validar [verificar: docs/refs/65c816-quirks.md, fullsnes].
- **Fuentes de Interrupción**: Las interrupciones hardware (NMI por V-blank, IRQ por temporizadores H/V) se evalúan a partir de los contadores H/V del `State` de la CPU, que avanzan con `master_cycle` (ADR-0005, Detección de eventos). Esto garantiza que el timing de interrupciones sea independiente del catch-up de la PPU y, por tanto, idéntico entre los perfiles `accuracy` y `performance`.
- **BRK y COP**: Se implementan como entradas normales en las tablas de handlers, no como excepciones de hardware.
- **WAI (Wait for Interrupt)**: Se modela como un flag `waiting` en el `State` de la CPU. Mientras el flag esté activo, el bucle principal ejecuta un `Bus::idle()` por iteración y cada iteración es un límite de instrucción (punto seguro). Despierta con NMI o IRQ; si I=1, la IRQ lo despierta pero no se atiende y la ejecución continúa con la siguiente instrucción [verificar].
- **STP (Stop the Clock)**: Se modela como un flag `stopped` en el `State` de la CPU. Mientras esté activo, el bucle ejecuta un `Bus::idle()` por iteración como límite de instrucción seguro. Solo sale con un reset por hardware.
- **XCE (Exchange Carry and Emulation)**: Al entrar en modo emulación (E=1), la CPU fuerza dinámicamente M=X=1, limpia los bytes altos de los registros X e Y, fija el byte alto de SP en `$01` y recalcula la tabla activa [verificar el hardware real].

## Alternativas consideradas

1. **Intérprete basado en un switch gigante**:
   - Consiste en un único método `step()` que contiene `switch(opcode) { case 0x00: ... case 0xFF: ... }`.
   - *Descartada porque:* Produce un método monstruoso que afecta la legibilidad y complica el mantenimiento. La comparación de rendimiento contra este modelo queda como experimento opcional, no como requisito para validar esta decisión.
2. **Tabla dinámica de punteros a función (C Tradicional)**:
   - Registrar punteros a 256 funciones C estáticas separadas en un array dinámico, recibiendo M/X por parámetro.
   - *Descartada porque:* Implica resolver la lógica modular con macros o reescribir manualmente cada combinación (e.g. `LDA_Immediate_M8`), generando código duplicado propenso a errores humanos y costoso de mantener.
3. **Bifurcación por M/X dentro de cada handler (tabla única de 256)**:
   - Usar una única tabla donde cada handler contiene condicionales `if (M)` o `if (X)`.
   - *Descartada porque:* Obliga a evaluar los flags dinámicamente en cada instrucción, añadiendo overhead, mientras que la estrategia de 5 tablas recalcula el puntero a la tabla activa solo cuando los flags cambian (REP, SEP, XCE, PLP, RTI y reset).

## Consecuencias

- 🔄 **Rendimiento**: Hipótesis pendiente de validación. Se asume que la tabla de plantillas minimizará el overhead de despacho; esto se verificará mediante el criterio de validación 1.
- ✅ **Legibilidad y Escalabilidad**: Modificar la lógica de un modo de direccionamiento (ej. `Absolute Indexed`) o una operación base (ej. `ADC`) arregla todos los opcodes afectados de forma declarativa.
- ✅ **Serialización predecible**: El estado (`struct State` trivialmente copiable) es determinista, fácilmente serializable y libre de abstracciones opacas. El puntero de la tabla activa se restaura dinámicamente.
- ⚠️ **Tamaño del binario y compilación**: El compilador resolverá las instanciaciones (2 perfiles × 1280 handlers = 2560 funciones generadas), lo que incrementará el tiempo de compilación inicial y el tamaño del binario final, considerándose un trade-off aceptable.

## Criterios de Validación

1. **Rendimiento (Línea Base)**: Microbenchmark del intérprete corriendo con un `Bus` nulo (instrucciones por segundo). Se registrará como línea base (Clang 17, -O2, media de 50 ejecuciones). El criterio de éxito es que no ocurran caídas de rendimiento mayores al 5 % monitorizadas mediante una alerta nocturna en CI (alineado con SPEC §9).
2. **Corrección de Opcodes**: Pruebas unitarias por opcode derivadas lógicamente de `docs/refs/65c816-opcodes.md`. P1-001 (Implementación CPU) tiene una dependencia estricta con P1-000 (verificación cruzada de la tabla).
3. **Pase de Suites de Prueba**: Debe pasar las ROMs de prueba referenciadas en P1-017 (CPU Test ROMs).
4. **Suites Comunitarias Adicionales**: Se incorporarán suites de prueba comunitarias instrucción por instrucción **solo si** su licencia está explícitamente verificada como compatible con GPLv3 [verificar licencia].

## Documentos relacionados

- `docs/SPEC.md` §4 (Decisiones 2 y 3)
- `docs/adr/0005-scheduler-cooperativo.md` (Scheduler y políticas de sincronización)
- `docs/refs/65c816-opcodes.md` (Tabla que define las funciones a mapear)
- `docs/refs/65c816-quirks.md` (Quirks: NMI/IRQ sampling, WAI, STP, XCE)
- `docs/BACKLOG.md` (Tareas P1-000, P1-001, P1-017)
