<!-- SPDX-FileCopyrightText: 2026 henoSNES contributors -->
<!-- SPDX-License-Identifier: GPL-3.0-or-later -->

# ADR-0005: Scheduler Cooperativo

## Estado

Aceptado

## Fecha

2026-10-03

## Historial de revisión

- **2026-10-03 (E5)**: Correcciones de revisión cruzada. Puntos de captura únicos en V-blank, WAI/STP como puntos seguros, revisión de frecuencias, DMA/HDMA y clarificación del perfil performance.

## Contexto

La SNES contiene múltiples procesadores concurrentes — CPU (65C816), PPU (S-PPU1/S-PPU2),
APU (SPC700 + S-DSP) y coprocesadores de cartucho — cada uno con su propio reloj. El
comportamiento correcto de los juegos depende de la sincronización precisa entre estos
componentes: la CPU escribe a registros de la PPU en momentos específicos del scanline, la
APU intercambia datos con la CPU a través de puertos I/O de 4 bytes, y coprocesadores como
SA-1 comparten el bus de memoria con la CPU principal.

La SPEC (§4, decisiones 1–3) establece tres requisitos que este ADR debe resolver de forma
conjunta:

1. **Núcleo dual** (`accuracy` y `performance`): un único árbol de código con dos perfiles
   de compilación que comparten estados de guardado compatibles.
2. **Scheduler cooperativo**: hilos lógicos con relojes independientes sincronizados en
   puntos de acceso compartido.
3. **Serialización determinista**: todo el estado se serializa en un formato binario
   versionado; es la base de estados, rebobinado, netplay rollback y pruebas de regresión.

Además, `AGENTS.md` impone que `core/` no use excepciones C++ y que todo acceso a memoria
emulada pase por el bus con trazabilidad para el depurador.

Existe un spike exploratorio en la rama de archivo `spike/P1-bus-sched` (antes `feat/P1-bus-sched`, no mergeada) que implementa
una variante de catch-up con `Bus` llamando a `Scheduler::advance()`. Se evalúa aquí como
una de las alternativas.

### Fuera de alcance

- **Coprocesadores que acceden al bus como maestros** (p. ej. SA-1, Fase 2): requieren que
  más de un componente emita accesos al bus y, por tanto, otro modelo de maestro/subordinado.
  Se resolverán en un **ADR propio futuro**. Este ADR solo cubre **coprocesadores
  pasivos**: aquellos que solo progresan observablemente cuando la CPU accede a sus puertos [verificar] (p. ej., DSP-1,
  DSP-2), distinguiéndose de los coprocesadores activos (SA-1, SuperFX) que emplean el bus de la CPU o del cartucho mediante arbitraje y quedan fuera de alcance.
- El formato binario del savestate (ADR de serialización asociado a P1-021).

## Decisión

Se adopta un **scheduler con la CPU como maestra y sincronización por catch-up de los
componentes subordinados**. La CPU se escribe como código secuencial (un handler por
instrucción); solo los subordinados (PPU, APU) avanzan por catch-up hasta el ciclo maestro
actual. La política de sincronización es un **parámetro de plantilla** que diferencia los
perfiles `accuracy` y `performance`.

### 1. Unidad de tiempo común

El tiempo se mide en **ciclos de reloj maestro** como un contador entero de 64 bits
(`uint64_t`). Este es el reloj base de la consola, derivado del cristal principal.

**Frecuencias del cristal principal:**

| Región | Frecuencia | Representación | Fuente |
|--------|-----------|----------------|--------|
| NTSC   | 315/88 × 6 MHz ≈ 21 477 272,727… Hz | Racional exacto 315/88 × 6 000 000 | [verificar: fullsnes §Clocking, anomie docs] |
| PAL    | ≈ 21 281 370 Hz | [verificar: fullsnes §Clocking] | [verificar] |

Las frecuencias se representan como **racionales exactos** (numerador/denominador enteros)
cuando la fuente primaria lo permita, para evitar acumulación de error por redondeo.

**Divisores respecto al reloj maestro:**

| Componente | Divisor | Ciclos maestros por ciclo de componente | Fuente |
|-----------|---------|----------------------------------------|--------|
| CPU (acceso normal, WRAM, ROM lenta) | ÷8 | 8 | [verificar: fullsnes, `docs/refs/65c816-bus.md`] |
| CPU (acceso rápido, FastROM) | ÷6 | 6 | [verificar: fullsnes] |
| CPU (ciclos internos) | ÷6 | 6 | [verificar: fullsnes] |
| CPU (registros I/O lentos, $4000-$41FF) | ÷12 | 12 | [verificar: fullsnes] |
| PPU | ÷4 (dot clock) | 4 | [verificar: fullsnes §PPU Timing] |
| APU (SPC700 + S-DSP) | Resonador cerámico | 24 606 720 Hz (constante canónica) | [verificar: fullsnes §APU Timing; varía por consola] |

