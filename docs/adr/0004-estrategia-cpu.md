<!-- SPDX-FileCopyrightText: 2026 henoSNES contributors -->
<!-- SPDX-License-Identifier: CC-BY-4.0 -->

# ADR-0004: Estrategia de Implementación del Intérprete de la CPU 65C816

## Estado

Propuesto

## Fecha

2026-10-02

## Contexto

El núcleo de la CPU 65C816 requiere una implementación estricta, ciclo a ciclo. Dado que su conjunto es complejo (256 opcodes, más modos dinámicos según el estado de los flags M/X de 8/16 bits), la forma de despachar (dispatching) la ejecución de la CPU afecta directamente:
1. **Precisión de ciclos**: Necesaria para la emulación correcta y pase estricto de las suites de prueba.
2. **Determinismo y Serialización**: La ejecución debe ser pausada y volcada a disco bit a bit de forma serializable (esencial para run-ahead y rollback netplay).
3. **Rendimiento**: En el modelo ciclo a ciclo, la sobrecarga del despacho debe ser lo más cercana a cero posible.
4. **Legibilidad**: Facilidad para revisar, depurar e integrar nuevos contribuidores.

## Decisión

**Se recomienda utilizar la opción 3 (Tabla de Plantillas C++20)**, aunque se mantiene en estado de propuesta hasta Gate A.

La estrategia de tabla de plantillas (Templates array) aprovecha el preprocesador y el compilador de C++ para construir, en tiempo de compilación, una tabla de punteros a función de las 256 instrucciones resolviendo de antemano el código repetitivo de lectura de operandos, ejecución y escritura. Cada "handler" emite sus propios `bus.read()` y `bus.write()` y el control avanza de forma determinista. El estado se encapsula completamente en la instancia de la clase `CPU`, haciéndolo trivialmente serializable.

### Detalles técnicos

- La CPU define un bucle que se ejecuta invocando `handlers[opcode](this)` en cada iteración de instrucción principal (o manejando subciclos en un state machine).
- Los modos de direccionamiento y operaciones lógicas se modelan como funciones separadas inyectadas vía plantilla `<Op, AddrMode>`.
- Evita el gran impacto de caché y la enorme longitud de código de un `switch` único.
- El estado serializable queda 100% definido por los registros y contadores de la CPU, sin depender de variables estáticas o callbacks con clausuras impredecibles.

## Alternativas consideradas

1. **Intérprete basado en un switch gigante**:
   - Consiste en un único método `step()` que contiene `switch(opcode) { case 0x00: ... case 0xFF: ... }`.
   - *Descartada porque:* Es infame por dificultar la legibilidad, produce un binario de métodos extremadamente largos que el compilador no siempre optimiza eficientemente, y entorpece las métricas de cobertura.
2. **Tabla dinámica de punteros a función (C Tradicional)**:
   - Registrar punteros a 256 funciones C estáticas separadas en un array dinámico.
   - *Descartada porque:* Implica reescribir manualmente cada modo (e.g. `LDA_Immediate`, `LDA_Absolute`), generando muchísimo código duplicado, propenso a errores humanos (falla de DRY).
3. **Tabla de plantillas C++20**:
   - Generación de los 256 handlers combinando lógicas modulares (e.g. `template <auto Op, auto Mode> void Execute()`).
   - *Elegida porque:* Mantiene el código corto y declarativo, logrando un rendimiento igual o superior a la opción 1 (optimización inlining fuerte) y haciendo el estado determinista y fácilmente serializable, como exige el `run-ahead`.

## Consecuencias

- ✅ Rendimiento de ejecución predecible y optimizado, facilitando la meta de 60fps constantes incluso con emulación ciclo-exacta en hardware de bajo nivel (Intel UHD).
- ✅ Escalabilidad y facilidad de depuración: modificar un modo de direccionamiento arregla todos los opcodes que lo usan automáticamente.
- ✅ Estado completamente serializable (no hay llamadas lambda o generadores ocultos que rompan el determinismo).
- ⚠️ Complejidad de compilación: el uso extensivo de templates (inlining de las 256 combinaciones) incrementará ligeramente el tiempo de compilación en frío del archivo `.cpp`.

## Documentos relacionados

- `docs/SPEC.md` §4 (Decisiones 2 y 3)
- `docs/refs/65c816-opcodes.md` (Tabla que define las funciones a mapear)
- `docs/BACKLOG.md` (Tarea P-01)
