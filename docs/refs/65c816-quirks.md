<!-- SPDX-FileCopyrightText: 2026 henoSNES contributors -->
<!-- SPDX-License-Identifier: CC-BY-4.0 -->

# Referencia: Quirks y comportamientos especiales del 65C816

Comportamientos especiales del 65C816 necesarios para una emulación ciclo a ciclo en la SNES. Los ciclos base y los modificadores de cada opcode están en `65c816-opcodes.md`.

## 1. Modo emulación, modo nativo y XCE

- El flag E no forma parte de P: solo se cambia con XCE, que intercambia C y E. RESET pone E = 1; no existe vector de RESET en modo nativo.
- Al entrar en modo emulación (E: 0 → 1): M y X quedan forzados en 1, los bytes altos de X e Y pasan a 0 y el byte alto de S pasa a `$01`. D, DBR y PBR conservan su valor.
- Al volver a modo nativo (E: 1 → 0): M y X siguen en 1 (8 bits) hasta que REP o PLP los cambien; los registros conservan su valor.
- En modo emulación, el bit 4 de P no es X: al apilar P (BRK, PHP) ese bit es B.

## 2. Envolvimiento de direcciones

- El PC es de 16 bits: al pasar de `$FFFF` vuelve a `$0000` sin cambiar PBR, y los saltos relativos envuelven dentro del banco de programa.
- `JMP (a)` y `JML [a]` leen su puntero en el banco 0; `JMP (a,X)` y `JSR (a,X)` lo leen en el banco de programa. La hoja de datos de WDC (§3.5.2) indica banco 0 para `(a,X)`; se sigue el documento de B. Clark, probado sobre hardware.
- En modo emulación con DL = `$00`, `(d,X)` lee los dos bytes del puntero dentro de la página directa.
- Página directa en modo nativo: la dirección (D + desplazamiento + índice) se calcula en 16 bits, puede cruzar páginas y queda confinada al banco 0.
- Página directa en modo emulación: si DL = `$00`, el direccionamiento directo indexado de los modos heredados del 6502 envuelve dentro de la página (`$xx00-$xxFF`). Si DL ≠ `$00`, no envuelve por página. Las instrucciones y modos nuevos del 65C816 no envuelven por página: `[d]`, `[d],Y` y PEI leen su puntero de forma continua aunque DL = `$00` (B. Clark, §5.1.1).
- Pila: en modo emulación queda confinada a la página 1 (`$0100-$01FF`) para las instrucciones heredadas; en modo nativo S es de 16 bits dentro del banco 0.
- Con índices de 8 bits (X = 1), los bytes altos de X e Y valen 0. No implica envolvimiento por página fuera de lo descrito arriba.

## 3. Ciclos extra

- DL ≠ `$00`: +1 ciclo en todos los modos de página directa (Tabla 5-7, nota 2).
- Indexación absoluta y `(d),y` en lecturas: +1 ciclo si se cruza una página o si X = 0 (nota 4). Las escrituras y las instrucciones de lectura-modificación-escritura ya incluyen ese ciclo en su base.
- Saltos condicionales: +1 ciclo si el salto se toma (nota 5); en modo emulación, +1 adicional si el salto tomado cruza página (nota 6). En modo nativo no hay penalización por cruce de página. BRA siempre se toma.

## 4. BRK, COP e interrupciones

- Vectores en modo nativo (banco `$00`): COP `$FFE4`, BRK `$FFE6`, ABORT `$FFE8` (sin uso en el 5A22), NMI `$FFEA`, IRQ `$FFEE`.
- Vectores en modo emulación: COP `$FFF4`, ABORT `$FFF8` (sin uso), NMI `$FFFA`, RESET `$FFFC`, IRQ y BRK comparten `$FFFE`.
- Todos los vectores son de 16 bits y el banco destino se fuerza a `$00`.
- BRK y COP avanzan PC en 2 (el byte siguiente es un byte de firma); la dirección apilada es la de la instrucción + 2.
- En modo nativo se apilan PBR, PCH, PCL y P (4 bytes). En modo emulación se apilan PCH, PCL y P (3 bytes), sin PBR: un BRK o COP fuera del banco 0 no puede volver correctamente.
- En modo emulación, el bit B del P apilado vale 1 para BRK y 0 para IRQ, para distinguirlos porque comparten vector. En modo nativo no existe B: BRK tiene vector propio y el bit 4 de P es X.
- Toda interrupción pone I = 1. En modo nativo también pone D = 0 [verificar el comportamiento de D en modo emulación].

## 5. WAI y STP

- WAI (`$CB`): detiene la ejecución hasta que llega NMI, IRQ o RESET. Si I = 1 y llega una IRQ, la CPU no la atiende: continúa con la instrucción siguiente. Permite responder a la interrupción con menor latencia [verificar la magnitud exacta].
- STP (`$DB`): detiene el reloj interno de la CPU hasta un RESET.
- El modelado en el emulador (flags `waiting` y `stopped` en el estado de la CPU) está definido en ADR-0004 y ADR-0005.

## 6. MVN y MVP

- Codificación: opcode, banco destino, banco origen. La sintaxis habitual de ensamblador es `MVN origen,destino` [verificar el orden según el ensamblador usado en las pruebas].
- C (el acumulador de 16 bits, sea cual sea M) contiene la cantidad de bytes menos 1; X la dirección de origen; Y la dirección de destino. La instrucción deja DBR con el banco destino.
- Cada byte cuesta 7 ciclos. MVN incrementa X e Y; MVP los decrementa; ambos decrementan C. La instrucción se vuelve a ejecutar (PC no avanza) hasta que C pasa de `$0000` a `$FFFF`.
- Es interrumpible entre bytes y se reanuda después de la interrupción.
- En modo emulación, X e Y son de 8 bits, lo que limita la transferencia a la página 0.

## 7. Ciclos internos y lecturas falsas

- Los ciclos internos del 65C816 no son accesos a memoria. En el S-CPU duran 6 ciclos maestros (ver `65c816-bus.md`).
- La idea de "lecturas falsas" en el penúltimo ciclo de las instrucciones indexadas de lectura-modificación-escritura proviene del 6502 NMOS. En el 65C816 en modo nativo, ese ciclo es interno. El comportamiento exacto de esas instrucciones en modo emulación (posibles escrituras dobles del valor original) queda [verificar] contra la Tabla 5-7 y suites de prueba con actividad de bus.

## Fuentes

- WDC, W65C816S Datasheet, 2024-03-13 (Tablas 5-4 y 5-7, con sus notas), descripción del Program Bank Register y §3.5.2.
- SNESdev Wiki, "CPU vectors" (https://snes.nesdev.org/wiki/CPU_vectors), contenido CC0.
- SNESdev Wiki, "Signature byte" (https://snes.nesdev.org/wiki/Signature_byte), contenido CC0.
- B. Clark, "65C816 Opcodes", tutorial de 6502.org (http://6502.org/tutorials/65c816opcodes.html).

*Nota de verificación: contenido verificado el 2026-10-03 contra las fuentes listadas. Los puntos marcados [verificar] siguen pendientes de confirmación.*
