<!-- SPDX-FileCopyrightText: 2026 henoSNES contributors -->
<!-- SPDX-License-Identifier: GPL-3.0-or-later -->

# Playbook: Creación de un Nuevo ADR

Este playbook define los pasos y la plantilla para registrar decisiones de arquitectura (Architecture Decision Records) en formato MADR (Markdown Any Decision Record) dentro de `docs/adr/`.

---

## Cuándo crear un ADR

Se **debe** crear un ADR **antes** de implementar cualquier cambio que involucre:
- Decisiones arquitectónicas no explícitas en `docs/SPEC.md`.
- Cambios en el diseño de capas o fronteras de abstracción (e.g. `core/`, `heno-gfx`, `heno.h`).
- Adición de nuevas dependencias externas (requiere además aprobación humana).
- Decisiones de layout de structs, versionado o estabilidad de ABI.
- Cambios en la estrategia de timing, scheduling, serialización o sincronización de audio/video.

---

## Procedimiento

1. **Determinar el número secuencial:**
   - Listar los archivos en `docs/adr/` y tomar el siguiente número de cuatro dígitos (e.g. `0004`).
2. **Nombrar el archivo:**
   - Formato: `docs/adr/NNNN-titulo-en-kebab-case.md` (e.g. `docs/adr/0004-scheduler-cooperativo.md`).
3. **Redactar el documento usando la plantilla MADR:**
   - Incluir contexto claro, alternativas evaluadas y consecuencias (positivas y negativas).
4. **Enlazar el ADR:**
   - Registrar el nuevo ADR en `docs/adr/README.md` y referenciarlo desde `docs/BACKLOG.md` o el PR correspondiente.

---

## Plantilla MADR

```markdown
<!-- SPDX-FileCopyrightText: 2026 henoSNES contributors -->
<!-- SPDX-License-Identifier: GPL-3.0-or-later -->

# ADR-NNNN: [Título corto y descriptivo]

## Estado

[Propuesto | Aceptado | Rechazado | Superado por ADR-XXXX]

## Fecha

YYYY-MM-DD

## Contexto

[¿Cuál es el problema o necesidad técnica? ¿Por qué se requiere tomar una decisión ahora?
Mencionar restricciones legales, de rendimiento, portabilidad o arquitectura relevantes.]

## Decisión

[¿Qué opción se eligió y por qué? Describir los detalles técnicos de la solución adoptada.]

### Detalles técnicos

- [Punto clave 1]
- [Punto clave 2]

## Alternativas consideradas

1. **[Opción A]:** [Descripción breve y motivo por el cual se descartó].
2. **[Opción B]:** [Descripción breve y motivo por el cual se descartó].

## Consecuencias

- ✅ [Consecuencia positiva 1: beneficios esperados]
- ✅ [Consecuencia positiva 2]
- ⚠️ [Consecuencia negativa / compromiso 1: compensaciones aceptadas]
- ⚠️ [Consecuencia negativa / compromiso 2]

## Documentos relacionados

- `docs/SPEC.md` §X
- `docs/BACKLOG.md` (Tarea PX-YYY)
- [Archivos de código afectados, e.g. `core/api/heno.h`]
```
