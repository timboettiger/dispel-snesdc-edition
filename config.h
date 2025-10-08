
#ifndef CONFIG_H
#define CONFIG_H

#include <ctype.h>
#include <stdio.h>

enum Parameter {
    INPUT_FILE = -1,
    SKIP_HEADER = 0,
    VERBOSE = 2,
    SILENT = 3,
    FORMAT = 4,
    HIGHROM = 5,
    LOWROM = 6,
    A8BIT = 7,
    XY8BIT = 8,
    DISABLE_BOUNDARY = 9,
    SPLIT_ROUTINES = 10,
    HEXDUMP_WIDTH = 11,
    OUTPUT_FILE = 15,
    AR_PATCH_MODE = 16,
    AR_AUTO_8BIT = 17,
};

typedef struct {
    const char *name;
    const char *help_text;
} ParamOption;

typedef struct {
    const char *name;
    int required;
    int hide;
    const char *value_default;
    const char *value_name;
    const char *help_text;
    int num_options;
    const ParamOption *valid_values;
} FlagParam;

extern const char *version;

extern const ParamOption format_values[];
extern const ParamOption output_channels[];

extern FlagParam skip_header;
extern FlagParam little_output;
extern FlagParam verbose;
extern FlagParam silent;
extern FlagParam output_format;
extern FlagParam force_high_rom;
extern FlagParam force_low_rom;
extern FlagParam accumulator_8bit;
extern FlagParam xy_8bit;
extern FlagParam disable_bank_boundary;
extern FlagParam split_subroutines;
extern FlagParam produce_hexdump;
extern FlagParam force_shadow_bank;
extern FlagParam force_shadow_region;
extern FlagParam rewrite_origin_addresses;
extern FlagParam output_channel;
extern FlagParam ar_patch_mode;
extern FlagParam ar_auto_8bit;

#endif // CONFIG_H
