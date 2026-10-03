<!-- SPDX-FileCopyrightText: 2026 henoSNES contributors -->
<!-- SPDX-License-Identifier: CC-BY-4.0 -->

# Referencia: Quirks y Comportamientos Especiales del 65C816

Este documento detalla comportamientos especiales, "quirks", y edge cases requeridos para alcanzar una emulación ciclo-exacta del 65C816 en el entorno de la SNES.

## 1. Modos de Operación e Instrucción XCE

El 65C816 puede operar en dos modos principales determinados por el bit `E` (Emulation mode), el cual es inaccesible directamente pero intercambiable mediante la instrucción `XCE` (Exchange Carry and Emulation).
- Al cambiar del modo Nativo (E=0) al modo Emulación (E=1):
  - Los registros X e Y se truncan forzosamente a 8 bits (el byte alto `$XX00` se pierde y los flags X/M internos operan en modo 8 bits).
  - El Stack Pointer (`S`) fuerza su byte alto a `$01` (comportamiento de página fija del 6502 original).
- Al cambiar del modo Emulación (E=1) al modo Nativo (E=0):
  - Los índices retienen su valor (no cambian automáticamente a 16 bits completos hasta que se modifique el bit X o M en el registro de estatus P mediante `REP`/`SEP`).

## 2. Instrucciones WAI (Wait for Interrupt) y STP (Stop)

- **WAI (`$CB`)**: Detiene la ejecución del procesador hasta que se recibe una interrupción de hardware (NMI, IRQ, RESET).
  - Reduce la latencia de la interrupción en un ciclo, ya que el procesador está esperando en lugar de tener que terminar una instrucción en curso.
  - El procesador permanece internamente en un estado de lectura de 3 ciclos.
- **STP (`$DB`)**: Detiene completamente el reloj interno del procesador.
  - En la SNES, ejecutar `STP` bloquea irremediablemente el sistema hasta un reset por hardware (el cual no es comúnmente invocado en software normal).

## 3. Comportamiento de BRK y COP

Las instrucciones por software `BRK` (`$00`) y `COP` (`$02`) disparan vectores de interrupción distintos dependiendo del modo de emulación:
- **En modo Emulación (E=1):**
  - Ambos comparten los vectores del 6502 (`$FFFE` para `BRK`, `$FFF4` para `COP`).
  - No pushean el registro de banco de programa (PB).
- **En modo Nativo (E=0):**
  - Tienen vectores separados (`$FFE6` para `BRK`, `$FFE4` para `COP`).
  - Pushean el registro PB (3 bytes totales de PC + PB, más 1 byte de status).
  - A diferencia de las IRQ de hardware, el bit `B` (Break) invisible en el registro de status pusheado permite diferenciar si fue un interrupción de software o hardware.

## 4. Página Directa No Alineada (+1 Ciclo)

El registro `D` (Direct Page Register, 16 bits) define el inicio de la "Página Cero".
- A diferencia del 6502 que forzaba la página a `$0000`, el 65C816 puede moverla a cualquier dirección.
- Si el byte bajo (DL) del registro `D` no es cero (`DL != 0x00`), **se añade un ciclo de penalización** a cualquier instrucción que acceda a la Página Directa, debido al acarreo que se produce en la ALU para calcular la dirección.

## 5. Truncado de Índices (M/X Flags)

El comportamiento al envolver (wrap) direcciones varía:
- En modo 8 bits de índice (X=1), si se indexa fuera del banco de Página Directa, la dirección **envuelve** dentro del banco cero (`$0000-$00FF`), sin cruzar a páginas superiores.
- En modo 16 bits, la indexación suma linealmente y puede cruzar el límite de la página, incurriendo a veces en el ciclo de penalización por "page cross".

## 6. Instrucciones de Bloque MVN y MVP

- Transfieren bloques de memoria (hasta 64KB) byte a byte.
- Toman 7 ciclos *por cada byte* transferido.
- Se ven interrumpidas por NMI/IRQ, tras lo cual la CPU reanuda la misma instrucción actualizando los registros A, X e Y, que actúan como contadores/punteros en vivo.
- El Banco de Origen y Destino son argumentos de la instrucción (operando de 2 bytes), mientras que X, Y, y C acumulan la lógica de puntero y contador.

## 7. Lecturas Falsas (Dummy Reads)

Para mantener retrocompatibilidad ciclo a ciclo con el 6502 (y el bus timing estricto), el 65C816 a menudo realiza "lecturas basura" (dummy reads) antes de una escritura o durante cruces de página.
- Esto es crítico en la SNES: una dummy read a un registro de un coprocesador o puerto I/O que tiene efectos secundarios de "lectura destructiva" (read-to-clear) puede consumir un flag prematuramente.
- Las dummy reads ocurren típicamente en el penúltimo ciclo de instrucciones RMW (Read-Modify-Write) indexadas como `INC a,X` o durante saltos relativos que cruzan límites de página.