> **Dominio de reloj de la APU.** La APU tiene su propio resonador cerámico
> (frecuencia que varía entre consolas [verificar]), independiente del cristal principal.
> Para mantener coherencia con la salida PCM de 32 040 Hz fijada en la SPEC, se adopta
> la constante canónica de 24 606 720 Hz (= 32 040 Hz × 768) [verificar]. La sincronización
> CPU↔APU ocurre a través de los 4 puertos I/O (`$2140–$2143`) y no comparte un divisor
> entero exacto con el reloj maestro. El scheduler mantiene un contador separado para la
> APU (`apu_cycle`, que cuenta ticks del oscilador; el SPC700 avanza cada 24 ticks [verificar]) y convierte entre dominios con **aritmética entera exclusivamente** (sin `float`),
> mediante un acumulador racional:
>
> ```text
> razón APU/maestro = F_apu / F_maestro   (fracción reducida N/D)
> por cada advance(master_cycles):
>     apu_acc   += master_cycles × N
>     apu_cycle += apu_acc / D
>     apu_acc    = apu_acc % D
> ```
>
> Usando la constante adoptada de 24 606 720 Hz y la frecuencia NTSC de 236 250 000 / 11 Hz:
> `(24 606 720 × 11) / 236 250 000 = 270 673 920 / 236 250 000`. Dividiendo por el MCD (2 160),
> la fracción reducida es N/D = 125 312 / 109 375. El
> resto `apu_acc` forma parte del estado serializado, de modo que la conversión es exacta y
> determinista a largo plazo. La razón PAL se deriva igual una vez verificada su frecuencia.

**Refresco de DRAM:** Aproximadamente cada scanline, el refresco de memoria detiene a la CPU durante unos 40 ciclos maestros [verificar]. Esto afecta al timing general en ambos perfiles.

### 2. Mecanismo de sincronización

#### Alternativas evaluadas

**(a) Hilos cooperativos con pila propia (estilo bsnes/libco)**

Cada componente tiene un stack real del SO; `yield()`/`resume()` cambia de contexto entre
componentes. Es el modelo de bsnes y higan [verificar: versión de bsnes en que se adoptó
libco].

- ✅ Permite escribir el código de cada componente como un bucle secuencial natural.
- ✅ Sincronización bus-level trivial: `yield()` en cada acceso al bus.
- ❌ **Serialización frágil**: el estado suspendido incluye la pila del SO (registros de
  máquina, instruction pointer, stack frames), que no es portable entre plataformas,
  compiladores ni versiones del compilador. bsnes resuelve esto sincronizando solo en
  puntos seguros y recorriendo el estado hasta alcanzarlos (afirmación pendiente de validación [verificar]), lo que complica
  significativamente la serialización.
- ❌ **Dependencia nueva**: `libco` es código ensamblador por plataforma (x86_64, ARM64,
  Win64 SEH). Según `AGENTS.md`, toda dependencia nueva requiere ADR aprobado por un
  humano.
- ❌ **Run-ahead**: clonar un estado que incluye stacks del SO es complejo y frágil.

**(b) Corrutinas C++20 sin pila (stackless)**

Uso de `co_await`/`co_yield` para suspender cada componente en puntos de sincronización. El
compilador transforma el cuerpo en una máquina de estados implícita.

- ✅ Código secuencial legible, similar a (a) pero en C++ estándar.
- ✅ Sin dependencias externas.
- ❌ **No serializable**: el frame de la corrutina es una estructura opaca generada por el
  compilador. No existe una API estándar para inspeccionar, copiar ni serializar este
  frame. Esto hace imposible implementar save states, rebobinado o netplay rollback sin
  recurrir a hacks específicos del compilador.
- ⚠️ Allocación heap por frame de corrutina (mitigable con allocadores custom, pero
  añade complejidad).

**(c) CPU maestra secuencial + subordinados por catch-up (ELEGIDA para `accuracy`)**

La CPU es la única maestra: sus handlers de instrucción son código secuencial ordinario y
nunca se suspenden. Cada ciclo de CPU (acceso al bus o ciclo interno) llama a
`advance()`, y el scheduler hace avanzar a los subordinados (PPU, APU) mediante
`catch_up(target_master_cycle)` hasta el ciclo maestro actual. Solo los subordinados deben
poder detenerse y reanudarse en un ciclo objetivo arbitrario; su progreso vive en su
`State`.

- ✅ **Serialización sencilla**: cada componente separa su estado en un `struct State`
  trivialmente copiable. Clonar = copiar los `State`. Serializar = recorrer los campos de
  cada `State` con el `Serializer`.
