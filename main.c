/* main.c
 * DisPel 65816 Disassembler
 * James Churchill
 * Created 20000924
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

#include "dispel.h"

int main(int argc, char *argv[]) {
    set_program_name("dispel", "DisPel", "DEADC0DE Edition");
    set_version("1.1.0");
    set_copyright("Copyright (c) 2000 James Churchill\nCopyright (c) 2024 Tim Böttiger");

    ParamOption format_values[] = {
        {"standard", "address, hexdump, opcode with operand"},
        {"hexdump", "hexdump only, specify width using -w"},
        {"assembler", "opcode with operand"},
        {"annotated", "address, hexdump, opcode with operand, annotation"}
    };

    ParamOption output_channels[] = {
        {"stdout", "stdout"},
        {"<file>", "Set output <file>"}
    };

    FlagParam skip_header =             {"n", 0,  0, NULL, NULL,                "Skip $200 byte SMC header", 0, NULL};
    FlagParam little_output =           {"t", 0,  1, NULL, NULL,                "Don't output addresses/hex dump", 0, NULL};
    FlagParam verbose =                 {"v", 0,  0, NULL, NULL,                "Enable verbose mode", 0, NULL};
    FlagParam silent =                  {"s", 0,  0, NULL, NULL,                "Enable silent mode", 0, NULL};
    FlagParam output_format =           {"f", 0,  0, "standard", " <format>",   "Output formatting", 4, format_values};
    FlagParam force_high_rom =          {"h", 0,  0, NULL, NULL,                "Force HiROM memory mapping", 0, NULL};
    FlagParam force_low_rom =           {"l", 0,  0, NULL, NULL,                "Force LoROM memory mapping", 0, NULL};
    FlagParam accumulator_8bit =        {"a", 0,  0, NULL, NULL,                "Start in 8-bit accumulator mode", 0, NULL};
    FlagParam xy_8bit =                 {"x", 0,  0, NULL, NULL,                "Start in 8-bit X/Y mode", 0, NULL};
    FlagParam disable_bank_boundary =   {"e", 0,  0, NULL, NULL,                "Disable bank-boundary enforcement", 0, NULL};
    FlagParam split_subroutines =       {"p", 0,  0, NULL, NULL,                "Split subroutines by placing blank lines after RTS, RTL, RTI", 0, NULL};
    FlagParam produce_hexdump =         {"w", 0,  0, "4", " <width>",           "Produce a hexdump with <width> bytes/line", 0, NULL};
    FlagParam force_shadow_bank =       {"b", 0,  1, NULL, " <bank>",           "Handle adresses in <bank> 00 as shadowed", 0, NULL};
    FlagParam force_shadow_region =     {"r", 0,  1, NULL, " <region>",         "Handle <region> $00000-$FFFFF as shadowed", 0, NULL};
    FlagParam rewrite_origin_adresses = {"g", 0,  1, NULL, " <origin>",         "Rewrite adresses to <origin>", 0, NULL};
    FlagParam output_channel =          {"o", -1, 0, "stdout", " [...]",        "Set output channel", 2, output_channels};
    FlagParam ar_patch_mode =           {"0", 0,  1, NULL, NULL,                "Enforce Action Replay Patch Mode", 0, NULL};
    setparam(16, &skip_header, &little_output, &verbose, &silent, &output_format, &force_high_rom, &force_low_rom, &accumulator_8bit, &xy_8bit, &disable_bank_boundary, &split_subroutines, &produce_hexdump, &force_shadow_bank, &force_shadow_region, &rewrite_origin_adresses, &output_channel, &ar_patch_mode);
    set_required_file("<inputfile>", "File to disassemble");

    if (argc == 1) {
        print_help();
        return 0;
    }
    process_args(argc, argv);
    return process_file();
}
