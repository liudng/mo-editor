#!/usr/bin/env bash
# SPDX-FileCopyrightText: 2026 liudng <liudng@users.noreply.github.com>
# SPDX-License-Identifier: GPL-3.0-or-later
#
# Run clang-tidy across the project using the compile_commands.json produced by
# CMake. Configure the project with -DCMAKE_EXPORT_COMPILE_COMMANDS=ON first.

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"
BUILD_DIR="${PROJECT_ROOT}/build"

if [[ ! -f "${BUILD_DIR}/compile_commands.json" ]]; then
    echo "error: ${BUILD_DIR}/compile_commands.json not found." >&2
    echo "       Run: cmake -S ${PROJECT_ROOT} -B ${BUILD_DIR} -G Ninja -DCMAKE_EXPORT_COMPILE_COMMANDS=ON" >&2
    exit 1
fi

if ! command -v run-clang-tidy >/dev/null 2>&1; then
    echo "error: run-clang-tidy not found. Install clang-tidy." >&2
    exit 1
fi

# Collect C++ sources under src/.
mapfile -t sources < <(find "${PROJECT_ROOT}/src" -type f \( -name '*.cpp' -o -name '*.cxx' \) | sort)

if [[ ${#sources[@]} -eq 0 ]]; then
    echo "No C++ sources found under ${PROJECT_ROOT}/src" >&2
    exit 0
fi

echo "Running clang-tidy on ${#sources[@]} file(s) using ${BUILD_DIR}/compile_commands.json"
run-clang-tidy -p "${BUILD_DIR}" "${sources[@]}"