- ✅ **Run-ahead**: clonar el estado es copiar los `State` de todos los componentes.
- ✅ **Determinismo**: sin estado oculto (no hay pila suspendida ni frame opaco).
- ✅ **Portabilidad**: C++ estándar puro, sin ensamblador ni extensiones del compilador.
- ✅ **CPU legible**: los handlers de la CPU siguen siendo secuenciales; no hay FSM en la
  CPU.
- ✅ **Conformidad con `AGENTS.md`**: sin dependencias nuevas, sin excepciones, todo
  acceso vía bus.
- ⚠️ Los subordinados (PPU, APU/SPC700) deben escribirse como código reanudable en
  cualquier ciclo objetivo, lo que es más verboso que un bucle secuencial.
- ⚠️ Como la CPU solo se serializa en límites de instrucción, todo lo que ocurra a mitad
  de instrucción (HDMA, ver §3) debe completarse dentro de la instrucción en curso.

**(d) Sincronización lazy (ELEGIDA para `performance`)**

Variante de (c) donde PPU y APU no se sincronizan en cada ciclo de CPU.
En Fase 1, se mantiene una sola PPU capaz de detenerse en cualquier ciclo; el perfil `performance` solo cambia la granularidad del catch-up manteniendo exactamente el mismo `State`. La implementación de un renderer de PPU por scanline completo se pospone para un ADR futuro con la restricción de que deberá mantener compatibilidad de `State`.

Los puntos de sincronización obligatoria de este perfil son:
- Fin de cada scanline.
- El punto único de captura (primer límite de instrucción tras inicio de V-blank, ver §3).
- Toda lectura o escritura a registros de la PPU (`$2100-$213F`) y puertos de la APU (`$2140-$2143`).

- ✅ **Mucho menos overhead**: órdenes de magnitud menos puntos de sincronización.
- ✅ Misma infraestructura que (c), mismo `State`, misma serialización y run-ahead.
- ⚠️ **Menor precisión**: efectos mid-scanline (HDMA, IRQ timing fino, raster effects)
  pueden ser incorrectos. Algunos juegos requieren sincronización por acceso para
  funcionar.
- ⚠️ No apto como referencia para pruebas de precisión.

#### Decisión

Se adopta **(c) CPU maestra + catch-up** como mecanismo base, con **(d) lazy scanline**
como política alternativa. Ambas comparten la misma infraestructura (`Scheduler`, `Bus`,
`State` de cada componente) y se seleccionan mediante un **parámetro de plantilla** en
tiempo de compilación:

```cpp
enum class SyncPolicy { Accuracy, Performance };

template <SyncPolicy Policy, class Ppu, class Apu>
class Scheduler { /* ... */ };
```

> **Compatibilidad de savestates entre perfiles** (SPEC §4, decisión 1): los savestates
> deben ser compatibles entre `accuracy` y `performance`. Esto se garantiza porque ambos
> perfiles serializan exactamente los mismos `State`, los savestates se capturan siempre
> en el punto único de captura de V-blank y, antes de capturar, se fuerza el catch-up de todos los
> subordinados hasta `master_cycle` en ambos perfiles (ver §3). La política de
> sincronización solo afecta *cuándo* se invoca catch-up durante la ejecución, no *qué*
> estado se serializa ni en qué punto temporal queda cada componente al serializar.

### 3. Granularidad y puntos de sincronización

#### Firma única de `advance()`

Hay una sola firma, miembro del scheduler parametrizado por política:

```cpp
template <SyncPolicy Policy, class Ppu, class Apu>
class Scheduler {
public:
    void advance(uint32_t master_cycles);
    void sync_all();   // catch-up forzado de todos los subordinados hasta master_cycle
};
```

Todas las llamadas a `advance()` se realizan desde el `Bus` (`read8()`, `write8()`, `idle()` y la vía dedicada de DMA); la CPU y el controlador DMA nunca llaman al scheduler directamente.

#### Perfil `accuracy`

- **La CPU es la maestra.** El bucle principal ejecuta instrucciones de CPU una a una. Los
  handlers son código secuencial; cada ciclo de la instrucción pasa por el bus.
- **Cada ciclo de CPU llama a `advance()`**, no solo los accesos a memoria:
  - Ciclos de acceso: `Bus::read8()` / `Bus::write8()` calculan el coste del acceso según
    la región (÷6, ÷8 o ÷12) y llaman a `advance(cycles)`.
  - Ciclos internos (sin acceso a memoria): la CPU llama a `Bus::idle()`, que llama a
    `advance(6)` [verificar: coste de ciclo interno].
  - El avance se aplica antes del acceso al dispositivo, de modo que el subordinado está
    al día en el instante del acceso. El orden fino dentro del ciclo se fija en P1-004
    [verificar].
