/* describe.c
 * Description generator for 65C816 opcodes and
 * SNES memory-mapped registers
 * Created 190924 by Tim Böttiger
 * Last Modified 151025
 */

#include <string.h>
#include <regex.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>

#include "dispel.h"

/* 65C816-focused translation table for memory-mapped registers and opcodes */
TranslationEntry translationTable[] = {
    // ---------------
    // PPU (2100–213F)
    // ---------------
    {"^0x2100$", "INIDISP - Display Control"},
    {"^0x2101$", "OBSEL - Object Size and Data Area Designation"},
    {"^0x2102$", "OAMADDL - OAM Address (Low)"},
    {"^0x2103$", "OAMADDH - OAM Address (High)"},
    {"^0x2104$", "OAMDATA - OAM Data Write"},
    {"^0x2105$", "BGMODE - BG Mode and Character Size"},
    {"^0x2106$", "MOSAIC - Mosaic Size and Enable"},
    {"^0x2107$", "BG1SC - BG1 Screen Base Address"},
    {"^0x2108$", "BG2SC - BG2 Screen Base Address"},
    {"^0x2109$", "BG3SC - BG3 Screen Base Address"},
    {"^0x210A$", "BG4SC - BG4 Screen Base Address"},
    {"^0x210B$", "BG12NBA - BG1/BG2 Character Data Area"},
    {"^0x210C$", "BG34NBA - BG3/BG4 Character Data Area"},
    {"^0x210D$", "BG1HOFS - BG1 Horizontal Scroll"},
    {"^0x210E$", "BG1VOFS - BG1 Vertical Scroll"},
    {"^0x210F$", "BG2HOFS - BG2 Horizontal Scroll"},
    {"^0x2110$", "BG2VOFS - BG2 Vertical Scroll"},
    {"^0x2111$", "BG3HOFS - BG3 Horizontal Scroll"},
    {"^0x2112$", "BG3VOFS - BG3 Vertical Scroll"},
    {"^0x2113$", "BG4HOFS - BG4 Horizontal Scroll"},
    {"^0x2114$", "BG4VOFS - BG4 Vertical Scroll"},
    {"^0x2115$", "VMAIN - VRAM Address Increment Mode"},
    {"^0x2116$", "VMADDL - VRAM Address (Low)"},
    {"^0x2117$", "VMADDH - VRAM Address (High)"},
    {"^0x2118$", "VMDATAL - VRAM Data Write (Low)"},
    {"^0x2119$", "VMDATAH - VRAM Data Write (High)"},
    {"^0x211A$", "M7SEL - Mode 7 Settings"},
    {"^0x211B$", "M7A - Mode 7 Matrix A"},
    {"^0x211C$", "M7B - Mode 7 Matrix B"},
    {"^0x211D$", "M7C - Mode 7 Matrix C"},
    {"^0x211E$", "M7D - Mode 7 Matrix D"},
    {"^0x211F$", "M7X - Mode 7 Center X Position"},
    {"^0x2120$", "M7Y - Mode 7 Center Y Position"},
    {"^0x2121$", "CGADD - CGRAM Address"},
    {"^0x2122$", "CGDATA - CGRAM Data Write"},
    {"^0x2123$", "W12SEL - Window Mask Settings for BG1/BG2"},
    {"^0x2124$", "W34SEL - Window Mask Settings for BG3/BG4"},
    {"^0x2125$", "WOBJSEL - Window Mask Settings for Objects"},
    {"^0x2126$", "WH0 - Window 1 Left Position"},
    {"^0x2127$", "WH1 - Window 1 Right Position"},
    {"^0x2128$", "WH2 - Window 2 Left Position"},
    {"^0x2129$", "WH3 - Window 2 Right Position"},
    {"^0x212A$", "WBGLOG - Window Mask Logic for BGs"},
    {"^0x212B$", "WOBJLOG - Window Mask Logic for Objects"},
    {"^0x212C$", "TM - Main Screen Designation"},
    {"^0x212D$", "TS - Sub Screen Designation"},
    {"^0x212E$", "TMW - Window Mask for Main Screen"},
    {"^0x212F$", "TSW - Window Mask for Sub Screen"},
    {"^0x2130$", "CGWSEL - Color Addition/Subtraction Select"},
    {"^0x2131$", "CGADSUB - Color Math Designation"},
    {"^0x2132$", "COLDATA - Color Data"},
    {"^0x2133$", "SETINI - Screen Mode/Video Select"},
    {"^0x2134$", "MPYL - Multiplication Result (Low)"},
    {"^0x2135$", "MPYM - Multiplication Result (Middle)"},
    {"^0x2136$", "MPYH - Multiplication Result (High)"},
    {"^0x2137$", "SLHV - Software Latch for H/V Counters"},
    {"^0x2138$", "OAMDATAREAD - OAM Data Read"},
    {"^0x2139$", "VMDATALREAD - VRAM Data Read (Low)"},
    {"^0x213A$", "VMDATAHREAD - VRAM Data Read (High)"},
    {"^0x213B$", "CGDATAREAD - CGRAM Data Read"},
    {"^0x213C$", "OPHCT - Horizontal Scanline Position"},
    {"^0x213D$", "OPVCT - Vertical Scanline Position"},
    {"^0x213E$", "STAT77 - PPU Status Flag"},
    {"^0x213F$", "STAT78 - PPU Status Flag"},
    /* Fallback for any PPU register in 2100–213F */
    {"^0x21[0-3][0-9A-Fa-f]{2}$", "PPU Register (at @m)"},

    // ---------------------------------
    // APU I/O (CPU side) & WRAM ports
    // ---------------------------------
    {"^0x2140$", "APUI0 - APU I/O Port 0"},
    {"^0x2141$", "APUI1 - APU I/O Port 1"},
    {"^0x2142$", "APUI2 - APU I/O Port 2"},
    {"^0x2143$", "APUI3 - APU I/O Port 3"},

    {"^0x2180$", "WMDATA - Work RAM Data Register"},
    {"^0x2181$", "WMADDL - Work RAM Address (Low)"},
    {"^0x2182$", "WMADDH - Work RAM Address (High)"},
    {"^0x2183$", "WMADDB - Work RAM Address (Bank)"},

    {"^0x2188$", "S-RTC Data Port (Write)"},
    {"^0x2189$", "S-RTC Data Port (Read)"},
    {"^0x218A$", "S-RTC Control Register"},
    {"^0x218B$", "S-RTC Status Register"},

    // --------------------
    // Joypad latches (CPU)
    // --------------------
    {"^0x4016$", "JOYSER0 - Joypad Port 1"},
    {"^0x4017$", "JOYSER1 - Joypad Port 2"},

    // ------------------------------------------
    // CPU-internal & joypad readback (4200–421F)
    // ------------------------------------------
    {"^0x4200$", "NMITIMEN - Interrupt Enable Flags"},
    {"^0x4201$", "WRIO - Programmable I/O Port"},
    {"^0x4202$", "WRMPYA - Multiplicand"},
    {"^0x4203$", "WRMPYB - Multiplier"},
    {"^0x4204$", "WRDIVL - Dividend (Low)"},
    {"^0x4205$", "WRDIVH - Dividend (High)"},
    {"^0x4206$", "WRDIVB - Divisor"},
    {"^0x4207$", "HTIMEL - H Timer (Low)"},
    {"^0x4208$", "HTIMEH - H Timer (High)"},
    {"^0x4209$", "VTIMEL - V Timer (Low)"},
    {"^0x420A$", "VTIMEH - V Timer (High)"},
    {"^0x420B$", "MDMAEN - General DMA Enable"},
    {"^0x420C$", "HDMAEN - H-Blank DMA Enable"},
    {"^0x420D$", "MEMSEL - ROM Speed"},

    {"^0x4210$", "RDNMI - NMI Flag and 5A22 Version"},
    {"^0x4211$", "TIMEUP - IRQ Flag"},
    {"^0x4212$", "HVBJOY - PPU Status Register"},
    {"^0x4213$", "RDIO - Programmable I/O Port (Read)"},
    {"^0x4214$", "RDDIVL - Division Result (Low)"},
    {"^0x4215$", "RDDIVH - Division Result (High)"},
    {"^0x4216$", "RDMPYL - Multiplication Result (Low)"},
    {"^0x4217$", "RDMPYH - Multiplication Result (High)"},
    {"^0x4218$", "JOY1L - Joypad 1 Data (Low)"},
    {"^0x4219$", "JOY1H - Joypad 1 Data (High)"},
    {"^0x421A$", "JOY2L - Joypad 2 Data (Low)"},
    {"^0x421B$", "JOY2H - Joypad 2 Data (High)"},
    {"^0x421C$", "JOY3L - Joypad 3 Data (Low)"},
    {"^0x421D$", "JOY3H - Joypad 3 Data (High)"},
    {"^0x421E$", "JOY4L - Joypad 4 Data (Low)"},
    {"^0x421F$", "JOY4H - Joypad 4 Data (High)"},
    /* Fallback for 4200–421F */
    {"^0x42[0-1][0-9A-Fa-f]{2}$", "CPU/PPU I/O Register (at @m)"},

    // --------------
    // Expansion port
    // --------------
    {"^0x4800$", "EXPANSION - Expansion Port Register 0"},
    {"^0x4801$", "EXPANSION - Expansion Port Register 1"},
    {"^0x4802$", "EXPANSION - Expansion Port Register 2"},
    {"^0x4803$", "EXPANSION - Expansion Port Register 3"},
    {"^0x4804$", "EXPANSION - Expansion Port Register 4"},
    {"^0x4805$", "EXPANSION - Expansion Port Register 5"},
    {"^0x4806$", "EXPANSION - Expansion Port Register 6"},
    {"^0x4807$", "EXPANSION - Expansion Port Register 7"},

    // ------------------------
    // DMA registers (4300–437F)
    // ------------------------
    {"^0x43[0-7][0-9A-Fa-f]{2}$", "DMA Register (Channel 0–7, at @m)"},

    // ------------------------------
    // WRAM (7E/7F) & low-WRAM mirror
    // ------------------------------
    {"^0x7E[0-9A-Fa-f]{4}$", "Work RAM (WRAM) (at @m)"},
    {"^0x7F[0-9A-Fa-f]{4}$", "Mirror of WRAM (at @m)"},
    /* 16-bit addresses (without bank) */
    {"^0x0[0-1][0-9A-Fa-f]{3}$", "Low WRAM mirror ($7E:0000–$7E:1FFF) (at @m)"},

    // ---------------------------------------------
    // Reset/Interrupt vectors (Bank $00)
    // ---------------------------------------------
    {"^0x00FFFA$", "NMI Vector"},
    {"^0x00FFFC$", "Reset Vector"},
    {"^0x00FFFE$", "IRQ/BRK Vector"},

    // ------------------------------------
    // Instruction descriptions (65C816)
    // ------------------------------------
    /* NOTE: Keep @s for operands because they may be immediates or indexed.
       Use @m only where a memory address is guaranteed (we cannot know addressing
       mode from mnemonic alone). */
    {"^adc$", "Add with carry to @s"},
    {"^and$", "Logical AND with @s"},
    {"^asl$", "Arithmetic shift left on @s"},
    {"^bcc$", "Branch to @m if carry is clear"},
    {"^bcs$", "Branch to @m if carry is set"},
    {"^beq$", "Branch to @m if equal"},
    {"^bit$", "Test bits in @s"},
    {"^bmi$", "Branch to @m if minus"},
    {"^bne$", "Branch to @m if not equal"},
    {"^bpl$", "Branch to @m if positive"},
    {"^bra$", "Unconditional branch to @m"},
    {"^brk$", "Force break"},
    {"^brl$", "Branch long to @s"},
    {"^bvc$", "Branch to @m if overflow is clear"},
    {"^bvs$", "Branch to @m if overflow is set"},
    {"^clc$", "Clear carry flag"},
    {"^cld$", "Clear decimal mode flag"},
    {"^cli$", "Clear interrupt disable flag"},
    {"^clv$", "Clear overflow flag"},
    {"^cmp$", "Compare accumulator with @s"},
    {"^cop$", "Coprocessor call"},
    {"^cpx$", "Compare X register with @s"},
    {"^cpy$", "Compare Y register with @s"},
    {"^dec$", "Decrement value at @s"},
    {"^dex$", "Decrement X register"},
    {"^dey$", "Decrement Y register"},
    {"^eor$", "Exclusive OR with @s"},
    {"^inc$", "Increment value at @s"},
    {"^inx$", "Increment X register"},
    {"^iny$", "Increment Y register"},
    /* Jumps/subroutines always target memory; prefer @m for clarity */
    {"^jmp$", "Jump to @m"},
    {"^jml$", "Long jump to @m"},
    {"^jsr$", "Jump to subroutine at @m"},
    {"^jsl$", "Long jump to subroutine at @m"},
    {"^lda$", "Load accumulator with value from @s"},
    {"^ldx$", "Load X register with @s"},
    {"^ldy$", "Load Y register with @s"},
    {"^lsr$", "Logical shift right on @s"},
    /* MVN/MVP operands are bank bytes, not full addresses → keep @s */
    {"^mvn$", "Block move negative from source @s to destination @s"},
    {"^mvp$", "Block move positive from source @s to destination @s"},
    {"^nop$", "No operation"},
    {"^ora$", "Logical OR with @s"},
    {"^pea$", "Push effective address @m onto stack"},
    {"^pei$", "Push indirect address @m onto stack"},
    {"^per$", "Push program counter relative address @m onto stack"},
    {"^pha$", "Push accumulator onto stack"},
    {"^phb$", "Push data bank register onto stack"},
    {"^phd$", "Push direct page register onto stack"},
    {"^phk$", "Push program bank register onto stack"},
    {"^php$", "Push processor status onto stack"},
    {"^phx$", "Push X register onto stack"},
    {"^phy$", "Push Y register onto stack"},
    {"^pla$", "Pull accumulator from stack"},
    {"^plb$", "Pull data bank register from stack"},
    {"^pld$", "Pull direct page register from stack"},
    {"^plp$", "Pull processor status from stack"},
    {"^plx$", "Pull X register from stack"},
    {"^ply$", "Pull Y register from stack"},
    {"^rep$", "Clear processor status bits present in @b"},
    {"^rol$", "Rotate left on @s"},
    {"^ror$", "Rotate right on @s"},
    {"^rti$", "Return from interrupt"},
    {"^rtl$", "Long return from subroutine"},
    {"^rts$", "Return from subroutine"},
    {"^sbc$", "Subtract with carry from @s"},
    {"^sec$", "Set carry flag"},
    {"^sed$", "Set decimal mode flag"},
    {"^sei$", "Set interrupt disable flag"},
    {"^sep$", "Set processor status bits present in @b"},
    {"^sta$", "Store accumulator at @s"},
    {"^stp$", "Stop processor"},
    {"^stx$", "Store X register at @s"},
    {"^sty$", "Store Y register at @s"},
    {"^stz$", "Store zero at @s"},
    {"^tax$", "Transfer accumulator to X register"},
    {"^tay$", "Transfer accumulator to Y register"},
    {"^tcd$", "Transfer accumulator to direct page register"},
    {"^tcs$", "Transfer accumulator to stack pointer"},
    {"^tdc$", "Transfer direct page register to accumulator"},
    {"^trb$", "Test and reset bits at @s"},
    {"^tsb$", "Test and set bits at @s"},
    {"^tsc$", "Transfer stack pointer to accumulator"},
    {"^tsx$", "Transfer stack pointer to X register"},
    {"^txa$", "Transfer X register to accumulator"},
    {"^txs$", "Transfer X register to stack pointer"},
    {"^txy$", "Transfer X register to Y register"},
    {"^tya$", "Transfer Y register to accumulator"},
    {"^tyx$", "Transfer Y register to X register"},
    {"^wai$", "Wait for interrupt"},
    {"^wdm$", "Reserved for future expansion"},
    {"^xba$", "Exchange accumulator bytes"},
    {"^xce$", "Exchange carry and emulation flags"}
};

