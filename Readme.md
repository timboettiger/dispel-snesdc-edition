
# DisPel v1.1.0 - DEADC0DE Edition

- (C) 2001 by James Churchill of Naruto, <pelrun@gmail.com>
- (C) 2024 DEADC0DE Edition by Tim Böttiger, <timboettiger@gmail.com>

## 65816 Action Replay Dialect Disassembler

DisPel is a 65816 disassembler originally developed by James Churchill. While it is not as fully featured as tools like Tracer (e.g., it is not suitable for NES ROMs), it serves many specific purposes.

The DEADC0DE Edition includes special modifications to disassemble specific "dead codes". These codes are significantly shorter than ROM files, lack headers, and contain unique quirks in their opcode dialect.

The highlight of the Edition is a new feature called *Annotions*. It helps analyzing the assembler code by providing detailed opcode annotations as well as a analysis of operands.

## Features

**General:**

- Disassemble specific ROM sections, a single bank, or the entire file
- Correct handling of `REP`/`SEP` instructions
- Accurate bank-boundary handling
- Automatic (but overridable) HiROM and LoROM detection
- Shadow ROM support
- User-defined listing origin (see below)
- Native SNES addressing support—no manual LoROM-offset conversion or header adjustments required
- Immediate cancellation with `Control-C`

**DEADC0DE Support:**

- Support for macOS/Linux binaries as targets (Makefile targets)
- More flexible validation of the minimum binary data length
- Assembler annotation generator
- Various output formatting themes (hexcode, assembler, annotated)
- Dynamic patch mode detection
- Disabled `brk` opcode operand in patch mode
- BSD license

### DEADC0DE Edition

When a dead code is being detected, the following message will be generated above the assembler instructions. It contains the length (bytes) of the patch, as well as the memory target where the patch will be injected or inserted.

```assembler
; Action Replay dead code patches 36 bytes from 0xC0025C:
```

In case the patch length is not defined within the patch header, it will be calculated. This is shown as follows:

```assembler
; No length defined. Dynamic mode.
```

In some cases the (specified!) patch length does not match the real length. If so, a warning will be displayed:

```assembler
; Warning: Patch length (28 bytes) mismatch. Allowed length: 36 bytes.
```

Dead codes were historically used in Action Replay MK II and MK III cheat devices and remained undocumented for years.