- **Catch-up en cada `advance()`**: el scheduler actualiza `master_cycle` y hace avanzar a
  los subordinados (PPU, APU) hasta el nuevo ciclo.
- **Granularidad real: por ciclo de CPU** (acceso al bus o ciclo interno). Esto es
  coherente con la SPEC, que exige "conteo de ciclos por acceso al bus" (P1-004).

#### Perfil `performance`

- Misma estructura y misma firma, pero `advance()` solo acumula ciclos sin invocar
  catch-up en cada llamada. Los subordinados se sincronizan al cruzar el final de cada
  scanline, cuando se accede a un recurso compartido (ver lista exhaustiva de puntos de sincronización en §2 (d)) y en los puntos de sincronización forzada (ver abajo).

#### DMA y HDMA

El controlador DMA forma parte del S-CPU y actúa en el lado maestro; no es un subordinado.
Para sus transferencias, el `Bus` expone una vía de acceso DMA dedicada que cobra 8 ciclos maestros por byte una sola vez (lectura en un bus y escritura en el otro simultáneamente), ignorando el coste normal por región de memoria [verificar].

- **DMA general**: se dispara al escribir en `$420B` (MDMAEN). La transferencia se ejecuta
  dentro del ciclo de bus siguiente a esa escritura, detenido el flujo de la CPU, y la
  CPU continúa al terminar [verificar: punto exacto de arranque y ciclos de overhead].
- **HDMA**: se ejecuta al comienzo de cada H-blank (y su inicialización al comienzo del
  frame) y **detiene a la CPU a mitad de instrucción**. El HDMA tiene prioridad sobre el DMA general en curso, por lo que el flag de HDMA pendiente se comprueba tanto en la ruta de ciclos común de la CPU como dentro del bucle del DMA general [verificar: fullsnes]. La CPU evalúa los contadores H/V y levanta este flag sin intervención del scheduler. Luego, ejecuta la transferencia HDMA como una llamada anidada dentro del handler en curso,
  fuera del contexto de `advance()`. Como la CPU es código secuencial, el handler
  simplemente está más arriba en la pila mientras el HDMA corre, y termina antes del
  siguiente límite de instrucción. `advance()` nunca invoca, directa ni indirectamente,
  otro `advance()`.

**Detección de eventos:** Los contadores de posición H/V, los latches de NMI/IRQ y el flag de HDMA pendiente pertenecen al `State` de la CPU (como parte del bloque S-CPU). Se actualizan directamente en la ruta de ciclos de la CPU añadiendo los mismos ciclos que se envían al Bus. El Scheduler no detecta eventos ni conoce la CPU. La evaluación se calcula a partir de contadores H/V que avanzan con `master_cycle`, teniendo en cuenta la existencia de líneas cortas y largas y el bit de interlace [verificar]. Esta detección **no depende del estado ni del catch-up de la PPU**. Como `master_cycle` avanza de la misma manera en ambos perfiles, el timing
de interrupciones y DMA/HDMA es idéntico tanto en `accuracy` como en `performance`.

#### Instrucciones MVN/MVP (transferencia de bloques)

MVN (`$54`) y MVP (`$44`) transfieren hasta 64 KB byte a byte, consumiendo **7 ciclos de
CPU por byte** (no ciclos maestros; el coste en ciclos maestros depende de la velocidad de
cada acceso) [verificar: `docs/refs/65c816-quirks.md`, fullsnes].

**Decisión:** cada byte transferido es una ejecución completa de la instrucción y
**termina en un límite de instrucción**. Tras mover un byte y decrementar C (16 bits, sea cual sea el estado de M), si C ≠
`$FFFF`, el PC se deja apuntando al opcode MVN/MVP, de modo que la siguiente instrucción
re-decodifica MVN/MVP y continúa la transferencia usando A, X, Y como contadores/punteros
vivos. Las interrupciones (NMI/IRQ) se atienden en ese límite como en cualquier otra
instrucción, y un savestate capturado ahí refleja "a punto de transferir el siguiente
byte". No existe ningún mecanismo especial de cesión de control.

Comportamiento del hardware: [verificar: `docs/refs/65c816-quirks.md`, fullsnes]. La
equivalencia con la implementación de bsnes se marca [verificar].

#### Puntos seguros de guardado (savestates) y clonación

Para garantizar el determinismo, los puntos de sincronización de cada perfil son siempre los mismos, haya o no una captura en curso. Toda captura (savestate, clon de run-ahead, rebobinado, rollback netplay) se toma única y exclusivamente en el **primer límite de instrucción tras el inicio de V-blank**.

