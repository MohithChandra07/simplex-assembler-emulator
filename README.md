# 🖥️ SIMPLEX Assembler & Emulator

<div align="center">

![Language](https://img.shields.io/badge/Language-C%2B%2B-blue?style=for-the-badge&logo=cplusplus)
![Web](https://img.shields.io/badge/Web-HTML%2FJS-orange?style=for-the-badge&logo=html5)
![ISA](https://img.shields.io/badge/ISA-SIMPLEX-purple?style=for-the-badge)
![License](https://img.shields.io/badge/License-MIT-green?style=for-the-badge)
![Course](https://img.shields.io/badge/CS2206-Computer%20Architecture-red?style=for-the-badge)
![Institute](https://img.shields.io/badge/IIT-Patna-darkblue?style=for-the-badge)

**A complete two-pass assembler and emulator for the SIMPLEX Instruction Set Architecture —**
**with a live interactive web studio**

*CS2206 – Computer Architecture | IIT Patna*
*Mohith Chandra Gugulothu · 2403cs04*

</div>

---

## 📌 Table of Contents

- [Overview](#-overview)
- [SIMPLEX Studio — Web App](#-simplex-studio--web-app)
- [Repository Structure](#-repository-structure)
- [Screen Recording](#-screen-recording)
- [Assembler](#-assembler)
- [Emulator](#-emulator)
- [Test Programs](#-test-programs)
- [Claims Document](#-claims-document)
- [License](#-license)

---

## 🔍 Overview

This project implements a **full toolchain** for the SIMPLEX ISA — a simple 32-bit instruction set architecture — consisting of:

| Component | File | Description |
|---|---|---|
| **Assembler** | `asm.cpp` | Two-pass assembler: converts `.asm` → binary `.o` with full error reporting |
| **Emulator** | `emu.cpp` | Executes `.o` files with configurable trace/dump output |
| **Web Studio** | `<!DOCTYPE html>.html` | Browser-based interactive assembler + emulator with AI explanations |

| Spec | Detail |
|---|---|
| ISA | SIMPLEX (19 opcodes, 0–18) |
| Assembler | Two-pass (symbol table → code generation) |
| Memory | 65,536 × 32-bit words |
| Numeric formats | Decimal, Octal (`0`-prefix), Hex (`0x`-prefix) |
| Pseudo-instruction | `SET` — assigns value to label, no machine code emitted |
| CLI output files | `.lst` `.log` `.o` (assembler) · `.trace` `.before` `.after` `.memdump` (emulator) |

---

## 🌐 SIMPLEX Studio — Web App

> A fully interactive browser-based IDE for the SIMPLEX ISA — no installation needed.

### ✨ Features

- **Assembly Input Panel** — Write or paste SIMPLEX assembly with line numbers; includes built-in sample programs (`test1`, `test3 SET`, `add two nums`, `loop`, `errors`)
- **Machine Code Output Table** — Displays Address · Machine Code · Label · Mnemonic · Operand · Notes for every assembled instruction
- **Symbol Table** — Shows all labels with their hex and decimal values in real time
- **Errors & Warnings Panel** — Highlights every error and warning with descriptions
- **Emulator — Step-by-Step Execution** — Live register view (A, B, SP, PC) with Step / +10 / Run All / Reset controls and a full execution trace table
- **🤖 AI Explanation Panel** — Click any instruction row to get an AI-powered explanation of what it does, how it's encoded, and what happens to the registers

### 📸 Screenshots

**Assembler View — SET pseudo-instruction (`test3.asm`) with symbol table and machine code output**

![SIMPLEX Studio Assembler](assets/demo1.png)

---

**Emulator View — Step-by-step register execution with AI explanation panel**

![SIMPLEX Studio Emulator](assets/demo2.png)

---

### 🚀 How to Run the Web App

The web app is a **single self-contained HTML file** — no server or dependencies needed.

```bash
# Clone the repo
git clone https://github.com/MohithChandra07/simplex-assembler-emulator.git
cd simplex-assembler-emulator

# Then just open the HTML file in your browser
open "<!DOCTYPE html>.html"        # macOS
start "<!DOCTYPE html>.html"       # Windows
xdg-open "<!DOCTYPE html>.html"    # Linux
```

> Works in any modern browser (Chrome, Firefox, Safari, Edge).
> No internet connection required — except for the AI Explanation feature.

---

## 📁 Repository Structure

```
simplex-assembler-emulator/
│
├── Programmings/                        # Source code & compiled binaries
│   ├── asm.cpp                          # Two-pass assembler
│   ├── emu.cpp                          # Emulator
│   ├── Machine code.cpp                 # Machine code generation helper
│   ├── Symbol table.cpp                 # Symbol table helper
│   ├── asm / emu                        # Compiled binaries (Linux)
│   └── test.asm … test9.asm            # All test assembly programs
│
├── Tests/                               # Test output folders
│   ├── Bubble sort/
│   ├── Error test/
│   ├── Multiply two numbers using repeated addition/
│   ├── Sum of first N numbers/
│   └── test1/ … test4/
│
├── <!DOCTYPE html>.html                 # 🌐 SIMPLEX Studio — web app
├── Commands.rtf                         # Quick-reference command list
├── claims.txt                           # Plain-text claims
├── Claims.pdf                           # 📄 Formatted claims PDF
├── assets/                              # README images (add yours here)
│   ├── demo1.png                        # Assembler screenshot
│   └── demo2.png                        # Emulator screenshot
├── .gitignore
└── README.md
```

---

## 🎬 Screen Recording

> A full 6-minute walkthrough of compiling, assembling, emulating, and using SIMPLEX Studio:

<!-- ──────────────────────────────────────────────────────────────────
  HOW TO ADD YOUR VIDEO — choose one option:

  OPTION A · Google Drive
    1. Upload your .mp4 to Google Drive
    2. Right-click the file → Share → set to "Anyone with the link"
    3. Copy the share link
    4. Replace YOUR_VIDEO_LINK_HERE below with that link

  OPTION B · YouTube (recommended — embeds a thumbnail automatically)
    1. Go to YouTube → Upload → set visibility to Unlisted or Public
    2. Copy the video URL (e.g. https://youtu.be/abc123)
    3. Replace YOUR_VIDEO_LINK_HERE below with that URL
    4. Also replace YOUR_YOUTUBE_ID with just the video ID (e.g. abc123)
       to get a proper thumbnail image instead of the badge

  After replacing, your README will show a clickable thumbnail like this:
    [![Demo](https://img.youtube.com/vi/YOUR_YOUTUBE_ID/0.jpg)](YOUR_VIDEO_LINK_HERE)
────────────────────────────────────────────────────────────────── -->

[![▶ Watch 6-min Demo](https://img.shields.io/badge/▶%20Watch%20Demo-6%20min%20full%20walkthrough-red?style=for-the-badge&logo=youtube)](YOUR_VIDEO_LINK_HERE)

**What the recording covers:**
- Compiling `asm.cpp` and `emu.cpp` with `g++`
- Assembling test files and inspecting `.lst` / `.log` / `.o` output
- Running the emulator with different `-o` flag combinations
- Live demo of SIMPLEX Studio in the browser

---

## 🔧 Assembler

### How It Works

| Pass | Job |
|---|---|
| `pass1()` | Scans all lines, builds the **symbol table** (labels → addresses), handles `SET` |
| `pass2()` | Resolves forward references, generates **machine code**, writes output files |

### Output Files

| File | Content |
|---|---|
| `.lst` | Address · machine code word · original source line |
| `.log` | Same as `.lst` plus all errors and warnings with line numbers |
| `.o` | Raw binary: 32-bit machine code words (error-free lines only) |

### Error & Warning Detection

- Duplicate label · invalid label name · undefined label
- Missing operand · unexpected operand · extra text after operand
- Invalid mnemonic · invalid numeric constant
- `SET` without a label or value
- ⚠ Warning: label declared but never used
- ⚠ Warning: data directive without a label

### Compilation & Usage

```bash
# Linux / macOS
g++ asm.cpp -o asm
./asm <filename>.asm

# Windows
g++ asm.cpp -o asm
asm <filename>.asm
```

```
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

### Output Flags

| Digit | File | Content |
|---|---|---|
| `1` | `.trace` | Step-by-step: PC · instruction · mnemonic · operand · A, B, SP |
| `2` | `.memdump` | Final memory (4 words/line) + final register state |
| `3` | `.before` | A, B, SP, PC **before** each instruction |
| `4` | `.after` | A, B, SP, PC **after** each instruction |

> Default (no flag): `.trace` is produced.

### Compilation & Usage

```bash
# Linux / macOS
g++ emu.cpp -o emu
./emu <filename>.o -o <flags>

# Windows
g++ emu.cpp -o emu
emu <filename>.o -o <flags>
```

```bash
./emu test5.o              # trace only (default)
./emu test4.o -o 12        # trace + memdump
./emu test4.o -o 1234      # all four output files
./emu test5.o test6.o -o 2 # multiple files in one run
```

---

## 🧪 Test Programs

| Test | Program | Assembler | Emulator Result |
|---|---|---|---|
| `test1.asm` | Unused label · infinite loop · forward ref | ✅ 0 errors, 1 warning | Loop traced correctly |
| `test2.asm` | Error detection | ⚠️ 9 errors — no `.o` | N/A |
| `test3.asm` | `SET` pseudo-instruction (`val=75`, `val2=66`) | ✅ 0 errors | A = 141 (0x8D) |
| `test4.asm` | Triangle numbers via recursion (N=10) | ✅ 0 errors | result = 55 (0x37) ✓ |
| `test5.asm` | Sum of integers 1..10 | ✅ 0 errors | result = 55 (0x37) ✓ |
| `test6.asm` | Multiplication 6×7 via repeated addition | ✅ 0 errors | result = 42 (0x2A) ✓ |
| `test8.asm` | Deliberate errors & warnings | ⚠️ 8 errors, 2 warnings — no `.o` | N/A |
| `test9.asm` | Bubble sort (10 elements) | ✅ 0 errors | Sorted `[0..9]` in memory ✓ |

---

## 📄 Claims Document

Full authorship and testing claims are available in two formats:

| Format | Link |
|---|---|
| Plain text | [`claims.txt`](./claims.txt) |
| Formatted PDF | [`Claims.pdf`](./Claims.pdf) — download for full test results, assembler/emulator docs, and declaration of authorship |

---

## 📜 License

```
MIT License — Copyright (c) 2026 Mohith Chandra Gugulothu

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
