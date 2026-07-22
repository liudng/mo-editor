# Mo architecture overview

This document describes the high-level structure of the Mo Qt6 text editor,
the responsibilities of each module, and the key design decisions behind them.

## Module breakdown

Mo is organized into the following top-level modules under `src/`:

| Module      | Path             | Responsibility                                              |
|-------------|------------------|-------------------------------------------------------------|
| `core`      | `src/core/`      | Domain types: `Document`, `Settings`, session state.        |
| `models`    | `src/models/`    | Qt item models that bridge core types to the UI.            |
| `services`  | `src/services/`  | Stateless helpers: encoding detection, highlighter factory. |
| `ui`        | `src/ui/`        | Widgets: `MainWindow`, tabs, find/replace, dialogs.         |
| `app`       | `src/app/`       | Application entry point, single-instance, IPC.              |

### core

`core` owns the editor's data model. `Document` wraps a single open file:
its text content, file path, encoding, modification state and undo stack.
`Settings` persists window geometry, recent files, theme, icon theme and
feature flags via `QSettings`. Session restore is implemented on top of
`Settings` plus a JSON snapshot of open tabs. The icon theme setting feeds
`QIcon::setFallbackThemeName()` so that toolbar icons render even on desktops
whose default icon theme (e.g. Adwaita) lacks standard action icons.

### models

`models` exposes Qt-style item models so the UI layer never touches core
types directly. The recent-files model and the find/replace history model
live here.

### services

`services` collects stateless, side-effect-free helpers:

* `EncodingDetector` wraps libuchardet to guess the encoding of bytes read
  from disk before they are decoded into a `QString`.
* `HighlighterFactory` builds a `KSyntaxHighlighting::Repository`-backed
  highlighter for a given MIME type or file extension.

### ui

`ui` contains all widgets. `MainWindow` owns the `QTabWidget`, the
find/replace dock and the menu bar. Each tab holds an `EditorView` that
combines a `QPlainTextEdit` with a syntax highlighter. The UI talks to
`core` and `services` only; it never touches the filesystem directly except
through `Document`.

### app

`app` wires everything together: it parses command-line arguments, ensures a
single running instance (via the D-Bus service `io.github.liudng.mo`),
restores the previous session and constructs the `MainWindow`.

## Dependency graph

```
        +-------+
        |  app  |
        +---+---+
            |
            v
        +---+---+
        |  ui   |
        +---+---+
            |
      +-----+-----+
      |           |
      v           v
  +---+---+   +---+--------+
  | models|   | services   |
  +---+---+   +---+--------+
      |           |
      v           v
  +---+-----------+---+
  |       core       |
  +------------------+
```

* `app` depends on `ui`, `core`, `services`.
* `ui` depends on `models`, `core`, `services`.
* `models` depends on `core`.
* `services` depends on `core` (and external libraries only).
* `core` depends only on Qt6 Core.

This layering keeps the domain model free of UI concerns and makes the core
logic testable without a `QGuiApplication`.

## Key design decisions

### KSyntaxHighlighting over QSyntaxHighlighter

Qt's built-in `QSyntaxHighlighter` requires hand-written rules for every
language. KDE's `KSyntaxHighlighting` ships definition files for 200+ languages
and is already packaged on every major Linux distribution. Using it gives
Mo first-class syntax support with negligible maintenance cost.

### libuchardet for encoding detection

Detecting the encoding of arbitrary text is hard. libuchardet is the de facto
standard library for this on Linux (used by Firefox, VS Code's terminal, etc.)
and handles UTF-8, GB18030, Shift-JIS, Big5 and many more. Mo defers all
encoding decisions to libuchardet and falls back to UTF-8 with replacement
characters on failure.

### Wayland only

Mo targets Linux desktops running Wayland. Dropping X11 support simplifies
the build (no `qt5compat` or `xcb-*` dependencies), enables zero-copy buffer
paths and matches the reality of modern distributions. The offscreen QPA
platform is used for headless testing.

### Session restore as a first-class feature

Session state is persisted whenever a tab is opened, closed or modified, not
only on quit. This makes crash recovery cheap and keeps the restore path
identical to the normal startup path.

### Find/replace as a service, not a dialog

The find/replace UI is a docked widget backed by a `FindEngine` in `services`.
Keeping the matching logic out of the dialog makes it reusable for batch
operations (replace in files) and unit-testable without widgets.