En ese punto, **ambos perfiles ejecutan obligatoriamente `sync_all()`** en todos los frames, forzando el catch-up de todos los subordinados, se requiera o no una captura. En `accuracy`, este `sync_all()` es efectivamente un no-op porque los subordinados ya están al día, pero en `performance` asegura la consistencia de los datos. Si el usuario solicita un savestate en otro momento, la petición se difiere internamente hasta alcanzar este punto de V-blank.

Las instrucciones WAI y STP se modelan como flags en el `State` de la CPU; cada iteración de su bucle de espera consume ciclos (`Bus::idle()`) y cuenta como un límite de instrucción válido (punto seguro) que puede coincidir con el inicio de V-blank.

En este punto seguro garantizado:

- El PC apunta a la siguiente instrucción (o al opcode MVN/MVP, WAI, STP en curso).
- Todos los registros de la CPU están en un estado definido.
- No hay estado "a mitad de decodificación" ni "a mitad de ejecución" (ni HDMA en curso).
- Todos los subordinados están exactamente en `master_cycle`.

El formato binario completo del savestate queda fuera de alcance de este ADR y se definirá
en un **ADR de serialización pendiente** (asociado a P1-021).

### 4. Dirección de dependencias entre CPU, Bus, Scheduler y Serializer

```
┌──────────────────────────────────────────────────────────┐
│                      Núcleo (capa 1)                     │
│                                                          │
│  CPU ──usa──▶ Bus ──usa──▶ Scheduler<Policy, Ppu, Apu>   │
│  (+DMA)       │              │                           │
│               │              ├──▶ Ppu (tipo concreto)    │
│               │              ├──▶ Apu (tipo concreto)    │
│               │              └··▶ ICoprocessor (virtual, │
│               │                   fuera camino crítico)  │
│               └──usa──▶ Cartridge                        │
│                                                          │
│  Serializer ◀──usado por── State de cada componente      │
└──────────────────────────────────────────────────────────┘
```

**Reglas de dependencia:**

1. **CPU → Bus**: la CPU (incluido su controlador DMA) accede a toda la memoria a través
   de `Bus`. Nunca accede directamente a WRAM, ROM ni registros I/O.
2. **Bus → Scheduler**: el Bus llama a `scheduler.advance(master_cycles)` en cada ciclo de
   CPU (accesos y `idle()`). El Bus recibe la referencia al Scheduler por inyección de
   dependencias (constructor).
3. **Bus → Cartridge**: el Bus delega lecturas/escrituras al espacio del cartucho al objeto
   `Cartridge`. Recibido por inyección.
4. **Scheduler → subordinados (composición estática)**: PPU y APU se componen
   estáticamente: el Scheduler los conoce por su **tipo concreto** (parámetros de
   plantilla restringidos por un `concept` que exige `catch_up(uint64_t)`), sin despacho
   virtual en el camino crítico. En las pruebas se sustituyen por mocks instanciando la
   plantilla con otros tipos. El **despacho dinámico** (interfaz `ICoprocessor` con
   `catch_up()` virtual) se reserva para coprocesadores pasivos de cartucho, y solo se
   invoca fuera del camino crítico: al acceder la CPU a su rango mapeado y en
   `sync_all()`, nunca en cada `advance()`.
5. **Serializer ← todos**: cada componente separa su estado en un `struct State`
   trivialmente copiable (`static_assert(std::is_trivially_copyable_v<State>)`); lo que
   se serializa y se clona es ese `State`, no la clase. La clase contiene además
   referencias inyectadas (Bus, Scheduler) y cachés recalculables, que no se serializan.
   El Serializer recorre los campos del `State` explícitamente (orden y endianness
   definidos), no como volcado binario de la estructura. El Serializer no depende de
   ningún componente: es una utilidad bidireccional (save/load) que recorre un buffer.
6. **Sin dependencias circulares**: CPU→Bus→Scheduler→subordinados. Ningún componente
   subordinado depende del Bus ni de la CPU directamente; por tanto, ningún `catch_up()`
   puede volver a llamar a `advance()`.

**Reentrancia prohibida por diseño:** `advance()` no es reentrante, directa ni
indirectamente. La regla 6 garantiza que ningún subordinado puede invocarlo durante su
catch-up, y el mecanismo de HDMA/DMA delega la ejecución en la CPU precisamente para
evitar invocar a `advance()` desde adentro de otro `advance()`. La implementación lo
verifica con `assert()` (flag de "dentro de `advance()`" comprobado al entrar). No
existe mecanismo de acumulación ni de "flush" para llamadas reentrantes.

**Evaluación de la decisión del spike (`feat/P1-bus-sched`):**

El spike implementa exactamente la dirección CPU→Bus→Scheduler descrita arriba. Su
diseño es correcto para las dependencias. Se identifican los siguientes ajustes
necesarios:

