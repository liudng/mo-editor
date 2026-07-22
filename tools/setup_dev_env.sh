#!/usr/bin/env bash
# SPDX-FileCopyrightText: 2026 Liu Dong <liudng@hotmail.com>
# SPDX-License-Identifier: GPL-3.0-or-later
#
# Install all development dependencies for the mo Qt6 text editor on
# Debian 13 (trixie). Re-run safely; existing packages are skipped.

set -euo pipefail

if ! command -v sudo >/dev/null 2>&1; then
    echo "error: sudo is required to install packages" >&2
    exit 1
fi

echo "==> Updating APT package index"
sudo apt update

echo "==> Installing base build tools"
sudo apt install -y --no-install-recommends \
    build-essential \
    cmake \
    ninja-build \
    git \
    pkg-config \
    clang \
    clang-tidy \
    clang-format \
    doxygen \
    graphviz \
    python3-sphinx \
    python3-breathe \
    python3-sphinx-rtd-theme

echo "==> Installing Qt6 development packages"
sudo apt install -y --no-install-recommends \
    qt6-base-dev \
    qt6-base-dev-tools \
    qt6-tools-dev \
    qt6-tools-dev-tools \
    qt6-l10n-tools \
    qt6-svg-dev

echo "==> Installing Wayland / input / GPU libraries"
sudo apt install -y --no-install-recommends \
    libwayland-dev \
    wayland-protocols \
    libxkbcommon-dev \
    libgl1-mesa-dev \
    libegl1-mesa-dev \
    mesa-common-dev

echo "==> Installing editor feature dependencies"
sudo apt install -y --no-install-recommends \
    libkf6syntaxhighlighting-dev \
    libuchardet-dev \
    breeze-icon-theme

echo "==> Installing packaging tooling"
sudo apt install -y --no-install-recommends \
    dpkg-dev \
    devscripts \
    debhelper \
    dh-make \
    rpm \
    flatpak \
    flatpak-builder

echo
echo "==> All dependencies installed. You can now configure the project with:"
echo "    cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug -DMO_BUILD_TESTS=ON"
echo "    cmake --build build"
echo "    ctest --test-dir build --output-on-failure"