const int translationTableSize = sizeof(translationTable) / sizeof(TranslationEntry);

/* Converts $-style addresses to 0x-prefixed hex (pass-through for others) */
void convertToHexFormat(const char* address, char* out) {
    if (address[0] == '$') {
        sprintf(out, "0x%s", address + 1);
    } else {
        strcpy(out, address);
    }
}

/* Converts immediates (#$.. or #..) to decimal wrapped in single quotes */
void convertToDecFormat(const char* number, char* out) {
    if (number[0] == '#') {
        long v;
        if (number[1] == '$') {
            v = strtol(number + 2, NULL, 16);
        } else {
            v = strtol(number + 1, NULL, 10);
        }
        sprintf(out, "'%ld'", v);
    } else {
        strcpy(out, number);
    }
}

/* Converts immediate values to 8-bit binary with 0b prefix */
void convertToFlagFormat(const char* number, char* out) {
    long v = 0;
    if (number[0] == '#') {
        v = (number[1] == '$') ? strtol(number + 2, NULL, 16)
                               : strtol(number + 1, NULL, 10);
        v &= 0xFF;
        char bits[9] = {0};
        for (int i = 7; i >= 0; --i)
            bits[7 - i] = (v & (1 << i)) ? '1' : '0';
        sprintf(out, "0b%s", bits);
    } else {
        strcpy(out, number);
    }
}

