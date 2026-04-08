# 🖥️ SIMPLEX Assembler & Emulator

<div align="center">

![Language](https://img.shields.io/badge/Language-C%2B%2B-blue?style=for-the-badge&logo=cplusplus)
![ISA](https://img.shields.io/badge/ISA-SIMPLEX-orange?style=for-the-badge)
![License](https://img.shields.io/badge/License-MIT-green?style=for-the-badge)
![Course](https://img.shields.io/badge/CS2206-Computer%20Architecture-red?style=for-the-badge)
![Institute](https://img.shields.io/badge/IIT-Patna-purple?style=for-the-badge)

**A complete two-pass assembler and emulator for the SIMPLEX Instruction Set Architecture**

*CS2206 – Computer Architecture | IIT Patna*
*Mohith Chandra Gugulothu · 2403cs04*

</div>

---

## 📌 Table of Contents

- [Overview](#-overview)
- [Repository Structure](#-repository-structure)
- [Demo](#-demo)
- [Screen Recording](#-screen-recording)
- [Assembler](#-assembler)
- [Emulator](#-emulator)
- [Test Programs](#-test-programs)
- [Claims Document](#-claims-document)
- [License](#-license)

---

## 🔍 Overview

This project implements a **full toolchain** for the SIMPLEX ISA — a simple 32-bit instruction set architecture — consisting of:

- **`asm.cpp`** — A two-pass assembler that converts SIMPLEX assembly (`.asm`) into binary object files (`.o`), with comprehensive error/warning reporting.
- **`emu.cpp`** — An emulator that loads and executes the binary object files, with configurable tracing and memory dump output.

| Feature | Detail |
|---|---|
| ISA | SIMPLEX (19 opcodes, 0–18) |
| Assembler passes | 2 (symbol table → code generation) |
| Memory | 65,536 × 32-bit words |
| Numeric formats | Decimal, Octal (`0`-prefix), Hex (`0x`-prefix) |
| Pseudo-instruction | `SET` (assigns value to label, no machine code emitted) |
| Output files | `.lst`, `.log`, `.o` (assembler) · `.trace`, `.before`, `.after`, `.memdump` (emulator) |

---

## 📁 Repository Structure

```
simplex-assembler-emulator/
│
├── Programmings/               # Source code & compiled binaries
│   ├── asm.cpp                 # Two-pass assembler (main source)
│   ├── emu.cpp                 # Emulator (main source)
│   ├── Machine code.cpp        # Machine code generation helper
│   ├── Symbol table.cpp        # Symbol table helper
│   ├── asm                     # Compiled assembler binary (Linux)
│   ├── emu                     # Compiled emulator binary (Linux)
│   ├── test.asm  – test9.asm   # All test assembly programs
│   └── ...
│
├── Tests/                      # Test output folders (one per test)
│   ├── Bubble sort/
│   ├── Error test/
│   ├── Multiply two numbers using repeated addition/
│   ├── Sum of first N numbers/
│   ├── test1/ – test4/
│   └── ...
│
├── <!DOCTYPE html>.html        # Web demo interface
├── Commands.rtf                # Quick-reference command list
├── claims.txt                  # Authorship & testing claims
├── .gitignore
└── README.md
```

---

## 🖼️ Demo

> Screenshots of the repository and project structure:

### Repository Root
![Repo Root](assets/screenshot1.png)

### Programmings Folder — Source & Test Files
![Programmings Folder](assets/screenshot2.png)

### Tests Folder — Output Directories
![Tests Folder](assets/screenshot3.png)

> 💡 **To add your screenshots:** create an `assets/` folder in the repo root, upload the three screenshots as `screenshot1.png`, `screenshot2.png`, `screenshot3.png`, and they will render automatically above.

---

## 🎬 Screen Recording

> A full 6-minute walkthrough of compiling, assembling, and emulating SIMPLEX programs:

<!-- Replace the URL below with your actual video link (Google Drive / YouTube) -->
[![Watch the Demo](https://img.shields.io/badge/▶%20Watch%20Demo-6%20min%20walkthrough-red?style=for-the-badge&logo=youtube)](YOUR_VIDEO_LINK_HERE)

> **How to add your video:**
> - Upload to Google Drive → Share → "Anyone with link" → paste the link above in place of `YOUR_VIDEO_LINK_HERE`
> - Or upload to YouTube and paste the YouTube URL

---

## 🔧 Assembler

### How It Works

The assembler is a classic **two-pass design**:

| Pass | Job |
|---|---|
| `pass1()` | Scans all lines, builds the **symbol table** (labels → addresses), handles `SET` pseudo-instruction |
| `pass2()` | Resolves all forward references, generates **machine code**, writes output files |

### Output Files

| File | Content |
|---|---|
| `.lst` | Listing: address · machine code word · original source line |
| `.log` | Log: same as `.lst` plus all errors and warnings with line numbers |
| `.o` | Binary object: raw 32-bit machine code words (only error-free lines) |

### Error & Warning Detection

The assembler detects and reports all of the following with exact line numbers:

- Duplicate label definition
- Invalid label name
- Undefined label (forward reference unresolved)
- Missing operand / unexpected operand / extra text after operand
- Invalid mnemonic
- Invalid numeric constant
- `SET` without a label or value
- **Warning:** label declared but never used
- **Warning:** data directive without a label

### Compilation & Usage

```bash
# Linux / macOS
g++ asm.cpp -o asm
./asm <filename>.asm

# Windows
g++ asm.cpp -o asm
asm <filename>.asm
```

> **Note:** The `.asm` input file must be in the same directory as the executable.
> If there are no errors, three output files are produced: `<filename>.lst`, `<filename>.log`, `<filename>.o`.

### Example

```bash
$ ./asm test5.asm
Assembling: test5.asm
Output files generated:
  test5.lst
  test5.log
  test5.o
Errors: 0, Warnings: 0
```

---

## ⚙️ Emulator

### How It Works

The emulator loads a `.o` binary, maps it into a 65,536-word memory array, and executes each 32-bit instruction word according to the SIMPLEX specification. All 19 opcodes (0–18) are implemented with correct register and memory behaviour.

### Output Flags

Select outputs by passing `-o` followed by any combination of digits:

| Digit | File | Content |
|---|---|---|
| `1` | `.trace` | Step-by-step: PC · instruction word · mnemonic · operand · A, B, SP |
| `2` | `.memdump` | Final memory (4 words/line up to last non-zero) + final register state |
| `3` | `.before` | A, B, SP, PC **before** each instruction |
| `4` | `.after` | A, B, SP, PC **after** each instruction |

> If no `-o` flag is given, `.trace` is produced by default.

### Compilation & Usage

```bash
# Linux / macOS
g++ emu.cpp -o emu
./emu <filename>.o -o <flags>

# Windows
g++ emu.cpp -o emu
emu <filename>.o -o <flags>
```

### Examples

```bash
# Trace only (default)
./emu test5.o

# Trace + memory dump
./emu test4.o -o 12

# All four output files
./emu test4.o -o 1234

# Multiple object files in one run
./emu test5.o test6.o -o 2
```

---

## 🧪 Test Programs

All test `.asm` files are in `Programmings/`. Their outputs (`.lst`, `.log`, `.o`, `.trace`, etc.) are in the corresponding sub-folders under `Tests/`.

| Test | Program | Assembler Result | Emulator Result |
|---|---|---|---|
| `test1.asm` | Unused label · infinite loop · forward reference | ✅ 0 errors, 1 warning | Infinite loop traced correctly |
| `test2.asm` | Error detection | ⚠️ 9 errors — no `.o` produced | N/A |
| `test3.asm` | `SET` pseudo-instruction (`val=75`, `val2=66`) | ✅ 0 errors | A = 141 (0x8D) after HALT |
| `test4.asm` | Triangle numbers via recursion (N=10) | ✅ 0 errors | result = 55 (0x37) ✓ |
| `test5.asm` | Sum of integers 1..10 | ✅ 0 errors | result = 55 (0x37) ✓ |
| `test6.asm` | Multiplication 6×7 via repeated addition | ✅ 0 errors | result = 42 (0x2A) ✓ |
| `test8.asm` | Deliberate errors & warnings | ⚠️ 8 errors, 2 warnings — no `.o` | N/A |
| `test9.asm` | Bubble sort (10 elements) | ✅ 0 errors | Sorted array `[0..9]` in memory ✓ |

---

## 📄 Claims Document

The full authorship and testing claims for this submission are available in two formats:

- [`claims.txt`](./claims.txt) — Plain text version (in repo root)
- 📥 **[Download Claims PDF](./claims_Mohith.pdf)** — Formatted PDF version with detailed test results

> To add the PDF: upload `claims_Mohith.pdf` to the repo root and the link above will work automatically.

---

## 📜 License

```
MIT License

Copyright (c) 2026 Mohith Chandra Gugulothu

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
```

---

<div align="center">

Made with ❤️ by **Mohith Chandra Gugulothu** · `2403cs04`
IIT Patna — CS2206 Computer Architecture

</div>
