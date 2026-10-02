<!-- SPDX-FileCopyrightText: 2026 henoSNES contributors -->
<!-- SPDX-License-Identifier: GPL-3.0-or-later -->

# Contributing to henoSNES

Thank you for your interest in contributing! This document explains the process.

## How to contribute

1. Fork the repository.
2. Create a branch named `feat/<backlog-id>` (see `docs/BACKLOG.md`).
3. Make your changes following the conventions in `AGENTS.md`.
4. Ensure all tests pass: `ctest --preset <your-preset> --output-on-failure`.
5. Submit a pull request referencing the backlog ID and relevant ADR.

## Commit conventions

- Use [Conventional Commits](https://www.conventionalcommits.org/) format.
- Each commit must include a `Signed-off-by:` line (see DCO below).
- Example: `git commit -s -m "feat(cpu): implement ADC opcode"`

## Developer Certificate of Origin (DCO)

By contributing to this project, you certify that your contribution complies
with the [Developer Certificate of Origin v1.1](https://developercertificate.org/):

```
Developer Certificate of Origin
Version 1.1

Copyright (C) 2004, 2006 The Linux Foundation and its contributors.

Everyone is permitted to copy and distribute verbatim copies of this
license document, but changing it is not allowed.

Developer's Certificate of Origin 1.1

By making a contribution to this project, I certify that:

(a) The contribution was created in whole or in part by me and I
    have the right to submit it under the open source license
    indicated in the file; or

(b) The contribution is based upon previous work that, to the best
    of my knowledge, is covered under an appropriate open source
    license and I have the right under that license to submit that
    work with modifications, whether created in whole or in part
    by me, under the same open source license (unless I am
    permitted to submit under a different license), as indicated
    in the file; or

(c) The contribution was provided directly to me by some other
    person who certified (a), (b) or (c) and I have not modified
    it.

(d) I understand and agree that this project and the contribution
    are public and that a record of the contribution (including all
    personal information I submit with it, including my sign-off) is
    maintained indefinitely and may be redistributed consistent with
    this project or the open source license(s) involved.
```

## Code style

- C++20. clang-format and clang-tidy are enforced in CI.
- Every file must have an SPDX license header.
- core/ must not depend on any UI, OS, or graphics library.
- No C++ exceptions in core/: use error codes and `std::expected`.

## Testing

- New code must include unit tests (Catch2).
- Golden frame regressions block the merge.
- Maximum 800 net lines per PR unless generated tables.

## Code of Conduct

This project follows the [Contributor Covenant v2.1](https://www.contributor-covenant.org/version/2/1/code_of_conduct/).