/* NEW: Convert a memory-looking token to bb/aacc.
 * Accepts forms like "$1234", "$001234", "0x1234", "0x001234",
 * as well as wrapped/suffixed variants "($1234)", "[$1234]", "$1234,X", etc.
 * If only 4 hex digits are found -> bank=00. If 6 -> use high 2 as bank.
 * If no 4+ hex digits are found, falls back to hex-formatting.
 */
void convertToMemFormat(const char* token, char* out) {
    char hex[16] = {0};
    int hlen = 0;

    /* Scan token and collect continuous hex digits after an optional 0x/$
       or any bracket; stop only when we amassed enough or hit non-hex after start. */
    const char* p = token;

    /* Skip wrappers and prefixes gracefully */
    while (*p && (*p=='(' || *p=='[' || *p==' ')) p++;

    if (p[0]=='#') {
        /* immediates are not memory addresses: leave as-is */
        strcpy(out, token);
        return;
    }

    if (p[0]=='0' && (p[1]=='x' || p[1]=='X')) p += 2;
    else if (p[0]=='$') p += 1;

    /* Collect hex digits possibly across the token until a non-hex */
    const char* q = p;
    while (*q && isxdigit((unsigned char)*q) && hlen < (int)sizeof(hex)-1) {
        hex[hlen++] = (char)toupper((unsigned char)*q);
        q++;
    }

    if (hlen >= 6) {
        /* Use last 6 digits to be robust (e.g., if a bank is embedded) */
        const char* base = hex + (hlen - 6);
        char bb[3] = { base[0], base[1], 0 };
        char aacc[5] = { base[2], base[3], base[4], base[5], 0 };
        sprintf(out, "%s/%s", bb, aacc);
    } else if (hlen >= 4) {
        const char* base = hex + (hlen - 4);
        char aacc[5] = { base[0], base[1], base[2], base[3], 0 };
        sprintf(out, "00/%s", aacc);
    } else {
        /* Not a parseable memory address: present as hex-form (or raw) */
        convertToHexFormat(token, out);
    }
}