- **Reentrancia**: el flag `m_in_catch_up` del spike acumula silenciosamente ciclos sin
  notificar a los subordinados durante llamadas reentrantes, ocultando un error de diseño.
  La implementación final elimina ese camino: una llamada reentrante a `advance()` es una
  violación de invariante y dispara `assert()`.
- **Despacho virtual**: el spike invoca a PPU/APU a través de una interfaz virtual (`ISchedulable::catch_up` virtual); la
  implementación final los compone estáticamente (regla 4).
- **Ciclos internos**: la implementación final añade `Bus::idle()` para que los ciclos
  internos de la CPU también llamen a `advance()`.
- **Trazabilidad del depurador**: el hook `debugger_hook()` del spike está vacío. La
  implementación final debe permitir registrar un callback externo para breakpoints y
  watchpoints, sin acoplar el Bus al depurador.

### 5. Perfiles `accuracy` y `performance`

La política de sincronización se selecciona como parámetro de plantilla:

```cpp
enum class SyncPolicy { Accuracy, Performance };

template <SyncPolicy Policy, class Ppu, class Apu>
class Scheduler {
public:
    void advance(uint32_t master_cycles);
    // En Accuracy: advance() invoca catch_up() en todos los subordinados.
    // En Performance: advance() acumula ciclos; catch_up() se invoca
    //   en puntos de sincronización obligatorios
    //   (fin de scanline, acceso a puertos APU, registros PPU, etc.).

    void sync_all();
    // En ambos perfiles: catch-up forzado de todos los subordinados (y
    // coprocesadores pasivos) hasta master_cycle. Obligatorio antes de
    // capturar un savestate o clonar para run-ahead.
};
```

**Implicaciones:**

- Dos instanciaciones de la plantilla producen dos rutas de código optimizadas por el
  compilador. Como PPU y APU se componen estáticamente (tipos concretos), no hay branch de
  política ni despacho virtual en el camino crítico; el único despacho dinámico
  (`ICoprocessor`) ocurre fuera de él.
- El mismo código fuente de CPU, PPU y APU se compila para ambos perfiles. La política
  solo afecta al `Scheduler` y, por composición estática, a los tipos que lo contienen
  (`Bus` y CPU se instancian una vez por perfil); el código de PPU y APU no depende de la
  política.
- **Savestates compatibles**: ambos perfiles serializan los mismos `State` en el mismo
  orden, y antes de capturar se ejecuta `sync_all()` en ambos, de modo que los
  subordinados quedan siempre en `master_cycle`. Un savestate creado en `accuracy` se
  puede cargar en `performance` y viceversa.

**Hipótesis de rendimiento** (sin evidencia medida todavía):

- El perfil `performance` debería ejecutar significativamente menos llamadas a `catch_up()`
  por frame (≈262 (NTSC) / 312 (PAL) scanlines por frame vs. ≈357 000 ciclos maestros por frame ÷ 6-8 por ciclo de CPU en `accuracy` [medir]). Esto debería
  reducir el overhead del scheduler de forma medible.
- **Criterio de aceptación**: benchmark de 600 frames (10 segundos de emulación) en
  `heno-cli` headless con una ROM homebrew de referencia. El perfil `performance` debe ser
  al menos un 30 % más rápido que `accuracy` midiendo el tiempo total para completar los 600 frames (media de 50 ejecuciones, Clang 17 -O2, misma máquina).

### 6. Serialización del estado del scheduler

#### Qué se serializa

El estado del scheduler es su propio `struct State` trivialmente copiable:

| Campo | Tipo | Descripción |
|-------|------|-------------|
| `master_cycle` | `uint64_t` | Contador global de ciclos maestros |
| `apu_cycle` | `uint64_t` | Contador del dominio de reloj de la APU |
| `apu_acc` | `uint64_t` | Resto del acumulador racional maestro→APU (ver §1) |

Adicionalmente, el `State` de la CPU (S-CPU) es propietario de variables de tiempo que no gestiona el Scheduler:

| Campo CPU | Tipo | Descripción |
|-----------|------|-------------|
| `h_counter` / `v_counter` | `uint16_t` | Posición del haz de la pantalla, avanza con la CPU |
| `nmi_latch` / `irq_latch` | `bool` | Estado de las líneas de interrupción |
| `hdma_pending` | `bool` | Flag para ejecución de HDMA en el H-blank actual |
| `waiting` / `stopped` | `bool` | Flags para el estado de las instrucciones WAI y STP |

Cada componente subordinado serializa su propio `State` (registros, VRAM, buffers de
audio, etc.) con el `Serializer`. El scheduler serializa solo su `State`. No se serializan
ciclos acumulados pendientes de notificar: `sync_all()` los deja en cero al capturar en V-blank.

#### Puntos seguros vs. estado intermedio

