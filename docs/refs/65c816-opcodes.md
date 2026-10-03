<!-- SPDX-FileCopyrightText: 2026 henoSNES contributors -->
<!-- SPDX-License-Identifier: CC-BY-4.0 -->

# Referencia 65C816: Tabla de Opcodes

**Tabla verificada el 2026-10-03 contra la hoja de datos WDC W65C816S del 2024-03-13 (Tablas 5-4 y 5-7).**

Esta tabla documenta los 256 opcodes del procesador 65C816 y sirve como única fuente de verdad para la emulación ciclo a ciclo.

## Modificadores de Ciclo (Condiciones)
*Nota de convención: los ciclos base siguen la Tabla 5-4 del datasheet (M=1, X=1, saltos no tomados, sin cruce de página, BRK y COP en modo emulación), con dos excepciones documentadas: RTI usa base de modo emulación (6 ciclos, +1 en modo nativo mediante "e") y BRA usa 3 ciclos porque siempre salta.*

- **m2**: +2 ciclos si el flag M está en 0 (acumulador/memoria de 16 bits)
- **m**: +1 ciclo si el flag M está en 0 (acumulador/memoria de 16 bits)
- **x**: +1 ciclo si el flag X está en 0 (índice de 16 bits)
- **e**: +1 ciclo en modo nativo (flag E=0)
- **p***: +1 ciclo si hay cruce de página o si el flag X está en 0 (índice de 16 bits)
- **p**: +1 ciclo si hay cruce de página (aplica en índices de 16 bits)
- **d**: +1 ciclo si la Página Directa no está alineada a página (DL != 0)
- **b**: +1 ciclo si el salto (branch) es tomado; +1 extra si hay cruce de página (solo en modo emulación)
- **s**: ciclo atado a velocidad de la memoria de acuerdo a su dominio

