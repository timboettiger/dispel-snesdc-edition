#include <string.h>
#include <regex.h>
#include <stdio.h>

#include "dispel.h"

TranslationEntry translationTable[] = {
    {"^0x00F0$", "SPC700 Register - Timer 0 Target"},
    {"^0x00F1$", "SPC700 Register - Timer 1 Target"},
    {"^0x00F2$", "SPC700 Register - Timer 2 Target"},
    {"^0x00F3$", "SPC700 Register - Timer Control"},
    {"^0x00F4$", "SPC700 Register - DSP Address"},
    {"^0x00F5$", "SPC700 Register - DSP Data"},
    {"^0x00F6$", "SPC700 Register - CPU I/O Port 0"},
    {"^0x00F7$", "SPC700 Register - CPU I/O Port 1"},
    {"^0x00F8$", "SPC700 Register - CPU I/O Port 2"},
    {"^0x00F9$", "SPC700 Register - CPU I/O Port 3"},
    {"^0x00FA$", "SPC700 Register - APU I/O Port 0"},
    {"^0x00FB$", "SPC700 Register - APU I/O Port 1"},
    {"^0x00FC$", "SPC700 Register - APU I/O Port 2"},
    {"^0x00FD$", "SPC700 Register - APU I/O Port 3"},
    {"^0x00FE$", "SPC700 Register - Test Register"},
    {"^0x00FF$", "SPC700 Register - Control Register"},


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
    {"^0x210B$", "BG12NBA - BG1/BG2 Character Data Area Designation"},
    {"^0x210C$", "BG34NBA - BG3/BG4 Character Data Area Designation"},
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

    {"^0x4016$", "JOYSER0 - Joypad Port 1"},
    {"^0x4017$", "JOYSER1 - Joypad Port 2"},

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

    {"^0x4800$", "EXPANSION - Expansion Port Register 0"},
    {"^0x4801$", "EXPANSION - Expansion Port Register 1"},
    {"^0x4802$", "EXPANSION - Expansion Port Register 2"},
    {"^0x4803$", "EXPANSION - Expansion Port Register 3"},
    {"^0x4804$", "EXPANSION - Expansion Port Register 4"},
    {"^0x4805$", "EXPANSION - Expansion Port Register 5"},
    {"^0x4806$", "EXPANSION - Expansion Port Register 6"},
    {"^0x4807$", "EXPANSION - Expansion Port Register 7"},

    {"^0x004211$", "RDIO - Programmable I/O Port (Read)"},
    {"^0x00EAEA$", "Address in LoROM typically used for infinite loops"},
    {"^0x80EAEA$", "Address in HiROM or mirrored LoROM, potentially infinite loop"},

    {"^0xFFFC$", "NMI Vector (Emulation Mode)"},
    {"^0xFFFE$", "Reset Vector (Emulation Mode)"},
    {"^0xFFFA$", "IRQ/BRK Vector (Emulation Mode)"},
    {"^0xFFF[8-9A-Fa-f]$", "Interrupt Vector (Emulation Mode, at @s)"},
    {"^0xFFE[0-7]$", "Interrupt Vector (Native Mode, at @s)"},
    {"^0xFF[CF][0-9A-Fa-f]{2}$", "Interrupt Vectors (Native Mode, at @s)"},

    {"^0x00[0-9A-Fa-f]{2}$", "SPC700 RAM (Zero Page and Stack) (at @s)"},
    {"^0x01[0-9A-Fa-f]{2}$", "SPC700 RAM (Direct Page) (at @s)"},
    {"^0x0[2-9A-Fa-f][0-9A-Fa-f]{2}$", "SPC700 RAM (Work RAM) (at @s)"},
    {"^0x[0-6][0-9A-Fa-f]{3}$", "SPC700 RAM (Mirror) (at @s)"},
    {"^0x7F[0-9A-Fa-f]{3}$", "SPC700 ROM (at @s)"},

    {"^0x22[0-3][0-9A-Fa-f]{2}$", "SA-1 Register (at @s)"},
    {"^0x21[0-3][0-9A-Fa-f]$", "PPU Register (at @s)"},

    {"^0x7E[0-9A-Fa-f]{4}$", "Work RAM (WRAM) (at @s)"},
    {"^0x7F[0-9A-Fa-f]{4}$", "Mirror of WRAM (at @s)"},

    {"^0x00[0-1F][0-9A-Fa-f]{2}$", "System Area (at @s)"},
    {"^0x80[0-1F][0-9A-Fa-f]{2}$", "Mirror of System Area (at @s)"},

    {"^0x00[0-7F][0-9A-Fa-f]{4}$", "LoROM Mapping (at @s)"},
    {"^0x80[0-7F][0-9A-Fa-f]{4}$", "Mirror of LoROM Mapping (at @s)"},

    {"^0x40[0-7F][0-9A-Fa-f]{4}$", "HiROM Mapping (at @s)"},
    {"^0xC0[0-7F][0-9A-Fa-f]{4}$", "Mirror of HiROM Mapping (at @s)"},

    {"^0x[0-3][0-9A-Fa-f]{5}$", "Hardware Registers and I/O Ports (at @s)"},
    {"^0x[8B][0-9A-Fa-f]{5}$", "Mirror of Hardware Registers and I/O Ports (at @s)"},

    {"^0x[4-7][0-9A-Fa-f]{5}$", "Open Bus Area (at @s)"},
    {"^0xFE[0-9A-Fa-f]{4}$", "High ROM Area (at @s)"},
    {"^0x[2-3][0-9A-Fa-f]8000$", "Open Bus or ROM Mirror (at @s)"},
    {"^0x[6-7][0-9A-Fa-f]{5}$", "Unmapped or Reserved Area (at @s)"},
    {"^0x80[0-9A-Fa-f]{5}$", "Open Bus or ROM Mirror (at @s)"},
    {"^0x40[0-9A-Fa-f]{5}$", "Extended ROM (HiROM or LoROM, at @s)"},

    {"^0x[0-3][0-9A-Fa-f]0000$", "Extended RAM or Hardware (at @s)"},
    {"^0x[8B][0-9A-Fa-f]0000$", "Mirror of Extended RAM or Hardware (at @s)"},

    {"^0x43[0-7][0-9A-Fa-f]$", "DMA Register for Channel %d (at @s)"},

    {"^0x00F[0-9A-Fa-f]{2}$", "SPC700 Register (at @s)"},
    {"^0xF0[0-9A-Fa-f]{2}$", "SPC700 Register (at @s)"},

    {"^adc$", "Add with carry to \\@s"},
    {"^and$", "Logical AND with \\@s"},
    {"^asl$", "Arithmetic shift left on \\@s"},
    {"^bcc$", "Branch to \\@s if carry is clear"},
    {"^bcs$", "Branch to \\@s if carry is set"},
    {"^beq$", "Branch to \\@s if equal"},
    {"^bit$", "Test bits in \\@s"},
    {"^bmi$", "Branch to \\@s if minus"},
    {"^bne$", "Branch to \\@s if not equal"},
    {"^bpl$", "Branch to \\@s if positive"},
    {"^bra$", "Unconditional branch to \\@s"},
    {"^brk$", "Force break"},
    {"^brl$", "Branch long to \\@s"},
    {"^bvc$", "Branch to \\@s if overflow is clear"},
    {"^bvs$", "Branch to \\@s if overflow is set"},
    {"^clc$", "Clear carry flag"},
    {"^cld$", "Clear decimal mode flag"},
    {"^cli$", "Clear interrupt disable flag"},
    {"^clv$", "Clear overflow flag"},
    {"^cmp$", "Compare accumulator with \\@s"},
    {"^cop$", "Coprocessor call"},
    {"^cpx$", "Compare X register with \\@s"},
    {"^cpy$", "Compare Y register with \\@s"},
    {"^dec$", "Decrement value at \\@s"},
    {"^dex$", "Decrement X register"},
    {"^dey$", "Decrement Y register"},
    {"^eor$", "Exclusive OR with \\@s"},
    {"^inc$", "Increment value at \\@s"},
    {"^inx$", "Increment X register"},
    {"^iny$", "Increment Y register"},
    {"^jmp$", "Jump to \\@s"},
    {"^jml$", "Long jump to \\@s"},
    {"^jsr$", "Jump to subroutine at \\@s"},
    {"^jsl$", "Long jump to subroutine at \\@s"},
    {"^lda$", "Load accumulator with value from \\@s"},
    {"^ldx$", "Load X register with \\@s"},
    {"^ldy$", "Load Y register with \\@s"},
    {"^lsr$", "Logical shift right on \\@s"},
    {"^mvn$", "Block move negative from source \\@s to destination @s"},
    {"^mvp$", "Block move positive from source \\@s to destination @s"},
    {"^nop$", "No operation"},
    {"^ora$", "Logical OR with \\@s"},
    {"^pea$", "Push effective address \\@s onto stack"},
    {"^pei$", "Push indirect address \\@s onto stack"},
    {"^per$", "Push program counter relative address \\@s onto stack"},
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
    {"^rep$", "Reset processor flags to \\@b"},
    {"^rol$", "Rotate left on \\@s"},
    {"^ror$", "Rotate right on \\@s"},
    {"^rti$", "Return from interrupt"},
    {"^rtl$", "Long return from subroutine"},
    {"^rts$", "Return from subroutine"},
    {"^sbc$", "Subtract with carry from \\@s"},
    {"^sec$", "Set carry flag"},
    {"^sed$", "Set decimal mode flag"},
    {"^sei$", "Set interrupt disable flag"},
    {"^sep$", "Set processor flags to \\@b"},
    {"^sta$", "Store accumulator at \\@s"},
    {"^stp$", "Stop processor"},
    {"^stx$", "Store X register at \\@s"},
    {"^sty$", "Store Y register at \\@s"},
    {"^stz$", "Store zero at \\@s"},
    {"^tax$", "Transfer accumulator to X register"},
    {"^tay$", "Transfer accumulator to Y register"},
    {"^tcd$", "Transfer accumulator to direct page register"},
    {"^tcs$", "Transfer accumulator to stack pointer"},
    {"^tdc$", "Transfer direct page register to accumulator"},
    {"^trb$", "Test and reset bits at \\@s"},
    {"^tsb$", "Test and set bits at \\@s"},
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

const char* describe(const char* input, const char *translationTemplate) {
    regex_t regex;
    int result;
    static char lookupValue[100];
    static char explanation[200]; // Increased size to accommodate additional text
    static char tempExplanation[200];
    char mainInput[100];
    char offsetPart[100];
    char tempInput[100];

    // Copy input to tempInput for manipulation
    strncpy(tempInput, input, sizeof(tempInput));
    tempInput[sizeof(tempInput) - 1] = '\0'; // Ensure null termination

    // Initialize offsetPart
    offsetPart[0] = '\0';

    // Check for comma
    char* commaPtr = strchr(tempInput, ',');
    if (commaPtr != NULL) {
        // Split at comma
        *commaPtr = '\0'; // Terminate mainInput at comma
        strncpy(mainInput, tempInput, sizeof(mainInput));
        mainInput[sizeof(mainInput) - 1] = '\0';
        // Store everything after comma
        strncpy(offsetPart, commaPtr + 1, sizeof(offsetPart));
        offsetPart[sizeof(offsetPart) - 1] = '\0';
    } else {
        // No comma, use input as is
        strncpy(mainInput, tempInput, sizeof(mainInput));
        mainInput[sizeof(mainInput) - 1] = '\0';
    }

    // Proceed to translate mainInput
    if (mainInput[0] == '$') {
        snprintf(lookupValue, sizeof(lookupValue), "0x%s", mainInput + 1);
    } else {
        strncpy(lookupValue, mainInput, sizeof(lookupValue));
        lookupValue[sizeof(lookupValue) - 1] = '\0';
    }

    for (int i = 0; i < translationTableSize; i++) {
        if (regcomp(&regex, translationTable[i].pattern, REG_EXTENDED) != 0) {
            printf("Failed to compile regex for pattern: %s\n", translationTable[i].pattern);
            continue;
        }

        result = regexec(&regex, lookupValue, 0, NULL, 0);
        regfree(&regex);

        if (result == 0) {
            const char* template = translationTable[i].description;
            // Now, if offsetPart is not empty, append explanation
            if (offsetPart[0] != '\0') {
                static char extendedLookup[200];
                char* offsetTrimmed = offsetPart;
                while (isspace((unsigned char)*offsetTrimmed)) {
                    offsetTrimmed++;
                }
                snprintf(extendedLookup, sizeof(extendedLookup), "%s with value of '%s' added", lookupValue, offsetTrimmed);

                process_template(template, extendedLookup, tempExplanation, sizeof(tempExplanation));
            } else {
                process_template(template, lookupValue, tempExplanation, sizeof(tempExplanation));
            }
            strncpy(explanation, tempExplanation, sizeof(explanation));
            explanation[sizeof(explanation) - 1] = '\0';
            return explanation;
        }
    }

    process_template(translationTemplate, lookupValue, tempExplanation, sizeof(tempExplanation));
    strncpy(explanation, tempExplanation, sizeof(explanation));
    explanation[sizeof(explanation) - 1] = '\0';
    return explanation;
}
