<!-- SPDX-FileCopyrightText: 2026 henoSNES contributors -->
<!-- SPDX-License-Identifier: CC-BY-4.0 -->

# Referencia 65C816: Tabla de Opcodes

Esta tabla documenta los 256 opcodes del procesador 65C816 y sirve como única fuente de verdad para la emulación ciclo a ciclo.

## Modificadores de Ciclo (Condiciones)
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
| `$00` | BRK | s | 2 | 7 | e | derivada del modelo, pendiente de verificación |
| `$01` | ORA | (d,X) | 2 | 6 | m,d | derivada del modelo, pendiente de verificación |
| `$02` | COP | s | 2 | 7 | e | derivada del modelo, pendiente de verificación |
| `$03` | ORA | d,S | 2 | 4 | m | derivada del modelo, pendiente de verificación |
| `$04` | TSB | d | 2 | 5 | m2,d | derivada del modelo, pendiente de verificación |
| `$05` | ORA | d | 2 | 3 | m,d | derivada del modelo, pendiente de verificación |
| `$06` | ASL | d | 2 | 5 | m2,d | derivada del modelo, pendiente de verificación |
| `$07` | ORA | [d] | 2 | 6 | m,d | derivada del modelo, pendiente de verificación |
| `$08` | PHP | s | 1 | 3 |  | derivada del modelo, pendiente de verificación |
| `$09` | ORA | # | 2 | 2 | m | derivada del modelo, pendiente de verificación |
| `$0A` | ASL | A | 1 | 2 |  | derivada del modelo, pendiente de verificación |
| `$0B` | PHD | s | 1 | 4 |  | derivada del modelo, pendiente de verificación |
| `$0C` | TSB | a | 3 | 6 | m2 | derivada del modelo, pendiente de verificación |
| `$0D` | ORA | a | 3 | 4 | m | derivada del modelo, pendiente de verificación |
| `$0E` | ASL | a | 3 | 6 | m2 | derivada del modelo, pendiente de verificación |
| `$0F` | ORA | al | 4 | 5 | m | derivada del modelo, pendiente de verificación |
| `$10` | BPL | r | 2 | 2 | b | derivada del modelo, pendiente de verificación |
| `$11` | ORA | (d),Y | 2 | 5 | m,p*,d | derivada del modelo, pendiente de verificación |
| `$12` | ORA | (d) | 2 | 5 | m,d | derivada del modelo, pendiente de verificación |
| `$13` | ORA | (d,S),Y | 2 | 7 | m | derivada del modelo, pendiente de verificación |
| `$14` | TRB | d | 2 | 5 | m2,d | derivada del modelo, pendiente de verificación |
| `$15` | ORA | d,X | 2 | 4 | m,d | derivada del modelo, pendiente de verificación |
| `$16` | ASL | d,X | 2 | 6 | m2,d | derivada del modelo, pendiente de verificación |
| `$17` | ORA | [d],Y | 2 | 6 | m,d | derivada del modelo, pendiente de verificación |
| `$18` | CLC | i | 1 | 2 |  | derivada del modelo, pendiente de verificación |
| `$19` | ORA | a,Y | 3 | 4 | m,p* | derivada del modelo, pendiente de verificación |
| `$1A` | INC | A | 1 | 2 |  | derivada del modelo, pendiente de verificación |
| `$1B` | TCS | i | 1 | 2 |  | derivada del modelo, pendiente de verificación |
| `$1C` | TRB | a | 3 | 6 | m2 | derivada del modelo, pendiente de verificación |
| `$1D` | ORA | a,X | 3 | 4 | m,p* | derivada del modelo, pendiente de verificación |
| `$1E` | ASL | a,X | 3 | 7 | m2 | derivada del modelo, pendiente de verificación |
| `$1F` | ORA | al,X | 4 | 5 | m | derivada del modelo, pendiente de verificación |
| `$20` | JSR | a | 3 | 6 |  | derivada del modelo, pendiente de verificación |
| `$21` | AND | (d,X) | 2 | 6 | m,d | derivada del modelo, pendiente de verificación |
| `$22` | JSL | al | 4 | 8 |  | derivada del modelo, pendiente de verificación |
| `$23` | AND | d,S | 2 | 4 | m | derivada del modelo, pendiente de verificación |
| `$24` | BIT | d | 2 | 3 | m,d | derivada del modelo, pendiente de verificación |
| `$25` | AND | d | 2 | 3 | m,d | derivada del modelo, pendiente de verificación |
| `$26` | ROL | d | 2 | 5 | m2,d | derivada del modelo, pendiente de verificación |
| `$27` | AND | [d] | 2 | 6 | m,d | derivada del modelo, pendiente de verificación |
| `$28` | PLP | s | 1 | 4 |  | derivada del modelo, pendiente de verificación |
| `$29` | AND | # | 2 | 2 | m | derivada del modelo, pendiente de verificación |
| `$2A` | ROL | A | 1 | 2 |  | derivada del modelo, pendiente de verificación |
| `$2B` | PLD | s | 1 | 5 |  | derivada del modelo, pendiente de verificación |
| `$2C` | BIT | a | 3 | 4 | m | derivada del modelo, pendiente de verificación |
| `$2D` | AND | a | 3 | 4 | m | derivada del modelo, pendiente de verificación |
| `$2E` | ROL | a | 3 | 6 | m2 | derivada del modelo, pendiente de verificación |
| `$2F` | AND | al | 4 | 5 | m | derivada del modelo, pendiente de verificación |
| `$30` | BMI | r | 2 | 2 | b | derivada del modelo, pendiente de verificación |
| `$31` | AND | (d),Y | 2 | 5 | m,p*,d | derivada del modelo, pendiente de verificación |
| `$32` | AND | (d) | 2 | 5 | m,d | derivada del modelo, pendiente de verificación |
| `$33` | AND | (d,S),Y | 2 | 7 | m | derivada del modelo, pendiente de verificación |
| `$34` | BIT | d,X | 2 | 4 | m,d | derivada del modelo, pendiente de verificación |
| `$35` | AND | d,X | 2 | 4 | m,d | derivada del modelo, pendiente de verificación |
| `$36` | ROL | d,X | 2 | 6 | m2,d | derivada del modelo, pendiente de verificación |
| `$37` | AND | [d],Y | 2 | 6 | m,d | derivada del modelo, pendiente de verificación |
| `$38` | SEC | i | 1 | 2 |  | derivada del modelo, pendiente de verificación |
| `$39` | AND | a,Y | 3 | 4 | m,p* | derivada del modelo, pendiente de verificación |
| `$3A` | DEC | A | 1 | 2 |  | derivada del modelo, pendiente de verificación |
| `$3B` | TSC | i | 1 | 2 |  | derivada del modelo, pendiente de verificación |
| `$3C` | BIT | a,X | 3 | 4 | m,p* | derivada del modelo, pendiente de verificación |
| `$3D` | AND | a,X | 3 | 4 | m,p* | derivada del modelo, pendiente de verificación |
| `$3E` | ROL | a,X | 3 | 7 | m2 | derivada del modelo, pendiente de verificación |
| `$3F` | AND | al,X | 4 | 5 | m | derivada del modelo, pendiente de verificación |
| `$40` | RTI | s | 1 | 6 | e | derivada del modelo, pendiente de verificación |
| `$41` | EOR | (d,X) | 2 | 6 | m,d | derivada del modelo, pendiente de verificación |
| `$42` | WDM | i | 2 | 2 |  | derivada del modelo, pendiente de verificación |
| `$43` | EOR | d,S | 2 | 4 | m | derivada del modelo, pendiente de verificación |
| `$44` | MVP | xyc | 3 | 7/byte |  | derivada del modelo, pendiente de verificación |
| `$45` | EOR | d | 2 | 3 | m,d | derivada del modelo, pendiente de verificación |
| `$46` | LSR | d | 2 | 5 | m2,d | derivada del modelo, pendiente de verificación |
| `$47` | EOR | [d] | 2 | 6 | m,d | derivada del modelo, pendiente de verificación |
| `$48` | PHA | s | 1 | 3 | m | derivada del modelo, pendiente de verificación |
| `$49` | EOR | # | 2 | 2 | m | derivada del modelo, pendiente de verificación |
| `$4A` | LSR | A | 1 | 2 |  | derivada del modelo, pendiente de verificación |
| `$4B` | PHK | s | 1 | 3 |  | derivada del modelo, pendiente de verificación |
| `$4C` | JMP | a | 3 | 3 |  | derivada del modelo, pendiente de verificación |
| `$4D` | EOR | a | 3 | 4 | m | derivada del modelo, pendiente de verificación |
| `$4E` | LSR | a | 3 | 6 | m2 | derivada del modelo, pendiente de verificación |
| `$4F` | EOR | al | 4 | 5 | m | derivada del modelo, pendiente de verificación |
| `$50` | BVC | r | 2 | 2 | b | derivada del modelo, pendiente de verificación |
| `$51` | EOR | (d),Y | 2 | 5 | m,p*,d | derivada del modelo, pendiente de verificación |
| `$52` | EOR | (d) | 2 | 5 | m,d | derivada del modelo, pendiente de verificación |
| `$53` | EOR | (d,S),Y | 2 | 7 | m | derivada del modelo, pendiente de verificación |
| `$54` | MVN | xyc | 3 | 7/byte |  | derivada del modelo, pendiente de verificación |
| `$55` | EOR | d,X | 2 | 4 | m,d | derivada del modelo, pendiente de verificación |
| `$56` | LSR | d,X | 2 | 6 | m2,d | derivada del modelo, pendiente de verificación |
| `$57` | EOR | [d],Y | 2 | 6 | m,d | derivada del modelo, pendiente de verificación |
| `$58` | CLI | i | 1 | 2 |  | derivada del modelo, pendiente de verificación |
| `$59` | EOR | a,Y | 3 | 4 | m,p* | derivada del modelo, pendiente de verificación |
| `$5A` | PHY | s | 1 | 3 | x | derivada del modelo, pendiente de verificación |
| `$5B` | TCD | i | 1 | 2 |  | derivada del modelo, pendiente de verificación |
| `$5C` | JMP | al | 4 | 4 |  | derivada del modelo, pendiente de verificación |
| `$5D` | EOR | a,X | 3 | 4 | m,p* | derivada del modelo, pendiente de verificación |
| `$5E` | LSR | a,X | 3 | 7 | m2 | derivada del modelo, pendiente de verificación |
| `$5F` | EOR | al,X | 4 | 5 | m | derivada del modelo, pendiente de verificación |
| `$60` | RTS | s | 1 | 6 |  | derivada del modelo, pendiente de verificación |
| `$61` | ADC | (d,X) | 2 | 6 | m,d | derivada del modelo, pendiente de verificación |
| `$62` | PER | rl | 3 | 6 |  | derivada del modelo, pendiente de verificación |
| `$63` | ADC | d,S | 2 | 4 | m | derivada del modelo, pendiente de verificación |
| `$64` | STZ | d | 2 | 3 | m,d | derivada del modelo, pendiente de verificación |
| `$65` | ADC | d | 2 | 3 | m,d | derivada del modelo, pendiente de verificación |
| `$66` | ROR | d | 2 | 5 | m2,d | derivada del modelo, pendiente de verificación |
| `$67` | ADC | [d] | 2 | 6 | m,d | derivada del modelo, pendiente de verificación |
| `$68` | PLA | s | 1 | 4 | m | derivada del modelo, pendiente de verificación |
| `$69` | ADC | # | 2 | 2 | m | derivada del modelo, pendiente de verificación |
| `$6A` | ROR | A | 1 | 2 |  | derivada del modelo, pendiente de verificación |
| `$6B` | RTL | s | 1 | 6 |  | derivada del modelo, pendiente de verificación |
| `$6C` | JMP | (a) | 3 | 5 |  | derivada del modelo, pendiente de verificación |
| `$6D` | ADC | a | 3 | 4 | m | derivada del modelo, pendiente de verificación |
| `$6E` | ROR | a | 3 | 6 | m2 | derivada del modelo, pendiente de verificación |
| `$6F` | ADC | al | 4 | 5 | m | derivada del modelo, pendiente de verificación |
| `$70` | BVS | r | 2 | 2 | b | derivada del modelo, pendiente de verificación |
| `$71` | ADC | (d),Y | 2 | 5 | m,p*,d | derivada del modelo, pendiente de verificación |
| `$72` | ADC | (d) | 2 | 5 | m,d | derivada del modelo, pendiente de verificación |
| `$73` | ADC | (d,S),Y | 2 | 7 | m | derivada del modelo, pendiente de verificación |
| `$74` | STZ | d,X | 2 | 4 | m,d | derivada del modelo, pendiente de verificación |
| `$75` | ADC | d,X | 2 | 4 | m,d | derivada del modelo, pendiente de verificación |
| `$76` | ROR | d,X | 2 | 6 | m2,d | derivada del modelo, pendiente de verificación |
| `$77` | ADC | [d],Y | 2 | 6 | m,d | derivada del modelo, pendiente de verificación |
| `$78` | SEI | i | 1 | 2 |  | derivada del modelo, pendiente de verificación |
| `$79` | ADC | a,Y | 3 | 4 | m,p* | derivada del modelo, pendiente de verificación |
| `$7A` | PLY | s | 1 | 4 | x | derivada del modelo, pendiente de verificación |
| `$7B` | TDC | i | 1 | 2 |  | derivada del modelo, pendiente de verificación |
| `$7C` | JMP | (a,X) | 3 | 6 |  | derivada del modelo, pendiente de verificación |
| `$7D` | ADC | a,X | 3 | 4 | m,p* | derivada del modelo, pendiente de verificación |
| `$7E` | ROR | a,X | 3 | 7 | m2 | derivada del modelo, pendiente de verificación |
| `$7F` | ADC | al,X | 4 | 5 | m | derivada del modelo, pendiente de verificación |
| `$80` | BRA | r | 2 | 3 | +1 si E=1 y cruza pág | derivada del modelo, pendiente de verificación |
| `$81` | STA | (d,X) | 2 | 6 | m,d | derivada del modelo, pendiente de verificación |
| `$82` | BRL | rl | 3 | 4 |  | derivada del modelo, pendiente de verificación |
| `$83` | STA | d,S | 2 | 4 | m | derivada del modelo, pendiente de verificación |
| `$84` | STY | d | 2 | 3 | x,d | derivada del modelo, pendiente de verificación |
| `$85` | STA | d | 2 | 3 | m,d | derivada del modelo, pendiente de verificación |
| `$86` | STX | d | 2 | 3 | x,d | derivada del modelo, pendiente de verificación |
| `$87` | STA | [d] | 2 | 6 | m,d | derivada del modelo, pendiente de verificación |
| `$88` | DEY | i | 1 | 2 |  | derivada del modelo, pendiente de verificación |
| `$89` | BIT | # | 2 | 2 | m | derivada del modelo, pendiente de verificación |
| `$8A` | TXA | i | 1 | 2 |  | derivada del modelo, pendiente de verificación |
| `$8B` | PHB | s | 1 | 3 |  | derivada del modelo, pendiente de verificación |
| `$8C` | STY | a | 3 | 4 | x | derivada del modelo, pendiente de verificación |
| `$8D` | STA | a | 3 | 4 | m | derivada del modelo, pendiente de verificación |
| `$8E` | STX | a | 3 | 4 | x | derivada del modelo, pendiente de verificación |
| `$8F` | STA | al | 4 | 5 | m | derivada del modelo, pendiente de verificación |
| `$90` | BCC | r | 2 | 2 | b | derivada del modelo, pendiente de verificación |
| `$91` | STA | (d),Y | 2 | 6 | m,d | derivada del modelo, pendiente de verificación |
| `$92` | STA | (d) | 2 | 5 | m,d | derivada del modelo, pendiente de verificación |
| `$93` | STA | (d,S),Y | 2 | 7 | m | derivada del modelo, pendiente de verificación |
| `$94` | STY | d,X | 2 | 4 | x,d | derivada del modelo, pendiente de verificación |
| `$95` | STA | d,X | 2 | 4 | m,d | derivada del modelo, pendiente de verificación |
| `$96` | STX | d,Y | 2 | 4 | x,d | derivada del modelo, pendiente de verificación |
| `$97` | STA | [d],Y | 2 | 6 | m,d | derivada del modelo, pendiente de verificación |
| `$98` | TYA | i | 1 | 2 |  | derivada del modelo, pendiente de verificación |
| `$99` | STA | a,Y | 3 | 5 | m | derivada del modelo, pendiente de verificación |
| `$9A` | TXS | i | 1 | 2 |  | derivada del modelo, pendiente de verificación |
| `$9B` | TXY | i | 1 | 2 |  | derivada del modelo, pendiente de verificación |
| `$9C` | STZ | a | 3 | 4 | m | derivada del modelo, pendiente de verificación |
| `$9D` | STA | a,X | 3 | 5 | m | derivada del modelo, pendiente de verificación |
| `$9E` | STZ | a,X | 3 | 5 | m | derivada del modelo, pendiente de verificación |
| `$9F` | STA | al,X | 4 | 5 | m | derivada del modelo, pendiente de verificación |
| `$A0` | LDY | # | 2 | 2 | x | derivada del modelo, pendiente de verificación |
| `$A1` | LDA | (d,X) | 2 | 6 | m,d | derivada del modelo, pendiente de verificación |
| `$A2` | LDX | # | 2 | 2 | x | derivada del modelo, pendiente de verificación |
| `$A3` | LDA | d,S | 2 | 4 | m | derivada del modelo, pendiente de verificación |
| `$A4` | LDY | d | 2 | 3 | x,d | derivada del modelo, pendiente de verificación |
| `$A5` | LDA | d | 2 | 3 | m,d | derivada del modelo, pendiente de verificación |
| `$A6` | LDX | d | 2 | 3 | x,d | derivada del modelo, pendiente de verificación |
| `$A7` | LDA | [d] | 2 | 6 | m,d | derivada del modelo, pendiente de verificación |
| `$A8` | TAY | i | 1 | 2 |  | derivada del modelo, pendiente de verificación |
| `$A9` | LDA | # | 2 | 2 | m | derivada del modelo, pendiente de verificación |
| `$AA` | TAX | i | 1 | 2 |  | derivada del modelo, pendiente de verificación |
| `$AB` | PLB | s | 1 | 4 |  | derivada del modelo, pendiente de verificación |
| `$AC` | LDY | a | 3 | 4 | x | derivada del modelo, pendiente de verificación |
| `$AD` | LDA | a | 3 | 4 | m | derivada del modelo, pendiente de verificación |
| `$AE` | LDX | a | 3 | 4 | x | derivada del modelo, pendiente de verificación |
| `$AF` | LDA | al | 4 | 5 | m | derivada del modelo, pendiente de verificación |
| `$B0` | BCS | r | 2 | 2 | b | derivada del modelo, pendiente de verificación |
| `$B1` | LDA | (d),Y | 2 | 5 | m,p*,d | derivada del modelo, pendiente de verificación |
| `$B2` | LDA | (d) | 2 | 5 | m,d | derivada del modelo, pendiente de verificación |
| `$B3` | LDA | (d,S),Y | 2 | 7 | m | derivada del modelo, pendiente de verificación |
| `$B4` | LDY | d,X | 2 | 4 | x,d | derivada del modelo, pendiente de verificación |
| `$B5` | LDA | d,X | 2 | 4 | m,d | derivada del modelo, pendiente de verificación |
| `$B6` | LDX | d,Y | 2 | 4 | x,d | derivada del modelo, pendiente de verificación |
| `$B7` | LDA | [d],Y | 2 | 6 | m,d | derivada del modelo, pendiente de verificación |
| `$B8` | CLV | i | 1 | 2 |  | derivada del modelo, pendiente de verificación |
| `$B9` | LDA | a,Y | 3 | 4 | m,p* | derivada del modelo, pendiente de verificación |
| `$BA` | TSX | i | 1 | 2 |  | derivada del modelo, pendiente de verificación |
| `$BB` | TYX | i | 1 | 2 |  | derivada del modelo, pendiente de verificación |
| `$BC` | LDY | a,X | 3 | 4 | p* | derivada del modelo, pendiente de verificación |
| `$BD` | LDA | a,X | 3 | 4 | m,p* | derivada del modelo, pendiente de verificación |
| `$BE` | LDX | a,Y | 3 | 4 | p* | derivada del modelo, pendiente de verificación |
| `$BF` | LDA | al,X | 4 | 5 | m | derivada del modelo, pendiente de verificación |
| `$C0` | CPY | # | 2 | 2 | x | derivada del modelo, pendiente de verificación |
| `$C1` | CMP | (d,X) | 2 | 6 | m,d | derivada del modelo, pendiente de verificación |
| `$C2` | REP | # | 2 | 3 |  | derivada del modelo, pendiente de verificación |
| `$C3` | CMP | d,S | 2 | 4 | m | derivada del modelo, pendiente de verificación |
| `$C4` | CPY | d | 2 | 3 | x,d | derivada del modelo, pendiente de verificación |
| `$C5` | CMP | d | 2 | 3 | m,d | derivada del modelo, pendiente de verificación |
| `$C6` | DEC | d | 2 | 5 | m2,d | derivada del modelo, pendiente de verificación |
| `$C7` | CMP | [d] | 2 | 6 | m,d | derivada del modelo, pendiente de verificación |
| `$C8` | INY | i | 1 | 2 |  | derivada del modelo, pendiente de verificación |
| `$C9` | CMP | # | 2 | 2 | m | derivada del modelo, pendiente de verificación |
| `$CA` | DEX | i | 1 | 2 |  | derivada del modelo, pendiente de verificación |
| `$CB` | WAI | i | 1 | 3 |  | derivada del modelo, pendiente de verificación |
| `$CC` | CPY | a | 3 | 4 | x | derivada del modelo, pendiente de verificación |
| `$CD` | CMP | a | 3 | 4 | m | derivada del modelo, pendiente de verificación |
| `$CE` | DEC | a | 3 | 6 | m2 | derivada del modelo, pendiente de verificación |
| `$CF` | CMP | al | 4 | 5 | m | derivada del modelo, pendiente de verificación |
| `$D0` | BNE | r | 2 | 2 | b | derivada del modelo, pendiente de verificación |
| `$D1` | CMP | (d),Y | 2 | 5 | m,p*,d | derivada del modelo, pendiente de verificación |
| `$D2` | CMP | (d) | 2 | 5 | m,d | derivada del modelo, pendiente de verificación |
| `$D3` | CMP | (d,S),Y | 2 | 7 | m | derivada del modelo, pendiente de verificación |
| `$D4` | PEI | (d) | 2 | 6 | d | derivada del modelo, pendiente de verificación |
| `$D5` | CMP | d,X | 2 | 4 | m,d | derivada del modelo, pendiente de verificación |
| `$D6` | DEC | d,X | 2 | 6 | m2,d | derivada del modelo, pendiente de verificación |
| `$D7` | CMP | [d],Y | 2 | 6 | m,d | derivada del modelo, pendiente de verificación |
| `$D8` | CLD | i | 1 | 2 |  | derivada del modelo, pendiente de verificación |
| `$D9` | CMP | a,Y | 3 | 4 | m,p* | derivada del modelo, pendiente de verificación |
| `$DA` | PHX | s | 1 | 3 | x | derivada del modelo, pendiente de verificación |
| `$DB` | STP | i | 1 | 3 |  | derivada del modelo, pendiente de verificación |
| `$DC` | JMP | (al) | 3 | 6 |  | derivada del modelo, pendiente de verificación |
| `$DD` | CMP | a,X | 3 | 4 | m,p* | derivada del modelo, pendiente de verificación |
| `$DE` | DEC | a,X | 3 | 7 | m2 | derivada del modelo, pendiente de verificación |
| `$DF` | CMP | al,X | 4 | 5 | m | derivada del modelo, pendiente de verificación |
| `$E0` | CPX | # | 2 | 2 | x | derivada del modelo, pendiente de verificación |
| `$E1` | SBC | (d,X) | 2 | 6 | m,d | derivada del modelo, pendiente de verificación |
| `$E2` | SEP | # | 2 | 3 |  | derivada del modelo, pendiente de verificación |
| `$E3` | SBC | d,S | 2 | 4 | m | derivada del modelo, pendiente de verificación |
| `$E4` | CPX | d | 2 | 3 | x,d | derivada del modelo, pendiente de verificación |
| `$E5` | SBC | d | 2 | 3 | m,d | derivada del modelo, pendiente de verificación |
| `$E6` | INC | d | 2 | 5 | m2,d | derivada del modelo, pendiente de verificación |
| `$E7` | SBC | [d] | 2 | 6 | m,d | derivada del modelo, pendiente de verificación |
| `$E8` | INX | i | 1 | 2 |  | derivada del modelo, pendiente de verificación |
| `$E9` | SBC | # | 2 | 2 | m | derivada del modelo, pendiente de verificación |
| `$EA` | NOP | i | 1 | 2 |  | derivada del modelo, pendiente de verificación |
| `$EB` | XBA | i | 1 | 3 |  | derivada del modelo, pendiente de verificación |
| `$EC` | CPX | a | 3 | 4 | x | derivada del modelo, pendiente de verificación |
| `$ED` | SBC | a | 3 | 4 | m | derivada del modelo, pendiente de verificación |
| `$EE` | INC | a | 3 | 6 | m2 | derivada del modelo, pendiente de verificación |
| `$EF` | SBC | al | 4 | 5 | m | derivada del modelo, pendiente de verificación |
| `$F0` | BEQ | r | 2 | 2 | b | derivada del modelo, pendiente de verificación |
| `$F1` | SBC | (d),Y | 2 | 5 | m,p*,d | derivada del modelo, pendiente de verificación |
| `$F2` | SBC | (d) | 2 | 5 | m,d | derivada del modelo, pendiente de verificación |
| `$F3` | SBC | (d,S),Y | 2 | 7 | m | derivada del modelo, pendiente de verificación |
| `$F4` | PEA | a | 3 | 5 |  | derivada del modelo, pendiente de verificación |
| `$F5` | SBC | d,X | 2 | 4 | m,d | derivada del modelo, pendiente de verificación |
| `$F6` | INC | d,X | 2 | 6 | m2,d | derivada del modelo, pendiente de verificación |
| `$F7` | SBC | [d],Y | 2 | 6 | m,d | derivada del modelo, pendiente de verificación |
| `$F8` | SED | i | 1 | 2 |  | derivada del modelo, pendiente de verificación |
| `$F9` | SBC | a,Y | 3 | 4 | m,p* | derivada del modelo, pendiente de verificación |
| `$FA` | PLX | s | 1 | 4 | x | derivada del modelo, pendiente de verificación |
| `$FB` | XCE | i | 1 | 2 |  | derivada del modelo, pendiente de verificación |
| `$FC` | JSR | (a,X) | 3 | 8 |  | derivada del modelo, pendiente de verificación |
| `$FD` | SBC | a,X | 3 | 4 | m,p* | derivada del modelo, pendiente de verificación |
| `$FE` | INC | a,X | 3 | 7 | m2 | derivada del modelo, pendiente de verificación |
| `$FF` | SBC | al,X | 4 | 5 | m | derivada del modelo, pendiente de verificación |

## Resumen
- **Filas verificadas:** 0
- **Filas pendientes de verificación:** 256
