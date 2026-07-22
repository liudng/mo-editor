# Security Policy

## Supported Versions

Only the latest minor release line receives security updates.

| Version | Supported          |
| ------- | ------------------ |
| 0.1.x   | :white_check_mark: |
| < 0.1   | :x:                |

## Reporting a Vulnerability

If you discover a security vulnerability, **please do not open a public GitHub
issue**. Instead, report it privately so it can be triaged and fixed before
public disclosure.

- **Email:** liudng@hotmail.com
- **Subject prefix:** `[SECURITY] mo:`

Please include:
- A description of the issue and its potential impact.
- Steps to reproduce, including build configuration and platform details.
- Suggested fix or mitigation, if any.

### Response SLA

| Stage                     | Target      |
| ------------------------- | ----------- |
| Acknowledgement of report | 72 hours    |
| Initial assessment        | 7 days      |
| Fix or mitigation release | 30 days     |

You will receive a CVE identifier when applicable once the fix is published.

## Hardened Build

Production builds enable the following hardening flags (see `cmake/Hardening.cmake`):

- `-D_FORTIFY_SOURCE=3` — fortified glibc string/memory functions.
- `-fstack-protector-strong` — stack canaries on vulnerable functions.
- `-fcf-protection=full` — Intel CET control-flow integrity.
- `-fstack-clash-protection` — guard against stack-clash attacks.
- `-Wl,-z,relro,-z,now` — full RELRO and immediate binding.
- `-Wl,-z,noexecstack` — non-executable stack.
- `-fPIE` / `-pie` — position-independent executable.

Sanitizer builds are available via the `asan`, `tsan`, and `ubsan` CMake presets.

## Reproducible Builds

Releases are built reproducibly. To verify a build:

```bash
# Build with the release preset using a fixed SOURCE_DATE_EPOCH
SOURCE_DATE_EPOCH=$(git log -1 --pretty=%ct) cmake --preset release
cmake --build --preset release

# Compare the resulting hash with the published release hash
sha256sum build-release/src/mo
```

The published SHA-256 checksums are attached to each GitHub release. A
mismatch indicates a compromised or non-reproducible build and should be
reported as a security issue.
