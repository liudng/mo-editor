# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added

- Support for `mo file1 file2 ...` command-line arguments: each file opens
  in its own tab, and when an instance is already running, the arguments are
  forwarded to it via D-Bus (`io.github.liudng.mo` `/io/github/liudng/mo`
  `openFiles`), raising its window.
- `--help` and `--version` command-line options.

## [0.1.0] - 2026-07-21

### Added

- Initial project scaffold
- Qt6 Widgets-based text editor skeleton
- Multi-tab document interface
- KSyntaxHighlighting integration
- libuchardet encoding detection
- Session restore
- Find/Replace dialog
- Settings dialog
- Wayland-only support
- English (default) and Simplified Chinese translations
- Flatpak, deb, rpm, tgz packaging
- GitHub Actions CI/CD pipeline
