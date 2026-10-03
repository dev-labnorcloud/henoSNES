<!-- SPDX-FileCopyrightText: 2026 henoSNES contributors -->
<!-- SPDX-License-Identifier: CC-BY-4.0 -->

# Referencia 65C816: Tabla de Opcodes

Esta tabla documenta los 256 opcodes del procesador 65C816 y sirve como única fuente de verdad para la emulación ciclo a ciclo.

## Modificadores de Ciclo (Condiciones)
- **m**: +1 ciclo si el flag M está en 0 (acumulador/memoria de 16 bits)
- **x**: +1 ciclo si el flag X está en 0 (índice de 16 bits)
- **e**: +1 ciclo en modo emulación (flag E=1)
- **p**: +1 ciclo si hay cruce de página (aplica en índices de 16 bits)
- **d**: +1 ciclo si la Página Directa no está alineada a página (DL != 0)
- **b**: +1 ciclo si el salto (branch) es tomado; +1 extra si hay cruce de página (solo en modo emulación)
- **s**: ciclo atado a velocidad de la memoria de acuerdo a su dominio

| Opcode | Mnemónico | Modo de Direccionamiento | Bytes | Ciclos Base | Condiciones Extra | Fuente |
| --- | --- | --- | --- | --- | --- | --- |
| `$00` | BRK | s | 2 | 7 | e | Programming the 65816 |
| `$01` | ORA | (d,X) | 2 | 6 | m,d | Programming the 65816 |
| `$02` | COP | s | 2 | 7 | e | Programming the 65816 |
| `$03` | ORA | d,S | 2 | 4 | m | Programming the 65816 |
| `$04` | TSB | d | 2 | 5 | m,d | Programming the 65816 |
| `$05` | ORA | d | 2 | 3 | m,d | Programming the 65816 |
| `$06` | ASL | d | 2 | 5 | m,d | Programming the 65816 |
| `$07` | ORA | [d] | 2 | 6 | m,d | Programming the 65816 |
| `$08` | PHP | s | 1 | 3 |  | Programming the 65816 |
| `$09` | ORA | # | 2 | 2 | m | Programming the 65816 |
| `$0A` | ASL | A | 1 | 2 |  | Programming the 65816 |
| `$0B` | PHD | s | 1 | 4 |  | Programming the 65816 |
| `$0C` | TSB | a | 3 | 6 | m | Programming the 65816 |
| `$0D` | ORA | a | 3 | 4 | m | Programming the 65816 |
| `$0E` | ASL | a | 3 | 6 | m | Programming the 65816 |
| `$0F` | ORA | al | 4 | 5 | m | Programming the 65816 |
| `$10` | BPL | r | 2 | 2 | b | Programming the 65816 |
| `$11` | ORA | (d),Y | 2 | 5 | m,x,p,d | Programming the 65816 |
| `$12` | ORA | (d) | 2 | 5 | m,d | Programming the 65816 |
| `$13` | ORA | (d,S),Y | 2 | 7 | m | Programming the 65816 |
| `$14` | TRB | d | 2 | 5 | m,d | Programming the 65816 |
| `$15` | ORA | d,X | 2 | 4 | m,d | Programming the 65816 |
| `$16` | ASL | d,X | 2 | 6 | m,d | Programming the 65816 |
| `$17` | ORA | [d],Y | 2 | 6 | m,x,p,d | Programming the 65816 |
| `$18` | CLC | i | 1 | 2 |  | Programming the 65816 |
| `$19` | ORA | a,Y | 3 | 4 | m,x,p | Programming the 65816 |
| `$1A` | INC | A | 1 | 2 |  | Programming the 65816 |
| `$1B` | TCS | i | 1 | 2 |  | Programming the 65816 |
| `$1C` | TRB | a | 3 | 6 | m | Programming the 65816 |
| `$1D` | ORA | a,X | 3 | 4 | m,x,p | Programming the 65816 |
| `$1E` | ASL | a,X | 3 | 7 | m | Programming the 65816 |
| `$1F` | ORA | al,X | 4 | 5 | m | Programming the 65816 |
| `$20` | JSR | a | 3 | 6 |  | Programming the 65816 |
| `$21` | AND | (d,X) | 2 | 6 | m,d | Programming the 65816 |
| `$22` | JSL | al | 4 | 8 |  | Programming the 65816 |
| `$23` | AND | d,S | 2 | 4 | m | Programming the 65816 |
| `$24` | BIT | d | 2 | 3 | m,d | Programming the 65816 |
| `$25` | AND | d | 2 | 3 | m,d | Programming the 65816 |
| `$26` | ROL | d | 2 | 5 | m,d | Programming the 65816 |
| `$27` | AND | [d] | 2 | 6 | m,d | Programming the 65816 |
| `$28` | PLP | s | 1 | 4 |  | Programming the 65816 |
| `$29` | AND | # | 2 | 2 | m | Programming the 65816 |
| `$2A` | ROL | A | 1 | 2 |  | Programming the 65816 |
| `$2B` | PLD | s | 1 | 5 |  | Programming the 65816 |
| `$2C` | BIT | a | 3 | 4 | m | Programming the 65816 |
| `$2D` | AND | a | 3 | 4 | m | Programming the 65816 |
| `$2E` | ROL | a | 3 | 6 | m | Programming the 65816 |
| `$2F` | AND | al | 4 | 5 | m | Programming the 65816 |
| `$30` | BMI | r | 2 | 2 | b | Programming the 65816 |
| `$31` | AND | (d),Y | 2 | 5 | m,x,p,d | Programming the 65816 |
| `$32` | AND | (d) | 2 | 5 | m,d | Programming the 65816 |
| `$33` | AND | (d,S),Y | 2 | 7 | m | Programming the 65816 |
| `$34` | BIT | d,X | 2 | 4 | m,d | Programming the 65816 |
| `$35` | AND | d,X | 2 | 4 | m,d | Programming the 65816 |
| `$36` | ROL | d,X | 2 | 6 | m,d | Programming the 65816 |
| `$37` | AND | [d],Y | 2 | 6 | m,x,p,d | Programming the 65816 |
| `$38` | SEC | i | 1 | 2 |  | Programming the 65816 |
| `$39` | AND | a,Y | 3 | 4 | m,x,p | Programming the 65816 |
| `$3A` | DEC | A | 1 | 2 |  | Programming the 65816 |
| `$3B` | TSC | i | 1 | 2 |  | Programming the 65816 |
| `$3C` | BIT | a,X | 3 | 4 | m,x,p | Programming the 65816 |
| `$3D` | AND | a,X | 3 | 4 | m,x,p | Programming the 65816 |
| `$3E` | ROL | a,X | 3 | 7 | m | Programming the 65816 |
| `$3F` | AND | al,X | 4 | 5 | m | Programming the 65816 |
| `$40` | RTI | s | 1 | 6 | e | Programming the 65816 |
| `$41` | EOR | (d,X) | 2 | 6 | m,d | Programming the 65816 |
| `$42` | WDM | i | 2 | 2 |  | Programming the 65816 |
| `$43` | EOR | d,S | 2 | 4 | m | Programming the 65816 |
| `$44` | MVP | xyc | 3 | 7 |  | Programming the 65816 |
| `$45` | EOR | d | 2 | 3 | m,d | Programming the 65816 |
| `$46` | LSR | d | 2 | 5 | m,d | Programming the 65816 |
| `$47` | EOR | [d] | 2 | 6 | m,d | Programming the 65816 |
| `$48` | PHA | s | 1 | 3 | m | Programming the 65816 |
| `$49` | EOR | # | 2 | 2 | m | Programming the 65816 |
| `$4A` | LSR | A | 1 | 2 |  | Programming the 65816 |
| `$4B` | PHK | s | 1 | 3 |  | Programming the 65816 |
| `$4C` | JMP | a | 3 | 3 |  | Programming the 65816 |
| `$4D` | EOR | a | 3 | 4 | m | Programming the 65816 |
| `$4E` | LSR | a | 3 | 6 | m | Programming the 65816 |
| `$4F` | EOR | al | 4 | 5 | m | Programming the 65816 |
| `$50` | BVC | r | 2 | 2 | b | Programming the 65816 |
| `$51` | EOR | (d),Y | 2 | 5 | m,x,p,d | Programming the 65816 |
| `$52` | EOR | (d) | 2 | 5 | m,d | Programming the 65816 |
| `$53` | EOR | (d,S),Y | 2 | 7 | m | Programming the 65816 |
| `$54` | MVN | xyc | 3 | 7 |  | Programming the 65816 |
| `$55` | EOR | d,X | 2 | 4 | m,d | Programming the 65816 |
| `$56` | LSR | d,X | 2 | 6 | m,d | Programming the 65816 |
| `$57` | EOR | [d],Y | 2 | 6 | m,x,p,d | Programming the 65816 |
| `$58` | CLI | i | 1 | 2 |  | Programming the 65816 |
| `$59` | EOR | a,Y | 3 | 4 | m,x,p | Programming the 65816 |
| `$5A` | PHY | s | 1 | 3 | x | Programming the 65816 |
| `$5B` | TCD | i | 1 | 2 |  | Programming the 65816 |
| `$5C` | JMP | al | 4 | 4 |  | Programming the 65816 |
| `$5D` | EOR | a,X | 3 | 4 | m,x,p | Programming the 65816 |
| `$5E` | LSR | a,X | 3 | 7 | m | Programming the 65816 |
| `$5F` | EOR | al,X | 4 | 5 | m | Programming the 65816 |
| `$60` | RTS | s | 1 | 6 |  | Programming the 65816 |
| `$61` | ADC | (d,X) | 2 | 6 | m,d | Programming the 65816 |
| `$62` | PER | rl | 3 | 6 |  | Programming the 65816 |
| `$63` | ADC | d,S | 2 | 4 | m | Programming the 65816 |
| `$64` | STZ | d | 2 | 3 | m,d | Programming the 65816 |
| `$65` | ADC | d | 2 | 3 | m,d | Programming the 65816 |
| `$66` | ROR | d | 2 | 5 | m,d | Programming the 65816 |
| `$67` | ADC | [d] | 2 | 6 | m,d | Programming the 65816 |
| `$68` | PLA | s | 1 | 4 | m | Programming the 65816 |
| `$69` | ADC | # | 2 | 2 | m | Programming the 65816 |
| `$6A` | ROR | A | 1 | 2 |  | Programming the 65816 |
| `$6B` | RTL | s | 1 | 6 |  | Programming the 65816 |
| `$6C` | JMP | (a) | 3 | 5 |  | Programming the 65816 |
| `$6D` | ADC | a | 3 | 4 | m | Programming the 65816 |
| `$6E` | ROR | a | 3 | 6 | m | Programming the 65816 |
| `$6F` | ADC | al | 4 | 5 | m | Programming the 65816 |
| `$70` | BVS | r | 2 | 2 | b | Programming the 65816 |
| `$71` | ADC | (d),Y | 2 | 5 | m,x,p,d | Programming the 65816 |
| `$72` | ADC | (d) | 2 | 5 | m,d | Programming the 65816 |
| `$73` | ADC | (d,S),Y | 2 | 7 | m | Programming the 65816 |
| `$74` | STZ | d,X | 2 | 4 | m,d | Programming the 65816 |
| `$75` | ADC | d,X | 2 | 4 | m,d | Programming the 65816 |
| `$76` | ROR | d,X | 2 | 6 | m,d | Programming the 65816 |
| `$77` | ADC | [d],Y | 2 | 6 | m,x,p,d | Programming the 65816 |
| `$78` | SEI | i | 1 | 2 |  | Programming the 65816 |
| `$79` | ADC | a,Y | 3 | 4 | m,x,p | Programming the 65816 |
| `$7A` | PLY | s | 1 | 4 | x | Programming the 65816 |
| `$7B` | TDC | i | 1 | 2 |  | Programming the 65816 |
| `$7C` | JMP | (a,X) | 3 | 6 |  | Programming the 65816 |
| `$7D` | ADC | a,X | 3 | 4 | m,x,p | Programming the 65816 |
| `$7E` | ROR | a,X | 3 | 7 | m | Programming the 65816 |
| `$7F` | ADC | al,X | 4 | 5 | m | Programming the 65816 |
| `$80` | BRA | r | 2 | 3 | b | Programming the 65816 |
| `$81` | STA | (d,X) | 2 | 6 | m,d | Programming the 65816 |
| `$82` | BRL | rl | 3 | 4 |  | Programming the 65816 |
| `$83` | STA | d,S | 2 | 4 | m | Programming the 65816 |
| `$84` | STY | d | 2 | 3 | x,d | Programming the 65816 |
| `$85` | STA | d | 2 | 3 | m,d | Programming the 65816 |
| `$86` | STX | d | 2 | 3 | x,d | Programming the 65816 |
| `$87` | STA | [d] | 2 | 6 | m,d | Programming the 65816 |
| `$88` | DEY | i | 1 | 2 |  | Programming the 65816 |
| `$89` | BIT | # | 2 | 2 | m | Programming the 65816 |
| `$8A` | TXA | i | 1 | 2 |  | Programming the 65816 |
| `$8B` | PHB | s | 1 | 3 |  | Programming the 65816 |
| `$8C` | STY | a | 3 | 4 | x | Programming the 65816 |
| `$8D` | STA | a | 3 | 4 | m | Programming the 65816 |
| `$8E` | STX | a | 3 | 4 | x | Programming the 65816 |
| `$8F` | STA | al | 4 | 5 | m | Programming the 65816 |
| `$90` | BCC | r | 2 | 2 | b | Programming the 65816 |
| `$91` | STA | (d),Y | 2 | 6 | m,d | Programming the 65816 |
| `$92` | STA | (d) | 2 | 5 | m,d | Programming the 65816 |
| `$93` | STA | (d,S),Y | 2 | 7 | m | Programming the 65816 |
| `$94` | STY | d,X | 2 | 4 | x,d | Programming the 65816 |
| `$95` | STA | d,X | 2 | 4 | m,d | Programming the 65816 |
| `$96` | STX | d,Y | 2 | 4 | x,d | Programming the 65816 |
| `$97` | STA | [d],Y | 2 | 6 | m,d | Programming the 65816 |
| `$98` | TYA | i | 1 | 2 |  | Programming the 65816 |
| `$99` | STA | a,Y | 3 | 5 | m | Programming the 65816 |
| `$9A` | TXS | i | 1 | 2 |  | Programming the 65816 |
| `$9B` | TXY | i | 1 | 2 |  | Programming the 65816 |
| `$9C` | STZ | a | 3 | 4 | m | Programming the 65816 |
| `$9D` | STA | a,X | 3 | 5 | m | Programming the 65816 |
| `$9E` | STZ | a,X | 3 | 5 | m | Programming the 65816 |
| `$9F` | STA | al,X | 4 | 5 | m | Programming the 65816 |
| `$A0` | LDY | # | 2 | 2 | x | Programming the 65816 |
| `$A1` | LDA | (d,X) | 2 | 6 | m,d | Programming the 65816 |
| `$A2` | LDX | # | 2 | 2 | x | Programming the 65816 |
| `$A3` | LDA | d,S | 2 | 4 | m | Programming the 65816 |
| `$A4` | LDY | d | 2 | 3 | x,d | Programming the 65816 |
| `$A5` | LDA | d | 2 | 3 | m,d | Programming the 65816 |
| `$A6` | LDX | d | 2 | 3 | x,d | Programming the 65816 |
| `$A7` | LDA | [d] | 2 | 6 | m,d | Programming the 65816 |
| `$A8` | TAY | i | 1 | 2 |  | Programming the 65816 |
| `$A9` | LDA | # | 2 | 2 | m | Programming the 65816 |
| `$AA` | TAX | i | 1 | 2 |  | Programming the 65816 |
| `$AB` | PLB | s | 1 | 4 |  | Programming the 65816 |
| `$AC` | LDY | a | 3 | 4 | x | Programming the 65816 |
| `$AD` | LDA | a | 3 | 4 | m | Programming the 65816 |
| `$AE` | LDX | a | 3 | 4 | x | Programming the 65816 |
| `$AF` | LDA | al | 4 | 5 | m | Programming the 65816 |
| `$B0` | BCS | r | 2 | 2 | b | Programming the 65816 |
| `$B1` | LDA | (d),Y | 2 | 5 | m,x,p,d | Programming the 65816 |
| `$B2` | LDA | (d) | 2 | 5 | m,d | Programming the 65816 |
| `$B3` | LDA | (d,S),Y | 2 | 7 | m | Programming the 65816 |
| `$B4` | LDY | d,X | 2 | 4 | x,d | Programming the 65816 |
| `$B5` | LDA | d,X | 2 | 4 | m,d | Programming the 65816 |
| `$B6` | LDX | d,Y | 2 | 4 | x,d | Programming the 65816 |
| `$B7` | LDA | [d],Y | 2 | 6 | m,x,p,d | Programming the 65816 |
| `$B8` | CLV | i | 1 | 2 |  | Programming the 65816 |
| `$B9` | LDA | a,Y | 3 | 4 | m,x,p | Programming the 65816 |
| `$BA` | TSX | i | 1 | 2 |  | Programming the 65816 |
| `$BB` | TYX | i | 1 | 2 |  | Programming the 65816 |
| `$BC` | LDY | a,X | 3 | 4 | x,p | Programming the 65816 |
| `$BD` | LDA | a,X | 3 | 4 | m,x,p | Programming the 65816 |
| `$BE` | LDX | a,Y | 3 | 4 | x,p | Programming the 65816 |
| `$BF` | LDA | al,X | 4 | 5 | m | Programming the 65816 |
| `$C0` | CPY | # | 2 | 2 | x | Programming the 65816 |
| `$C1` | CMP | (d,X) | 2 | 6 | m,d | Programming the 65816 |
| `$C2` | REP | # | 2 | 3 |  | Programming the 65816 |
| `$C3` | CMP | d,S | 2 | 4 | m | Programming the 65816 |
| `$C4` | CPY | d | 2 | 3 | x,d | Programming the 65816 |
| `$C5` | CMP | d | 2 | 3 | m,d | Programming the 65816 |
| `$C6` | DEC | d | 2 | 5 | m,d | Programming the 65816 |
| `$C7` | CMP | [d] | 2 | 6 | m,d | Programming the 65816 |
| `$C8` | INY | i | 1 | 2 |  | Programming the 65816 |
| `$C9` | CMP | # | 2 | 2 | m | Programming the 65816 |
| `$CA` | DEX | i | 1 | 2 |  | Programming the 65816 |
| `$CB` | WAI | i | 1 | 3 |  | [verificar] |
| `$CC` | CPY | a | 3 | 4 | x | Programming the 65816 |
| `$CD` | CMP | a | 3 | 4 | m | Programming the 65816 |
| `$CE` | DEC | a | 3 | 6 | m | Programming the 65816 |
| `$CF` | CMP | al | 4 | 5 | m | Programming the 65816 |
| `$D0` | BNE | r | 2 | 2 | b | Programming the 65816 |
| `$D1` | CMP | (d),Y | 2 | 5 | m,x,p,d | Programming the 65816 |
| `$D2` | CMP | (d) | 2 | 5 | m,d | Programming the 65816 |
| `$D3` | CMP | (d,S),Y | 2 | 7 | m | Programming the 65816 |
| `$D4` | PEI | (d) | 2 | 6 | d | Programming the 65816 |
| `$D5` | CMP | d,X | 2 | 4 | m,d | Programming the 65816 |
| `$D6` | DEC | d,X | 2 | 6 | m,d | Programming the 65816 |
| `$D7` | CMP | [d],Y | 2 | 6 | m,x,p,d | Programming the 65816 |
| `$D8` | CLD | i | 1 | 2 |  | Programming the 65816 |
| `$D9` | CMP | a,Y | 3 | 4 | m,x,p | Programming the 65816 |
| `$DA` | PHX | s | 1 | 3 | x | Programming the 65816 |
| `$DB` | STP | i | 1 | 3 |  | [verificar] |
| `$DC` | JMP | (al) | 3 | 6 |  | Programming the 65816 |
| `$DD` | CMP | a,X | 3 | 4 | m,x,p | Programming the 65816 |
| `$DE` | DEC | a,X | 3 | 7 | m | Programming the 65816 |
| `$DF` | CMP | al,X | 4 | 5 | m | Programming the 65816 |
| `$E0` | CPX | # | 2 | 2 | x | Programming the 65816 |
| `$E1` | SBC | (d,X) | 2 | 6 | m,d | Programming the 65816 |
| `$E2` | SEP | # | 2 | 3 |  | Programming the 65816 |
| `$E3` | SBC | d,S | 2 | 4 | m | Programming the 65816 |
| `$E4` | CPX | d | 2 | 3 | x,d | Programming the 65816 |
| `$E5` | SBC | d | 2 | 3 | m,d | Programming the 65816 |
| `$E6` | INC | d | 2 | 5 | m,d | Programming the 65816 |
| `$E7` | SBC | [d] | 2 | 6 | m,d | Programming the 65816 |
| `$E8` | INX | i | 1 | 2 |  | Programming the 65816 |
| `$E9` | SBC | # | 2 | 2 | m | Programming the 65816 |
| `$EA` | NOP | i | 1 | 2 |  | Programming the 65816 |
| `$EB` | XBA | i | 1 | 3 |  | Programming the 65816 |
| `$EC` | CPX | a | 3 | 4 | x | Programming the 65816 |
| `$ED` | SBC | a | 3 | 4 | m | Programming the 65816 |
| `$EE` | INC | a | 3 | 6 | m | Programming the 65816 |
| `$EF` | SBC | al | 4 | 5 | m | Programming the 65816 |
| `$F0` | BEQ | r | 2 | 2 | b | Programming the 65816 |
| `$F1` | SBC | (d),Y | 2 | 5 | m,x,p,d | Programming the 65816 |
| `$F2` | SBC | (d) | 2 | 5 | m,d | Programming the 65816 |
| `$F3` | SBC | (d,S),Y | 2 | 7 | m | Programming the 65816 |
| `$F4` | PEA | a | 3 | 5 |  | Programming the 65816 |
| `$F5` | SBC | d,X | 2 | 4 | m,d | Programming the 65816 |
| `$F6` | INC | d,X | 2 | 6 | m,d | Programming the 65816 |
| `$F7` | SBC | [d],Y | 2 | 6 | m,x,p,d | Programming the 65816 |
| `$F8` | SED | i | 1 | 2 |  | Programming the 65816 |
| `$F9` | SBC | a,Y | 3 | 4 | m,x,p | Programming the 65816 |
| `$FA` | PLX | s | 1 | 4 | x | Programming the 65816 |
| `$FB` | XCE | i | 1 | 2 |  | Programming the 65816 |
| `$FC` | JSR | (a,X) | 3 | 8 |  | Programming the 65816 |
| `$FD` | SBC | a,X | 3 | 4 | m,x,p | Programming the 65816 |
| `$FE` | INC | a,X | 3 | 7 | m | Programming the 65816 |
| `$FF` | SBC | al,X | 4 | 5 | m | Programming the 65816 |

## Resumen
- **Filas verificadas:** 254
- **Filas pendientes de verificación (`[verificar]`):** 2
