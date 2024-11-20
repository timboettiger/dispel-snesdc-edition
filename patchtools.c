#include <stdio.h>
#include "dispel.h"

int handleDeadCodePatch(FILE **fout, unsigned char **data, unsigned long len, unsigned long *rpos, unsigned long *pos) {
    // Check if there's enough data for a dead code patch header
    if (*rpos + 7 >= len) {
        return 0; // Not enough data, no patch handled
    }

    // Check for dead code patch marker
    if ((*data)[*rpos] == 0xDE && (*data)[*rpos + 1] == 0xAD &&
        (*data)[*rpos + 2] == 0xC0 && (*data)[*rpos + 3] == 0xDE) {

        // Extract address and length from patch data
        unsigned long address = ((unsigned long)(*data)[*rpos + 4] << 16) |
                                ((unsigned long)(*data)[*rpos + 5] << 8) |
                                (unsigned long)(*data)[*rpos + 6];
        unsigned char length = (*data)[*rpos + 7];
        unsigned long actual_bytes = len - *rpos - 8;
        unsigned long max_bytes = length ? (length * 4) : 0;

        // Output patch information if not in silent mode
        if (!flagged(SILENT)) {
            fprintf(*fout, "; Action Replay dead code patches %lu bytes from 0x%06lX:\n", length == 0 ? actual_bytes : max_bytes, address);
            if (length == 0) {
                fprintf(*fout, "; No length defined. Dynamic mode.\n");
            }
            if (max_bytes && (actual_bytes != max_bytes)) {
                fprintf(*fout, "; Warning: Patch length (%lu bytes) mismatch. Allowed length: %lu bytes.\n",
                       actual_bytes, max_bytes);
            }
        }

        // Advance read position and set output position
        *rpos = *rpos + 8;  // Skip patch header
        *pos = address;

        return 1; // Patch handled
    }
    return 0; // No patch handled
}