/* Smart conversion selector:
 *   - mode == 'b' -> binary (for immediates)
 *   - mode == 'h' -> force hex formatting
 *   - mode == 'm' -> memory bb/aacc formatting (2-byte => 00/aacc; 3-byte => bb/aacc)
 *   - default ('s'): immediates -> decimal in single quotes, otherwise hex
 */
static void convertSmart(const char* repl, char mode, char* out) {
    if (mode == 'b') { convertToFlagFormat(repl, out); return; }
    if (mode == 'h') { convertToHexFormat(repl, out);  return; }
    if (mode == 'm') { convertToMemFormat(repl, out);  return; }

    /* default @s */
    if (repl[0] == '#') convertToDecFormat(repl, out);
    else                convertToHexFormat(repl, out);
}

/* Multi-operand template processor.
 * Replaces *each* unescaped @X (X in {s,b,h,m}) with the next replacement string,
 * applying smart conversion per placeholder mode. If replacements are exhausted,
 * the last replacement is reused.
 * Escaped '@' (written as '\@' in the source literal) are emitted literally '@'.
 */
void process_template_multi(const char* tpl,
                            const char** replacements,
                            size_t repl_count,
                            char* output,
                            size_t out_size)
{
    size_t i = 0, k = 0;
    bool escape = false;
    size_t ri = 0; /* replacement index */

    if (repl_count == 0) {
        /* nothing to replace with: just copy while unescaping */
        while (tpl[i] != '\0' && k + 1 < out_size) {
            if (!escape && tpl[i] == '\\') { escape = true; i++; continue; }
            output[k++] = tpl[i++];
            escape = false;
        }
        output[k] = '\0';
        return;
    }

    while (tpl[i] != '\0' && k + 1 < out_size) {
        if (!escape && tpl[i] == '\\') { /* start escape, skip backslash */
            escape = true;
            i++;
            continue;
        }

        if (!escape && tpl[i] == '@' && tpl[i + 1] != '\0') {
            char mode = tpl[i + 1];
            if (mode == 's' || mode == 'b' || mode == 'h' || mode == 'm') {
                const char* cur = replacements[ri < repl_count ? ri : repl_count - 1];
                char buf[256]; buf[0] = '\0';
                convertSmart(cur, mode, buf);
                size_t len = strnlen(buf, sizeof(buf));
                if (k + len >= out_size) len = out_size - 1 - k;
                memcpy(output + k, buf, len);
                k += len;
                i += 2;
                if (ri + 1 < repl_count) ri++; /* advance until last, then stick */
                continue;
            }
        }

        /* regular char or escaped '@' */
        output[k++] = tpl[i++];
        escape = false;
    }
    output[k] = '\0';
}

