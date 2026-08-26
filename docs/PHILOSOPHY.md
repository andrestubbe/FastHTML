# FastHTML Philosophy

## Core Pillars

1. **Native AVX2 Vector Acceleration**: Scanning delimiters and tags with 256-bit SIMD registers rather than serial character parsing loops.
2. **Zero-DOM Overhead**: Eliminating heavy AST node tree allocations and garbage collector pauses by operating in a linear single-pass stream.
3. **Defense-in-Depth XSS Neutralization**: Scrubbing active execution tags, inline JavaScript handlers (`on*`), and dangerous URI schemes (`javascript:`) by default.
4. **Zero-Copy Memory Addressing**: Native compatibility with `FastPointer` and `FastMemory` for direct off-heap buffer processing.