Este ADR prescribe que la serialización ocurre **exclusivamente en el primer límite de instrucción tras el inicio de V-blank**
y después de `sync_all()` (ver §3), en ambos perfiles. En este punto no existe estado
intermedio de la CPU (todos los registros son válidos y el PC apunta a la siguiente
instrucción) ni de los subordinados (todos en `master_cycle`).

Para MVN/MVP, cada byte termina en un límite de instrucción con el PC apuntando al opcode
y A/X/Y reflejando el progreso, por lo que cualquier punto entre bytes es un punto seguro
ordinario.

#### Impacto en funciones avanzadas

- **Run-ahead** (SPEC F-06): en el punto de V-blank se clona el estado completo (copia
  de los `State` de todos los componentes). *Nota: duplicar el estado completo difiere de la decisión 2 de la SPEC §4, pero se justifica y adopta porque el tamaño estimado del estado es pequeño (~300 KB [medir]) y cada `State` es trivialmente copiable, evitando la complejidad de una serialización parcial.* Se ejecutan N
  frames extra y se descarta el clon. Durante la simulación de frames de run-ahead, el hook de trazabilidad del depurador se suprime.
- **Rebobinado** (SPEC F-07): se almacenan savestates comprimidos periódicamente en un
  buffer circular. Al rebobinar, se restaura el savestate más cercano.
- **Netplay rollback** (SPEC F-12): cada frame se serializa el estado mínimo necesario para
  rollback. El formato compacto se definirá en el ADR de serialización (P1-021).

El formato binario versionado con migraciones queda fuera de alcance de este ADR. Se
declarará en un ADR de serialización asociado a P1-021.

### 7. Determinismo

El scheduler y todos los componentes del núcleo deben garantizar ejecución **bit-identical**
entre plataformas, compiladores y ejecuciones para la misma entrada:

**Garantías:**

1. **Sin estado global mutable**: todo el estado está encapsulado en las instancias de los
   componentes. No hay variables `static` mutables ni singletons.
2. **Sin dependencia del reloj del sistema**: el tiempo de emulación se mide exclusivamente
   en ciclos de reloj maestro. `std::chrono`, `time()`, `clock_gettime()` y similares
   están prohibidos en `core/`.
3. **Orden de ejecución determinista**: los componentes se sincronizan en un orden
   fijo de la composición estática: PPU y luego APU. No hay hilos del SO
   ni indeterminismo de planificación.
4. **Sin generadores de números aleatorios no deterministas**: si un componente necesita
   aleatoriedad (e.g., ruido de la APU), debe usar un PRNG determinista cuyo estado se
   serialice.
5. **Aritmética definida**: sin dependencia de UB, sin `float` en la ruta de emulación
   principal (la SNES usa aritmética entera de punto fijo). Esto incluye la conversión
   entre dominios de reloj CPU↔APU, que usa el acumulador racional entero de §1.

**Prohibiciones:**

- `rand()`, `std::random_device`, `/dev/urandom` en `core/`.
- `thread_local` mutable en `core/`.
- `std::unordered_map`/`std::unordered_set` donde el orden de iteración afecte el
  resultado de la emulación (usar `std::map` o arrays si el orden importa).

### 8. Restricciones de `AGENTS.md`

| Restricción | Cómo se cumple |
|-------------|---------------|
| Sin excepciones C++ en `core/` | `advance()`, `sync_all()` y `catch_up()` retornan `void`. Los invariantes (e.g., ausencia de reentrancia en `advance()`) se verifican con `assert()`. Las operaciones con fallo legítimo (deserialización, carga) usan `std::expected<T, HenoError>`. |
| Todo acceso a memoria emulada pasa por el bus | La CPU y su controlador DMA nunca leen WRAM ni ROM directamente; siempre invocan `Bus::read8()`/`Bus::write8()` (y `Bus::idle()` para ciclos internos), que llaman al scheduler y al hook del depurador. La PPU accede a VRAM directamente. El SPC700 accede a la ARAM y a sus registros a través del bus propio de la APU, disparando su propio hook de depuración (cumpliendo SPEC F-16). |
| Sin dependencias nuevas sin ADR | Este ADR no introduce dependencias externas. El scheduler se implementa en C++20 estándar. |
| `core/` no incluye Qt, SDL, Vulkan ni API de SO | Cumplido: el scheduler usa solo tipos estándar de C++20. |

### 9. Criterios de validación

El ADR se considera implementado correctamente cuando se satisfagan los siguientes
criterios, verificables por pruebas automatizadas:

1. **Test de determinismo**: ejecutar 1000 frames de una ROM homebrew dos veces con la
   misma entrada; los hashes SHA-256 de cada frame deben ser idénticos.
