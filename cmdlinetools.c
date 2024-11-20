#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>

#include "dispel.h"

#define MAX_FLAGS 32

static FlagParam flag_params[MAX_FLAGS] = {0};
static unsigned int flags = 0;
static const char *flag_values[MAX_FLAGS] = {NULL};
static const char *required_file = NULL;
static const char *required_file_value_name = NULL;
static const char *required_file_help_text = NULL;
static const char *program_binary = NULL;
static const char *program_name = NULL;
static const char *program_subtitle = NULL;
static const char *program_version = NULL;
static const char *program_copyright = NULL;

void setparam(int count, ...) {
    va_list args;
    va_start(args, count);
    for (int i = 0; i < count && i < MAX_FLAGS; i++) {
        FlagParam *param = va_arg(args, FlagParam *);
        flag_params[i] = *param;
        if (!flag_values[i] && param->value_default) {
            flag_values[i] = param->value_default;
        }
    }
    va_end(args);
}

void set_required_file(const char *value_name, const char *help_text) {
    required_file_value_name = value_name;
    required_file_help_text = help_text;
}

void set_program_name(const char *binary, const char *name, const char *subtitle) {
    program_binary = binary;
    program_name = name;
    program_subtitle = subtitle;
}

void set_version(const char *version) {
    program_version = version;
}

void set_copyright(const char *copyright) {
    program_copyright = copyright;
}

int validate_value(int num_options, ParamOption *valid_values, const char *value) {
    if (!valid_values || num_options <= 0 || !value) {
        return 0;
    }
    for (int i = 0; i < num_options; i++) {
        if (strcmp(valid_values[i].name, value) == 0) {
            return 1;
        }
    }
    return 0;
}

void set_flag(const char *name, const char *value) {
    for (int i = 0; i < MAX_FLAGS; i++) {
        if (flag_params[i].name != NULL && strcmp(flag_params[i].name, name) == 0) {
            if (value && !flag_params[i].valid_values) {
                printf("Error: Parameter -%s does not accept values.\n", name);
                exit(1);
            }
            if (!value && flag_params[i].valid_values) {
                printf("Error: Missing option for parameter -%s.\nValid options: ", name);
                for (int p = 0; p < flag_params[i].num_options; p++) {
                    printf("\n  - %s\t%s", flag_params[i].valid_values[p].name, flag_params[i].valid_values[p].help_text);
                }
                printf("\n");
                exit(1);
            }
            else if ((!value && flag_params[i].valid_values) || (flag_params[i].required == 1 && !validate_value(flag_params[i].num_options, flag_params[i].valid_values, value))) {
                printf("Error: Invalid option '%s' for parameter -%s.\nValid options: ", value, name);
                for (int p = 0; p < flag_params[i].num_options; p++) {
                    printf("\n  - %s\t%s", flag_params[i].valid_values[p].name, flag_params[i].valid_values[p].help_text);
                }
                printf("\n");
                exit(1);
            }
            flag_values[i] = value;
            flags |= (1 << i);
            return;
        }
    }
    printf("Error: Unknown parameter -%s.\n", name);
    exit(1);
}

void flag(int flag_id) {
    flags |= (1 << flag_id);
}

void unflag(int flag_id) {
    flags &= ~(1 << flag_id);
}

int flagged(int flag_id) {
    return (flags & (1 << flag_id)) != 0;
}

const char *option(int flag_id) {
    return flag_id == -1 ? required_file : flag_values[flag_id];
}

int file_required(void) {
    return required_file_help_text != NULL;
}

void print_version(void) {
    if (program_name) {
        printf("%s", program_name);
        if (program_version) {
            printf(" %s", program_version);
        }
        printf("\n");
        if (program_subtitle) {
            printf("%s\n", program_subtitle);
        }
    }
    if (program_copyright) {
        printf("%s\n", program_copyright);
    }
}

void print_help(void) {
    print_version();
    printf("\nUsage: %s [options] %s\n", program_binary, required_file_value_name != NULL ? required_file_value_name : "");
    printf("Parameters:\n");
    for (int i = 0; i < MAX_FLAGS && flag_params[i].name != NULL; i++) {
        if (!flag_params[i].hide) {
            printf("  -%s%s\t%s (%s",
                   flag_params[i].name,
                   flag_params[i].value_name ? flag_params[i].value_name : "\t",
                   flag_params[i].help_text,
                   flag_params[i].required ? "required" : "optional");
            if (flag_params[i].value_default) {
                printf(", default: '%s'", flag_params[i].value_default);
            }
            printf(").\n");
            if (flag_params[i].valid_values && flag_params[i].num_options > 0) {
                printf("\t\tOptions:");
                for (int p = 0; p < flag_params[i].num_options; p++) {
                    printf("\n\t\t  - %s\t%s", flag_params[i].valid_values[p].name, flag_params[i].valid_values[p].help_text);
                }
                printf("\n");
            }
        }
    }
    printf("\n");
    if (required_file_help_text) {
        printf("  %s\t%s (required).\n\n", required_file_value_name, required_file_help_text);
    }
}

char *remove_quotes(const char *value) {
    int len = strlen(value);
    if ((value[0] == '"' && value[len - 1] == '"') || (value[0] == '\'' && value[len - 1] == '\'')) {
        char *unquoted = malloc(len - 1);
        strncpy(unquoted, value + 1, len - 2);
        unquoted[len - 2] = '\0';
        return unquoted;
    }
    return strdup(value);
}

void process_args(int argc, char *argv[]) {
    int positional_found = 0;
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            char *name = argv[i] + 1;
            char *value = NULL;

            int no_parameter_following = ((i + 1) < argc) && (argv[i + 1][0] != '-');
            int no_string_following = ((i + 1) < argc) && (argv[i + 1][0] != '"' || argv[i + 1][0] != '\'');
            int is_penultimate = ((i + 1) == (argc - 1));
            int no_file_following = (!file_required() && !is_penultimate);
            if (strchr(name, '=') != NULL) {
                value = strchr(name, '=') + 1;
                *strchr(name, '=') = '\0';
            } else if (no_parameter_following && no_string_following && no_file_following) {
                value = argv[++i];
            }
            if (value) {
                value = remove_quotes(value);
            }
            set_flag(name, value);
        } else {
            if (positional_found) {
                printf("Error: Too many positional arguments.\n");
                exit(1);
            }
            required_file = argv[i];
            positional_found = 1;
        }
    }
    for (int i = 0; i < MAX_FLAGS && flag_params[i].name != NULL; i++) {
        if (flag_params[i].required == 1 && !flagged(i)) {
            printf("Error: Required flag -%s is missing.\n", flag_params[i].name);
            print_help();
            exit(1);
        }
    }
    if (required_file_help_text && !required_file) {
        printf("Error: Missing required file argument.\n");
        print_help();
        exit(1);
    }
}
