# FastHTML Changelog

## [0.1.0] - 2026-08-26

### Added
- Initial release of FastHTML native AVX2 HTML sanitizer and lexer.
- 256-bit SIMD tag scanning and attribute cleaning (`fasthtml.dll`).
- Multi-tier safety profiles: `STRICT_WHITELIST`, `RELAXED`, and `TEXT_ONLY`.
- Direct off-heap memory buffer sanitization via native memory address pointers.
- Interactive hero terminal diff demonstration (`run-demo.bat`).