2. **Test de serialización round-trip**: `serialize()` seguido de `deserialize()` en
   el punto único de V-blank produce un estado que genera frames idénticos al
   estado original (hash match). Se validarán savestates mientras la CPU está ejecutando código normal y mientras está en un bucle WAI o STP.
3. **Test de compatibilidad de savestates entre perfiles**: en ambos sentidos
   (`accuracy`→`performance` y `performance`→`accuracy`), en el punto único de captura (primer límite de instrucción tras el inicio de V-blank), el savestate capturado en un perfil se carga en el otro y,
   re-serializado inmediatamente, produce bytes idénticos (todos los subordinados en
   `master_cycle`, sin ciclos pendientes). El estado cargado produce un frame válido (sin
   crash, sin corrupción visible); los frames subsiguientes pueden diferir por la
   distinta granularidad de sincronización.
4. **Test de catch-up**: un mock de subordinado (instanciado como parámetro de plantilla
   del Scheduler) verifica que, en `accuracy`, `catch_up()` se invoca con el ciclo maestro
   correcto después de cada `advance()`, incluidos los ciclos internos (`Bus::idle()`);
   y que, en `performance`, se invoca en los puntos obligatorios y en `sync_all()`.
5. **Test de MVN/MVP interrumpible**: verificar que una NMI durante una transferencia
   MVN/MVP se atiende en el límite de instrucción entre bytes y que la transferencia se
   reanuda correctamente tras el handler de NMI.
6. **Test de reentrancia prohibida**: un mock de subordinado que llama a `advance()` desde
   `catch_up()` dispara el `assert()` (death test en builds con aserciones activas).
7. **Benchmark de perfiles**: el criterio de rendimiento del §5 (performance ≥ 30 % más
   rápido que accuracy) se mide y se registra como evidencia.
8. **Test de run-ahead**: Mismos hashes de frame producidos con el run-ahead activado y desactivado, comprobado en ambos perfiles.

## Alternativas consideradas

Véase §2 para la evaluación detallada de las cuatro alternativas. Resumen:

1. **Hilos cooperativos con pila propia (libco)**: descartada por no serializable
   (estado en stack del SO), dependencia nueva y dificultad de clonación para run-ahead.
2. **Corrutinas C++20 sin pila**: descartada por no serializable (frame opaco generado
   por el compilador).
3. **CPU maestra secuencial + catch-up de subordinados** → **elegida** para `accuracy`.
4. **Lazy scanline** → **elegida** para `performance`, como política del mismo framework.

## Consecuencias

- ✅ Serialización sencilla: cada componente expone un `struct State` trivialmente
  copiable, sin pila oculta ni frames opacos. Run-ahead, rebobinado y netplay rollback son
  posibles sin hacks.
- ✅ Determinismo garantizado: sin hilos del SO, sin estado global, sin aleatoriedad no
  controlada, sin `float` en la conversión de relojes.
- ✅ Portabilidad total: C++20 estándar, sin ASM ni extensiones del compilador.
- ✅ Dos perfiles desde un mismo código fuente, con savestates compatibles gracias a
  `sync_all()` antes de capturar.
- ✅ Sin dependencias nuevas: cumple `AGENTS.md` sin excepciones.
- ✅ La CPU se escribe como código secuencial (un handler por instrucción); HDMA a mitad
  de instrucción se resuelve como llamada anidada, sin FSM.
- ✅ Sin despacho virtual en el camino crítico: PPU y APU se componen estáticamente.
- ⚠️ Mayor verbosidad en los subordinados: PPU y APU (SPC700) deben poder detenerse y
  reanudarse en cualquier ciclo objetivo, con su progreso guardado en `State`.
- ⚠️ MVN/MVP como una ejecución por byte añade complejidad al handler de estas
  instrucciones y al test de interrupciones.
- ⚠️ La composición estática hace que `Scheduler`, `Bus` y CPU se instancien una vez por
  perfil, incrementando el tamaño del binario [medir el impacto real].
- ⚠️ Los coprocesadores que acceden al bus (SA-1, Fase 2) no encajan en este modelo y
  requerirán un ADR propio.

## Documentos relacionados

- `docs/SPEC.md` §4 (Decisiones 1, 2 y 3)
- `docs/BACKLOG.md` (P1-005, P1-015, P1-020, P1-021)
- ADR-0002 — Arquitectura por capas (reglas de dependencia de capa 1)
- ADR-0003 — Contrato de la API C (serialización vía `heno.h`)
- ADR-0004 — Estrategia de implementación del intérprete de la CPU 65C816
- `docs/refs/65c816-bus.md` — Velocidades de acceso y mapa de memoria
- `docs/refs/65c816-quirks.md` — MVN/MVP, WAI, STP
- Spike `spike/P1-bus-sched` (commits `1465799^`..`beecc14`) — Prototipo evaluado (publicado como rama de archivo)
