<!-- SPDX-FileCopyrightText: 2026 henoSNES contributors -->
<!-- SPDX-License-Identifier: GPL-3.0-or-later -->

# Documentación de Referencia de Hardware (`docs/refs`)

Este directorio almacena las especificaciones técnicas, hojas de datos (datasheets) y notas de ingeniería inversa utilizadas como referencia técnica para la implementación del núcleo de emulación.

---

## 📚 Documentación Requerida para Fase 1 / P-01 (CPU y Bus de Memoria)

Para implementar `core/cpu` y `core/bus` en la tarea **P-01**, los siguientes documentos de referencia deben ubicarse en esta carpeta:

### 1. CPU WDC 65C816 / W65C816S
- **Conjunto de instrucciones y modos de direccionamiento:**
  - Hoja de datos oficial WDC 65C816 (instrucciones, modos de 8 y 16 bits, flags de status, comportamiento de emulación 6502).
  - *Programming the 65816 including the 6502, 65C02 and 65802* (David Eyes & Ron Lichty) o transcripción técnica libre.
- **Detalle de opcodes y ciclos por acceso:**
  - Matriz completa de los 256 opcodes ($00–$FF) con descomposición ciclo a ciclo (acceso a opcode, operando, memoria, stack).
  - Tratamiento de cruce de página, saltos condicionales y ciclos de parada (WAI, STP).
- **Vectores de interrupción y resets:**
  - Comportamiento de NMI, IRQ (hardware/software), COP, BRK, ABORT y RESET en modo nativo vs. modo emulación.

### 2. Mapa de Memoria SNES (Memory Bus & Address Decoding)
- **Mapeo de cartuchos (Mappers):**
  - Mapeo LoROM (Tipo $20 / Mode 20): bancos `$00-$7D` y `$80-$FF`, direccionamiento de ROM y SRAM.
  - Mapeo HiROM (Tipo $21 / Mode 21): distribución lineal de ROM y SRAM en bancos superiores.
  - Mapeo ExHiROM (Tipo $25 / Mode 25): bancos extendidos para ROMs > 4 MB.
- **Espacio de memoria del sistema:**
  - WRAM (128 KB): bancos `$7E-$7F` y espejos en bancos `$00-$3F`/`$80-$BF` (`$0000-$1FFF`).
  - Registros I/O de PPU y APU (`$2100-$213F`).
  - Registros de control de CPU y DMA/HDMA (`$4200-$437F`).
  - Comportamiento de Open Bus (retorno de último valor en el bus de datos en lecturas a direcciones no mapeadas).

### 3. Timing de Bus y Sincronización de Reloj
- **Frecuencias de reloj maestro (Master Clock):**
  - Reloj base NTSC: 21.47727 MHz (~1.89 master cycles por ciclo de CPU estándar).
  - Reloj base PAL: 21.28137 MHz.
- **Ciclos de acceso a memoria (Memory Access Speed):**
  - SlowROM / Accesos lentos: 6 master cycles (3.58 MHz / 2.68 MHz efectivo).
  - FastROM / Accesos rápidos: 6 o 8 master cycles (3.58 MHz).
  - Accesos a registros I/O y ciclos de refresco de DRAM (40 master cycles por scanline en H-Blank).

---

## ⚖️ Política de Licencias y Restricciones Legales

Cualquier documento que se suba a este directorio debe cumplir estrictamente con las políticas de `AGENTS.md` y `LEGAL.md`:

1. **Permitido:**
   - Hojas de datos públicas de WDC (Western Design Center).
   - Notas técnicas y documentos de ingeniería inversa de la comunidad homebrew (e.g. documentos de anomie, nocash, superfamicom.org, SNESdev wiki) con licencia de libre distribución, dominio público o Creative Commons.
   - Resúmenes y tablas generadas por contribuidores del proyecto.

2. **PROHIBIDO:**
   - **NUNCA** subir código filtrado (leaks) de fabricantes o SDKs propietarios bajo NDA.
   - **NUNCA** subir manuales oficiales con copyright reservado que prohíba explícitamente su redistribución.
   - **NUNCA** subir archivos binarios de BIOS o ROMs comerciales a esta carpeta.

---

## 📁 Estado del Directorio

*Listo para recibir archivos de referencia técnica antes de iniciar la sesión P-01.*
