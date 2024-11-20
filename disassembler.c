#include <stdio.h>
#include <memory.h>
#include <stdlib.h>

#include "dispel.h"

unsigned long openFiles(const char *infile, const char *outfile, FILE **fin, FILE **fout) {
    *fin = fopen(infile, "rb");
    if (!*fin) {
        if (!flagged(SILENT))
            printf("Cannot open %s for reading.\n", infile);
        exit(1);
    }

    if (strcmp(outfile, "stdout") == 0) {
        *fout = stdout;
    } else {
        *fout = fopen(outfile, "w");
        if (!*fout) {
            if (!flagged(SILENT))
                printf("Cannot open %s for writing.\n", outfile);
            exit(1);
        }
    }
    return getFileLength(fin);
}

void load_data(FILE **fin, unsigned long len, unsigned char **data) {
    fread(*data, len, 1, *fin);
    fclose(*fin);
}

int close_files(FILE **fout, unsigned char **data) {
    fclose(*fout);
    free(*data);
    return 0;
}

int process_file(void)
{
    unsigned long pos = 0, origin = 0x1000000, start = 0, end = 0;
    unsigned char hirom = 2, shadow = 2, tsrc = 0;
    unsigned int dwidth = 0, bank = 0x100;
    FILE *fin, *fout;

    unsigned long len = openFiles(option(INPUT_FILE), option(OUTPUT_FILE), &fin, &fout);

    if (len <= 392) {
        if (flagged(VERBOSE) && !flagged(SILENT))
            printf("File size <= 392 bytes. Activating 'Action Replay Patch Mode'.\n");

        flag(AR_PATCH_MODE);
    }
    else if (len < 0x8000 || (flagged(SKIP_HEADER) && len < 0x8200)) {
        if (!flagged(SILENT))
            printf("This file looks too small to be a legitimate ROM image.\n");
    }

    unsigned char *data = allocateMemory(len);
    load_data(&fin, len, &data);

    autoDetectHiLoROM(&data, &hirom);
    calculateAddressOptions(&data, &start, &end, &pos, origin, &hirom, &shadow, &bank, len);

    dwidth = strtol(option(HEXDUMP_WIDTH), NULL, 10);
    disassemble(&fout, &data, len, start, end, pos, hirom, shadow, dwidth, tsrc);
    return 0; //close_files(&fout, &data);
}

void disassemble(FILE **fout, unsigned char **data, unsigned long len, unsigned long start, unsigned long end, unsigned long pos, unsigned char hirom, unsigned char shadow, unsigned int dwidth, unsigned char tsrc) {
    unsigned char dmem[4];
    unsigned long rpos = start;
    char inst[521];
    unsigned char flag = 0;

    int output_hexdump = (strcmp(option(FORMAT), "hexdump") == 0);
    while (rpos < len && rpos <= end) {
        // Handle dead code patch if applicable
        if (pos == 0 && !output_hexdump && flagged(AR_PATCH_MODE) && handleDeadCodePatch(data, len, &rpos, &pos)) {
            continue; // Skip to next iteration if a patch was handled
        }

        // Copy the next 4 bytes into the staging area
        memcpy(dmem, *data + rpos, 4);

        // Disassemble or generate a hexdump
        unsigned int offset;
        if (output_hexdump) {
            offset = hexdump(data, pos, rpos, len, inst, dwidth);
        } else {
            offset = disasm(dmem, pos, &flag, inst, tsrc);
        }

        // Check for file/block overruns
        if ((rpos + offset) > len || (rpos + offset) > (end + 1)) {
            fprintf(*fout, "%02lX/%04lX:\t", (pos >> 16) & 0xFF, pos & 0xFFFF);
            for (unsigned long i = rpos; i < len && i <= end; i++) {
                fprintf(*fout, "%02X", (*data)[i]);
            }
            fprintf(*fout, "\n");
            break;
        }

        // Check for bank overruns
        if (((pos & 0xFFFF) + offset) > 0x10000) {
            unsigned int tmp = 0x10000 - (pos & 0xFFFF);
            fprintf(*fout, "%02lX/%04lX:\t", (pos >> 16) & 0xFF, pos & 0xFFFF);
            for (unsigned int i = 0; i < tmp; i++) {
                fprintf(*fout, "%02X", (*data)[rpos + i]);
            }
            fprintf(*fout, "\n");

            // Move to the next bank
            if (!hirom) {
                pos = (pos & 0xFF0000) + 0x18000;
            } else {
                pos += tmp;
            }
            rpos += tmp;
            continue;
        }

        // Print the disassembled instruction or hexdump
        fprintf(*fout, "%s\n", inst);

        // Move to the next instruction
        if (!hirom && ((pos & 0xFFFF) + offset) > 0xFFFF) {
            pos = (pos & 0xFF0000) + 0x18000;
        } else {
            pos += offset;
        }
        rpos += offset;
    }
}