/* Keep for compatibility – returns first placeholder found, defaults to @s */
void extract_placeholder(const char *descriptionTemplate, char *placeholder) {
    int length = (int)strlen(descriptionTemplate);
    int i = 0;
    placeholder[0] = '@';
    placeholder[1] = 's';
    placeholder[2] = '\0';

    while (i < length - 1) {
        if (descriptionTemplate[i] == '@' &&
            ((descriptionTemplate[i + 1] >= 'A' && descriptionTemplate[i + 1] <= 'Z') ||
             (descriptionTemplate[i + 1] >= 'a' && descriptionTemplate[i + 1] <= 'z'))) {
            placeholder[0] = descriptionTemplate[i];
            placeholder[1] = descriptionTemplate[i + 1];
            placeholder[2] = '\0';
            return;
        }
        i++;
    }
}

const char* describe(unsigned long pos, const char* input, const char *translationTemplate) {
    regex_t regex;
    int result;
    static char lookupValue[512];
    static char explanation[512];
    static char tempExplanation[1024];

    /* Parse input: main token (before first comma) and operands (after) */
    char tempInput[256];
    strncpy(tempInput, input, sizeof(tempInput));
    tempInput[sizeof(tempInput) - 1] = '\0';

    char *tokens[8];
    size_t token_count = 0;

    char *p = tempInput;
    while (*p && token_count < 8) {
        /* split by comma */
        char *comma = strchr(p, ',');
        if (comma) *comma = '\0';
        /* trim leading spaces */
        while (isspace((unsigned char)*p)) p++;
        /* trim trailing spaces */
        char *end = p + strlen(p);
        while (end > p && isspace((unsigned char)end[-1])) { end--; }
        *end = '\0';
        tokens[token_count++] = p;
        if (!comma) break;
        p = comma + 1;
    }

    const char* mainInput = (token_count > 0) ? tokens[0] : "";
    const char* operands[7];
    size_t operand_count = (token_count > 1) ? (token_count - 1) : 0;
    for (size_t i = 0; i < operand_count; ++i) operands[i] = tokens[i + 1];

    /* Build lookupValue for the main token (addresses become hex with optional patch detail) */
    if (mainInput[0] == '$') {
        char* hex_memory = (char*)&mainInput[1];
        unsigned long dec_memory = strtoul(hex_memory, NULL, 16);
        int internal = dec_memory >= patch_address_start && dec_memory <= patch_address_end;
        int jump_size = (int)(dec_memory - pos);

        static char internal_jump_details[256];
        if (internal)
            snprintf(internal_jump_details, sizeof(internal_jump_details),
                     " (patch internal by %i bytes)", jump_size);
        else
            internal_jump_details[0] = '\0';

        snprintf(lookupValue, sizeof(lookupValue), "0x%s%s", hex_memory, internal_jump_details);
    } else {
        strncpy(lookupValue, mainInput, sizeof(lookupValue));
        lookupValue[sizeof(lookupValue) - 1] = '\0';
    }

    /* Try table matches */
    for (int i = 0; i < translationTableSize; i++) {
        if (regcomp(&regex, translationTable[i].pattern, REG_EXTENDED | REG_ICASE) != 0) {
            printf("Failed to compile regex for pattern: %s\n", translationTable[i].pattern);
            continue;
        }

        result = regexec(&regex, lookupValue, 0, NULL, 0);
        regfree(&regex);

        if (result == 0) {
            const char* template = translationTable[i].description;

            /* Decide whether this entry is an instruction (pattern ^[A-Za-z]+$) */
            bool is_instruction = false;
            {
                const char* pat = translationTable[i].pattern;
                if (*pat == '^') pat++;
                is_instruction = true;
                for (; *pat && *pat != '$'; ++pat) {
                    if (!isalpha((unsigned char)*pat)) { is_instruction = false; break; }
                }
            }

            /* Build replacement list:
             * - instructions: use operands in order (supports multiple operands)
             * - non-instructions: replace placeholders with the looked-up address/name itself
             */
            const char* repl_buf[8];
            size_t repl_cnt = 0;

            if (is_instruction) {
                if (operand_count == 0) {
                    repl_buf[0] = "";
                    repl_cnt = 1;
                } else {
                    for (size_t r = 0; r < operand_count && r < 8; ++r) {
                        repl_buf[r] = operands[r];
                    }
                    repl_cnt = operand_count;
                }
            } else {
                repl_buf[0] = lookupValue;
                repl_cnt = 1;
            }

            process_template_multi(template, repl_buf, repl_cnt, tempExplanation, sizeof(tempExplanation));
            strncpy(explanation, tempExplanation, sizeof(explanation));
            explanation[sizeof(explanation) - 1] = '\0';
            return explanation;
        }
    }

    /* Fallback: use provided translationTemplate with the main token as single replacement */
    {
        const char* repl_buf[1] = { lookupValue };
        process_template_multi(translationTemplate, repl_buf, 1, tempExplanation, sizeof(tempExplanation));
        strncpy(explanation, tempExplanation, sizeof(explanation));
        explanation[sizeof(explanation) - 1] = '\0';
        return explanation;
    }
}
