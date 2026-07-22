# Contributing to Mo

Thanks for your interest in contributing to Mo! This document explains how to
set up a development environment and the conventions the project follows.
Contributions in both English and Chinese are welcome.

## Development Environment

A helper script bootstraps the full toolchain on Debian 13:

```bash
./tools/setup_dev_env.sh
```

This installs the compiler toolchain (GCC, Clang), CMake, Ninja, Qt6 development
packages, KSyntaxHighlighting, libuchardet, extra-cmake-modules, and the static
analysis tools (clang-format, clang-tidy, cppcheck, clangd).

Verify the environment with:

```bash
cmake --version     # >= 3.31
qmake6 --version    # Qt >= 6.8
ninja --version
clang-format --version
```

## Building

```bash
cmake --preset default
cmake --build --preset default
ctest --preset test
```

## Code Style

- **C++ standard:** C++20.
- **Formatting:** enforced by `clang-format` using the Qt6 official style defined
  in [`.clang-format`](.clang-format). Run `clang-format -i <file>` before
  committing.
- **Static analysis:** `clang-tidy` with the checks in [`.clang-tidy`](.clang-tidy).
  Run `cmake --build build --target run-clang-tidy` when available.
- **Naming:** classes `CamelCase`, functions/variables `camelBack`, private
  members suffixed with `_`, constants `kUPPER_CASE`, namespaces `lower_case`.
- **Strings:** all user-visible text must use `tr()` so it can be translated.
  Source language is English; `zh_CN` is the first translation.

## SPDX Headers

Every source file must begin with the appropriate SPDX header:

- C++ files:
  ```cpp
  // SPDX-FileCopyrightText: 2026 Liu Dong <liudng@hotmail.com>
  // SPDX-License-Identifier: GPL-3.0-or-later
  ```
- CMake files:
  ```cmake
  # SPDX-FileCopyrightText: 2026 Liu Dong <liudng@hotmail.com>
  # SPDX-License-Identifier: GPL-3.0-or-later
  ```

## Commit Messages

Follow [Conventional Commits](https://www.conventionalcommits.org/):

```
<type>(<scope>): <description>

[optional body]

[optional footer(s)]
```

Allowed types: `feat`, `fix`, `docs`, `refactor`, `test`, `chore`, `perf`,
`style`, `ci`, `build`.

Examples:
- `feat(editor): add bracket matching`
- `fix(session): restore cursor column correctly`
- `docs(readme): update build instructions`

## Branch Naming

- `feature/<short-description>` — new features
- `fix/<short-description>` — bug fixes
- `chore/<short-description>` — tooling and maintenance

## Pull Requests

1. Fork the repository and create a branch from `main`.
2. Make focused commits following the message format above.
3. Ensure `clang-format` has been applied and `ctest --preset test` passes.
4. Update the [CHANGELOG.md](CHANGELOG.md) under `## [Unreleased]` if relevant.
5. Keep the PR scoped to one concern; large changes should be split.

## Testing

- All new features and bug fixes should be accompanied by tests.
- `ctest --preset test` must pass before a PR can be merged.
- Where practical, prefer QtTest unit tests under `tests/`.

## Communication

Issues and PRs may be written in English or Chinese. Please be respectful and
constructive.
