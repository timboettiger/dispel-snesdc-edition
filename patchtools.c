/* disassemler.c
 * disassembler main loop
 * Created 230900 by James Churchill
 * Updated 190924 by Tim Böttiger
 * Last Modified 081025
 */

#include <stdio.h>
#include "dispel.h"

unsigned long patch_address_start = 0;
unsigned long patch_address_end = 0;

int handleDeadCodePatch(FILE **fout, unsigned char **data, unsigned long len, unsigned long *rpos, unsigned long *pos) {
    // Check if there's enough data for a DEADC0DE patch header
    if (*rpos + 7 >= len) {
        return 0; // Not enough data, no patch handled
    }

    // Check for DEADC0DE patch marker
    if ((*data)[*rpos] == 0xDE && (*data)[*rpos + 1] == 0xAD &&
        (*data)[*rpos + 2] == 0xC0 && (*data)[*rpos + 3] == 0xDE) {

        // Extract address and length from patch data
        unsigned long address = ((unsigned long)(*data)[*rpos + 4] << 16) |
                                ((unsigned long)(*data)[*rpos + 5] << 8) |
                                (unsigned long)(*data)[*rpos + 6];
        unsigned char length = (*data)[*rpos + 7];
        unsigned long actual_bytes = len - *rpos - 8;
        unsigned long max_bytes = length ? (length * 4) : 0;

        fprintf(*fout, "; Action Replay DEADC0DE Cheat\n");
        fprintf(*fout, ";   - Address-Hook: 0x%06lX\n", address);
        if (length == 0) {
            fprintf(*fout, ";   - Length: Dynamic (until EOF)\n");
        } else {
            fprintf(*fout, ";   - Length: %u DWORDs (%u bytes)\n", length, length * 4);
        }
        fprintf(*fout, ";\n");

        patch_address_start = address;
        patch_address_end = address + actual_bytes;

        // Output patch information if not in silent mode
        if (!flagged(SILENT)) {
            if (max_bytes && (actual_bytes != max_bytes)) {
                fprintf(*fout, "; WARNING! Patch length mismatch: expected %zu bytes, got %zu.\n; The cheat may be truncated, or its length byte may be incorrect.\n;\n", actual_bytes, max_bytes);
            }
        }

        // Advance read position and set output position
        *rpos = *rpos + 8;  // Skip patch header
        *pos = address;

        return 1; // Patch handled
    }
    return 0; // No patch handled
}
