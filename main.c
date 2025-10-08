/* main.c
 * DisPel 65816 Disassembler
 * Created 240900 by James Churchill
 * Updated 190924 by Tim Böttiger
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __APPLE__
#include <sys/uio.h>
#else
#ifdef __linux__
#include <sys/io.h>
#else
#include <io.h>
#endif
#endif

#include "config.h"
#include "dispel.h"

int main(int argc, char *argv[]) {
    set_program_name("dispel", "DisPel", "DEADC0DE Edition");
    set_version(version);
    set_copyright("Copyright (c) 2001 James Churchill\nCopyright (c) 2024-2025 Tim Böttiger");

    setparam(16, &skip_header, &little_output, &verbose, &silent, &output_format, &force_high_rom, &force_low_rom, &accumulator_8bit, &xy_8bit, &disable_bank_boundary, &split_subroutines, &produce_hexdump, &force_shadow_bank, &force_shadow_region, &rewrite_origin_addresses, &output_channel, &ar_patch_mode);
    set_required_file("<inputfile>", "File to disassemble");

    if (argc == 1) {
        print_help();
        return 0;
    }
    process_args(argc, argv);
    return process_file();
}