| Opcode | Mnemónico | Modo de Direccionamiento | Bytes | Ciclos Base | Condiciones Extra | Fuente |
| --- | --- | --- | --- | --- | --- | --- |
| `$00` | BRK | s | 2 | 7 | e | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$01` | ORA | (d,X) | 2 | 6 | m,d | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$02` | COP | s | 2 | 7 | e | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$03` | ORA | d,S | 2 | 4 | m | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$04` | TSB | d | 2 | 5 | m2,d | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$05` | ORA | d | 2 | 3 | m,d | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$06` | ASL | d | 2 | 5 | m2,d | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$07` | ORA | [d] | 2 | 6 | m,d | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$08` | PHP | s | 1 | 3 |  | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$09` | ORA | # | 2 | 2 | m | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$0A` | ASL | A | 1 | 2 |  | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$0B` | PHD | s | 1 | 4 |  | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$0C` | TSB | a | 3 | 6 | m2 | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$0D` | ORA | a | 3 | 4 | m | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$0E` | ASL | a | 3 | 6 | m2 | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$0F` | ORA | al | 4 | 5 | m | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$10` | BPL | r | 2 | 2 | b | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$11` | ORA | (d),Y | 2 | 5 | m,p*,d | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$12` | ORA | (d) | 2 | 5 | m,d | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$13` | ORA | (d,S),Y | 2 | 7 | m | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$14` | TRB | d | 2 | 5 | m2,d | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$15` | ORA | d,X | 2 | 4 | m,d | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$16` | ASL | d,X | 2 | 6 | m2,d | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$17` | ORA | [d],Y | 2 | 6 | m,d | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$18` | CLC | i | 1 | 2 |  | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$19` | ORA | a,Y | 3 | 4 | m,p* | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$1A` | INC | A | 1 | 2 |  | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$1B` | TCS | i | 1 | 2 |  | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$1C` | TRB | a | 3 | 6 | m2 | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$1D` | ORA | a,X | 3 | 4 | m,p* | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$1E` | ASL | a,X | 3 | 7 | m2 | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$1F` | ORA | al,X | 4 | 5 | m | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$20` | JSR | a | 3 | 6 |  | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$21` | AND | (d,X) | 2 | 6 | m,d | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$22` | JSL | al | 4 | 8 |  | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$23` | AND | d,S | 2 | 4 | m | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$24` | BIT | d | 2 | 3 | m,d | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$25` | AND | d | 2 | 3 | m,d | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$26` | ROL | d | 2 | 5 | m2,d | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$27` | AND | [d] | 2 | 6 | m,d | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$28` | PLP | s | 1 | 4 |  | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$29` | AND | # | 2 | 2 | m | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$2A` | ROL | A | 1 | 2 |  | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$2B` | PLD | s | 1 | 5 |  | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$2C` | BIT | a | 3 | 4 | m | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$2D` | AND | a | 3 | 4 | m | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$2E` | ROL | a | 3 | 6 | m2 | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$2F` | AND | al | 4 | 5 | m | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$30` | BMI | r | 2 | 2 | b | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$31` | AND | (d),Y | 2 | 5 | m,p*,d | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$32` | AND | (d) | 2 | 5 | m,d | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$33` | AND | (d,S),Y | 2 | 7 | m | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$34` | BIT | d,X | 2 | 4 | m,d | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$35` | AND | d,X | 2 | 4 | m,d | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$36` | ROL | d,X | 2 | 6 | m2,d | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$37` | AND | [d],Y | 2 | 6 | m,d | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$38` | SEC | i | 1 | 2 |  | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$39` | AND | a,Y | 3 | 4 | m,p* | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$3A` | DEC | A | 1 | 2 |  | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$3B` | TSC | i | 1 | 2 |  | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$3C` | BIT | a,X | 3 | 4 | m,p* | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$3D` | AND | a,X | 3 | 4 | m,p* | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$3E` | ROL | a,X | 3 | 7 | m2 | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$3F` | AND | al,X | 4 | 5 | m | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$40` | RTI | s | 1 | 6 | e | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$41` | EOR | (d,X) | 2 | 6 | m,d | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$42` | WDM | i | 2 | 2 |  | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$43` | EOR | d,S | 2 | 4 | m | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$44` | MVP | xyc | 3 | 7/byte |  | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$45` | EOR | d | 2 | 3 | m,d | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$46` | LSR | d | 2 | 5 | m2,d | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$47` | EOR | [d] | 2 | 6 | m,d | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$48` | PHA | s | 1 | 3 | m | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$49` | EOR | # | 2 | 2 | m | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$4A` | LSR | A | 1 | 2 |  | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$4B` | PHK | s | 1 | 3 |  | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$4C` | JMP | a | 3 | 3 |  | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$4D` | EOR | a | 3 | 4 | m | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$4E` | LSR | a | 3 | 6 | m2 | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$4F` | EOR | al | 4 | 5 | m | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$50` | BVC | r | 2 | 2 | b | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$51` | EOR | (d),Y | 2 | 5 | m,p*,d | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$52` | EOR | (d) | 2 | 5 | m,d | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$53` | EOR | (d,S),Y | 2 | 7 | m | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$54` | MVN | xyc | 3 | 7/byte |  | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$55` | EOR | d,X | 2 | 4 | m,d | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$56` | LSR | d,X | 2 | 6 | m2,d | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$57` | EOR | [d],Y | 2 | 6 | m,d | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$58` | CLI | i | 1 | 2 |  | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$59` | EOR | a,Y | 3 | 4 | m,p* | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$5A` | PHY | s | 1 | 3 | x | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$5B` | TCD | i | 1 | 2 |  | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$5C` | JMP | al | 4 | 4 |  | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$5D` | EOR | a,X | 3 | 4 | m,p* | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$5E` | LSR | a,X | 3 | 7 | m2 | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$5F` | EOR | al,X | 4 | 5 | m | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$60` | RTS | s | 1 | 6 |  | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$61` | ADC | (d,X) | 2 | 6 | m,d | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$62` | PER | s | 3 | 6 |  | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$63` | ADC | d,S | 2 | 4 | m | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$64` | STZ | d | 2 | 3 | m,d | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$65` | ADC | d | 2 | 3 | m,d | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$66` | ROR | d | 2 | 5 | m2,d | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$67` | ADC | [d] | 2 | 6 | m,d | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$68` | PLA | s | 1 | 4 | m | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$69` | ADC | # | 2 | 2 | m | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$6A` | ROR | A | 1 | 2 |  | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$6B` | RTL | s | 1 | 6 |  | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$6C` | JMP | (a) | 3 | 5 |  | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$6D` | ADC | a | 3 | 4 | m | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$6E` | ROR | a | 3 | 6 | m2 | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$6F` | ADC | al | 4 | 5 | m | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$70` | BVS | r | 2 | 2 | b | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$71` | ADC | (d),Y | 2 | 5 | m,p*,d | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$72` | ADC | (d) | 2 | 5 | m,d | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$73` | ADC | (d,S),Y | 2 | 7 | m | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$74` | STZ | d,X | 2 | 4 | m,d | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$75` | ADC | d,X | 2 | 4 | m,d | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$76` | ROR | d,X | 2 | 6 | m2,d | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$77` | ADC | [d],Y | 2 | 6 | m,d | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$78` | SEI | i | 1 | 2 |  | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$79` | ADC | a,Y | 3 | 4 | m,p* | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$7A` | PLY | s | 1 | 4 | x | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$7B` | TDC | i | 1 | 2 |  | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$7C` | JMP | (a,X) | 3 | 6 |  | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$7D` | ADC | a,X | 3 | 4 | m,p* | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$7E` | ROR | a,X | 3 | 7 | m2 | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$7F` | ADC | al,X | 4 | 5 | m | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$80` | BRA | r | 2 | 3 | +1 si E=1 y cruza pág | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$81` | STA | (d,X) | 2 | 6 | m,d | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$82` | BRL | rl | 3 | 4 |  | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$83` | STA | d,S | 2 | 4 | m | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$84` | STY | d | 2 | 3 | x,d | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$85` | STA | d | 2 | 3 | m,d | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$86` | STX | d | 2 | 3 | x,d | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$87` | STA | [d] | 2 | 6 | m,d | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 (errata en T5-4: indica 2,2) |
| `$88` | DEY | i | 1 | 2 |  | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$89` | BIT | # | 2 | 2 | m | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$8A` | TXA | i | 1 | 2 |  | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$8B` | PHB | s | 1 | 3 |  | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$8C` | STY | a | 3 | 4 | x | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$8D` | STA | a | 3 | 4 | m | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$8E` | STX | a | 3 | 4 | x | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$8F` | STA | al | 4 | 5 | m | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$90` | BCC | r | 2 | 2 | b | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$91` | STA | (d),Y | 2 | 6 | m,d | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$92` | STA | (d) | 2 | 5 | m,d | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$93` | STA | (d,S),Y | 2 | 7 | m | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$94` | STY | d,X | 2 | 4 | x,d | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$95` | STA | d,X | 2 | 4 | m,d | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$96` | STX | d,Y | 2 | 4 | x,d | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$97` | STA | [d],Y | 2 | 6 | m,d | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$98` | TYA | i | 1 | 2 |  | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$99` | STA | a,Y | 3 | 5 | m | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$9A` | TXS | i | 1 | 2 |  | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$9B` | TXY | i | 1 | 2 |  | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$9C` | STZ | a | 3 | 4 | m | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$9D` | STA | a,X | 3 | 5 | m | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$9E` | STZ | a,X | 3 | 5 | m | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$9F` | STA | al,X | 4 | 5 | m | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$A0` | LDY | # | 2 | 2 | x | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$A1` | LDA | (d,X) | 2 | 6 | m,d | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$A2` | LDX | # | 2 | 2 | x | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$A3` | LDA | d,S | 2 | 4 | m | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$A4` | LDY | d | 2 | 3 | x,d | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$A5` | LDA | d | 2 | 3 | m,d | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$A6` | LDX | d | 2 | 3 | x,d | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$A7` | LDA | [d] | 2 | 6 | m,d | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$A8` | TAY | i | 1 | 2 |  | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$A9` | LDA | # | 2 | 2 | m | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$AA` | TAX | i | 1 | 2 |  | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$AB` | PLB | s | 1 | 4 |  | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$AC` | LDY | a | 3 | 4 | x | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$AD` | LDA | a | 3 | 4 | m | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$AE` | LDX | a | 3 | 4 | x | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$AF` | LDA | al | 4 | 5 | m | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$B0` | BCS | r | 2 | 2 | b | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$B1` | LDA | (d),Y | 2 | 5 | m,p*,d | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$B2` | LDA | (d) | 2 | 5 | m,d | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$B3` | LDA | (d,S),Y | 2 | 7 | m | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$B4` | LDY | d,X | 2 | 4 | x,d | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$B5` | LDA | d,X | 2 | 4 | m,d | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$B6` | LDX | d,Y | 2 | 4 | x,d | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$B7` | LDA | [d],Y | 2 | 6 | m,d | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$B8` | CLV | i | 1 | 2 |  | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$B9` | LDA | a,Y | 3 | 4 | m,p* | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$BA` | TSX | i | 1 | 2 |  | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$BB` | TYX | i | 1 | 2 |  | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$BC` | LDY | a,X | 3 | 4 | x,p* | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$BD` | LDA | a,X | 3 | 4 | m,p* | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$BE` | LDX | a,Y | 3 | 4 | x,p* | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$BF` | LDA | al,X | 4 | 5 | m | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$C0` | CPY | # | 2 | 2 | x | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$C1` | CMP | (d,X) | 2 | 6 | m,d | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$C2` | REP | # | 2 | 3 |  | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$C3` | CMP | d,S | 2 | 4 | m | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$C4` | CPY | d | 2 | 3 | x,d | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$C5` | CMP | d | 2 | 3 | m,d | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$C6` | DEC | d | 2 | 5 | m2,d | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$C7` | CMP | [d] | 2 | 6 | m,d | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$C8` | INY | i | 1 | 2 |  | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$C9` | CMP | # | 2 | 2 | m | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$CA` | DEX | i | 1 | 2 |  | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$CB` | WAI | i | 1 | 3 |  | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$CC` | CPY | a | 3 | 4 | x | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$CD` | CMP | a | 3 | 4 | m | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$CE` | DEC | a | 3 | 6 | m2 | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$CF` | CMP | al | 4 | 5 | m | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$D0` | BNE | r | 2 | 2 | b | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$D1` | CMP | (d),Y | 2 | 5 | m,p*,d | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$D2` | CMP | (d) | 2 | 5 | m,d | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$D3` | CMP | (d,S),Y | 2 | 7 | m | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$D4` | PEI | s | 2 | 6 | d | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$D5` | CMP | d,X | 2 | 4 | m,d | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$D6` | DEC | d,X | 2 | 6 | m2,d | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$D7` | CMP | [d],Y | 2 | 6 | m,d | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$D8` | CLD | i | 1 | 2 |  | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$D9` | CMP | a,Y | 3 | 4 | m,p* | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$DA` | PHX | s | 1 | 3 | x | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$DB` | STP | i | 1 | 3 |  | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$DC` | JML | (a) | 3 | 6 |  | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$DD` | CMP | a,X | 3 | 4 | m,p* | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$DE` | DEC | a,X | 3 | 7 | m2 | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$DF` | CMP | al,X | 4 | 5 | m | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$E0` | CPX | # | 2 | 2 | x | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$E1` | SBC | (d,X) | 2 | 6 | m,d | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$E2` | SEP | # | 2 | 3 |  | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$E3` | SBC | d,S | 2 | 4 | m | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$E4` | CPX | d | 2 | 3 | x,d | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$E5` | SBC | d | 2 | 3 | m,d | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$E6` | INC | d | 2 | 5 | m2,d | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$E7` | SBC | [d] | 2 | 6 | m,d | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$E8` | INX | i | 1 | 2 |  | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$E9` | SBC | # | 2 | 2 | m | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$EA` | NOP | i | 1 | 2 |  | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$EB` | XBA | i | 1 | 3 |  | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$EC` | CPX | a | 3 | 4 | x | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$ED` | SBC | a | 3 | 4 | m | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$EE` | INC | a | 3 | 6 | m2 | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$EF` | SBC | al | 4 | 5 | m | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$F0` | BEQ | r | 2 | 2 | b | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$F1` | SBC | (d),Y | 2 | 5 | m,p*,d | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$F2` | SBC | (d) | 2 | 5 | m,d | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$F3` | SBC | (d,S),Y | 2 | 7 | m | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$F4` | PEA | s | 3 | 5 |  | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$F5` | SBC | d,X | 2 | 4 | m,d | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$F6` | INC | d,X | 2 | 6 | m2,d | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$F7` | SBC | [d],Y | 2 | 6 | m,d | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$F8` | SED | i | 1 | 2 |  | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$F9` | SBC | a,Y | 3 | 4 | m,p* | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$FA` | PLX | s | 1 | 4 | x | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$FB` | XCE | i | 1 | 2 |  | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$FC` | JSR | (a,X) | 3 | 8 |  | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$FD` | SBC | a,X | 3 | 4 | m,p* | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$FE` | INC | a,X | 3 | 7 | m2 | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |
| `$FF` | SBC | al,X | 4 | 5 | m | WDC W65C816S datasheet 2024-03-13, T5-4 y T5-7 |

## Resumen
- **Filas verificadas:** 0
- **Filas pendientes de verificación:** 256
