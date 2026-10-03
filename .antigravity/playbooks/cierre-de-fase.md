<!-- SPDX-FileCopyrightText: 2026 henoSNES contributors -->
<!-- SPDX-License-Identifier: GPL-3.0-or-later -->

# Playbook: Cierre de Fase (Gate Checklist)

Este playbook describe la lista de verificación obligatoria para validar y formalizar el cierre de una fase del roadmap (`docs/SPEC.md` §7) antes de iniciar la siguiente.

---

## Criterios de Aceptación por Fase (Gate)

Ninguna fase puede declararse cerrada ni abrirse la siguiente sin que todos los puntos del siguiente checklist estén en verde y verificados por el equipo.

---

## Checklist de Cierre

### 1. Build y CI Multiplataforma
- [ ] Pipeline de CI en verde en todas las plataformas principales soportadas:
  - Windows (`windows-latest` con MSVC)
  - Linux (`ubuntu-24.04` con Clang 17)
  - macOS Apple Silicon (`macos-14` con Apple Clang)
- [ ] Artefactos de la fase compilados y descargables desde el workflow de CI.
- [ ] Sin advertencias críticas del compilador ni errores de linking.

### 2. Pruebas y Regresión
- [ ] 100% de pruebas unitarias y de integración pasando (`ctest`).
- [ ] Suite de golden frames (`tests/golden`) ejecutada sin ninguna regresión visual respecto a los hashes de referencia.
- [ ] Test ROMs comunitarios libres aplicables a la fase ejecutados con resultado 100% verde (verificado vía `heno-cli` headless).

### 3. Código y Calidad
- [ ] Formateo verificado con `.clang-format` en todos los archivos nuevos/modificados.
- [ ] Análisis estático con `.clang-tidy` limpio en las áreas afectadas.
- [ ] Cabecera SPDX de licencia presente en cada archivo del repositorio (conformidad REUSE 3.0).

### 4. Gobernanza y Legal
- [ ] `THIRD_PARTY_NOTICES.md` actualizado con todas las dependencias o fragmentos de código portados durante la fase (URL de origen, commit exacto y licencia).
- [ ] Confirmación de que no se ha introducido ninguna ROM comercial, BIOS propietaria, microcódigo ni dependencias incompatibles con GPLv3.
- [ ] Todos los commits de la fase cuentan con certificación DCO (`Signed-off-by`).

### 5. Documentación y Backlog
- [ ] Todas las tareas de la fase en `docs/BACKLOG.md` marcadas como `COMPLETADO` o `RESUELTO`.
- [ ] Cualquier deuda técnica pendiente registrada explícitamente en la sección de Deuda Técnica de `docs/BACKLOG.md` con su identificador `DT-XXX` y plan de mitigación.
- [ ] Todos los ADRs generados durante la fase debidamente enlazados, numerados y con estado `Aceptado`.
- [ ] `docs/SPEC.md` actualizada si alguna decisión refinó especificaciones existentes.

---

## Formalización del Cierre

1. Crear un PR de cierre de fase etiquetado `chore(release): close phase X gate`.
2. Adjuntar el reporte de evidencia (resumen de hashes de golden frames, enlaces a runs de CI verdes, lista de ADRs).
3. Obtener aprobación explícita de un mantenedor humano antes de marcar la fase como cerrada e iniciar las tareas de la fase subsecuente.