More details about this can be found in the associated [GitHub project](https://github.com/timboettiger/action-replay-mk-iii/blob/master/deadc0de-story.md).

### Annotations

Annotations are appending detailed annotations as well as a analysis of operands each line after the asm code.

```assembler
; Action Replay dead code patches 24 bytes from 0x02FC7A:
02/FC7A:    08        php                ; Push processor status onto stack.
02/FC7B:    E220      sep #$20           ; Set processor flags to 00100000 (binary representation of '32').
02/FC7D:    A928      lda #$28           ; Load accumulator with value from constant '40'.
02/FC7F:    8D9603    sta $0396          ; Store accumulator at SPC700 RAM (Work RAM) (at 0x0396).
02/FC82:    A905      lda #$05           ; Load accumulator with value from constant '5'.
02/FC84:    8DAD15    sta $15AD          ; Store accumulator at SPC700 RAM (Mirror) (at 0x15AD).
02/FC87:    28        plp                ; Pull processor status from stack.
02/FC88:    6468      stz $68            ; Store zero at 0x68.
02/FC8A:    6469      stz $69            ; Store zero at 0x69.
02/FC8C:    5C7EFC02  jmp $02FC7E        ; Jump to Hardware Registers and I/O Ports (at 0x02FC7E).
02/FC90:    00        brk                ; Force break.
02/FC91:    00        brk                ; Force break.
```

**Features:**

- Description of all SNES opcodes by symbol mapping
- Conversion of hexadecimal numbers to decimal representation
- Conversion of flags to binary and decimal representation
- Mapping of well known registers, interrupts and memory sections

### True SNES Addressing

DisPel simplifies disassembly by automatically handling SNES bank/address conversions. Users can specify addresses directly in SNES format, and the tool manages the details. If the ROM contains an SMC header, it can be skipped using the `-n` option.

For disassembly tasks requiring specific sections of a ROM, address ranges and banks can be defined explicitly, avoiding unnecessarily large outputs filled with disassembled graphics data.

HiROM addressing is enabled using the `-h` option. Tools like inSNESt can help determine whether HiROM addressing is needed.

### Correct REP/SEP State Parsing

Due to the 65816's backward compatibility with the 6502, certain opcodes can use either a 1-byte or 2-byte operand, depending on processor flags. DisPel maintains and updates these state flags during disassembly, ensuring alignment and preventing gibberish output.

Options like `-a` (8-bit accumulator mode) and `-x` (8-bit X/Y register mode) allow users to specify the initial state explicitly. By default, DisPel assumes 16-bit accumulator and X/Y register mode.

### Bank-Boundary Handling

DisPel enforces SNES bank boundaries to ensure accurate disassembly. If an instruction crosses a bank boundary, it is ignored, and the remaining bytes are displayed as raw data. Users can disable this enforcement with the `-d` option.

### Shadow ROM Support

Shadow ROM, or FastROM, is a feature of SNES hardware where ROM data is mirrored at higher banks for faster access. DisPel supports Shadow ROM addressing, ensuring consistency in listings by mapping the addresses correctly. This is especially useful for FastROM code with addresses above `$800000`.

FastROM is detected automatically, but users can enable or disable it manually with the `-s` or `-i` options.

### User-Specified Origin Support

The `-g` option allows users to specify a custom origin address for disassembly. This is helpful for cases where code is copied and executed at a different location than its original position in the ROM.

---

## Usage

```bash
DisPel 1.1.0
DEADC0DE Edition
Copyright (c) 2001 James Churchill
Copyright (c) 2024 Tim Böttiger

Usage: dispel [options] <inputfile>
Parameters:
    -n              Skip $200 byte SMC header (optional)
    -v              Enable verbose mode (optional)
    -s              Enable silent mode (optional)
    -f <format>     Output formatting (optional, default: 'standard')
                    Options:
                      - standard   address, hexdump, opcode with operand
                      - hexdump    hexdump only, specify width using -w
                      - assembler  opcode with operand
                      - annotated  address, hexdump, opcode with operand, annotation
    -h              Force HiROM memory mapping (optional)
    -l              Force LoROM memory mapping (optional)
    -a              Start in 8-bit accumulator mode (optional)
    -x              Start in 8-bit X/Y mode (optional)
    -e              Disable bank-boundary enforcement (optional)
    -p              Split subroutines with blank lines after RTS, RTL, RTI (optional)
    -w <width>      Set hexdump width in bytes per line (optional, default: 4)
    -o <output>     Set output channel (default: stdout)
                      - stdout      Output to standard output
                      - <file>      Output to a file

    <inputfile>     File to disassemble (required)
```

---

## Release History

- **v1.1.0 - 21/11/2024:** Code refactoring, added dynamic patch mode detection, added annotation feature, enhanced Makefile.
- **v1.0d - 15/11/2024:** Updated `-t` parameter handling, removed minimal size validation, and updated README.
- **v1.0001 - 5/4/2011:** Source code adjustments for public release.
- **v1.00 - 10/7/2002:** Bug fixes for missing input file handling.
- **v0.99 - 10/7/2001:** Fixed register options and HiROM support issues.
- **v0.96 - 3/2/2001:** Corrected operand formatting for certain opcodes.
- **v0.95 - 19/11/2000:** Added HiROM and FastROM detection.
- **v0.91 - 25/09/2000:** Fixed HiROM addressing.
- **v0.9 - 24/09/2000:** Initial Release.

---

## License

In consultation with the original developer and rights holder, James Churchill, the project is being released under the BSD license.
