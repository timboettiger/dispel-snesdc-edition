/* dispel.h
 * Header file for DisPel
 * Created 240900 by James Churchill
 */

#ifndef DISPEL_H
#define DISPEL_H

#include "config.h"

typedef struct {
    int width;
    const char* text;
} column;

typedef struct {
    const char* pattern;
    const char* description;
} TranslationEntry;

// 65816.c
int disasm(unsigned char *mem, unsigned long pos, unsigned char *flag, char *inst, unsigned char tsrc);

// describe.c
extern unsigned long patch_address_start;
extern unsigned long patch_address_end;
void process_template_multi(const char* tpl, const char** replacements, size_t repl_count, char* output,
 size_t out_size);
//void process_template(const char* tpl, const char* replacement, const char* currentBankHex2, char* output, size_t out_size);
void extract_placeholder(const char *descriptionTemplate, char *placeholder);
void convertToHexFormat(const char* address, char* convertedAddress);
void convertToDecFormat(const char* number, char* convertedNumber);
void convertToFlagFormat(const char* number, char* convertedNumber);
const char* describe(unsigned long pos, const char* input, const char *translationTemplate);

// helper.c
char* trim(const char* str);
char* format(const char* format, ...);
char* table(int column_count, ...);
char* tab(const char* text, int tab_count);
int hexdump(unsigned char **data, unsigned long pos, unsigned long rpos, unsigned long len, char *inst, unsigned char dwidth);
int AllASCII(unsigned char *b, int size);
unsigned long getFileLength(FILE **fin);
unsigned char *allocateMemory(unsigned long len);

//cmdline.c
void setparam(int count, ...);
void set_required_file(const char *value_name, const char *help_text);
void set_program_name(const char *binary, const char *name, const char *subtitle);
void set_version(const char *version);
void set_copyright(const char *copyright);
int validate_value(int num_options, const ParamOption *valid_values, const char *value);
void set_flag(const char *name, const char *value);
int flagged(int flag_id);
const char *option(int flag_id);
void flag(int flag_id);
void unflag(int flag_id);
void print_help(void);
char *remove_quotes(const char *value);
void process_args(int argc, char *argv[]);

//romtools.c
unsigned long ConvertToFastROM(unsigned long address);
unsigned long ConvertToSlowROM(unsigned long address);
int ScoreHiROM(unsigned char **data);
int ScoreLoROM(unsigned char **data);
void autoDetectHiLoROM(unsigned char **data, unsigned char *hirom);
void calculateAddressOptions(unsigned char **data, unsigned long *start, unsigned long *end, unsigned long *pos, unsigned long origin, unsigned char *hirom, unsigned char *shadow, unsigned int *bank, unsigned long len);

// patchtools.c
int handleDeadCodePatch(FILE **fout, unsigned char **data, unsigned long len, unsigned long *rpos, unsigned long *pos);

// disassembler.c
int process_file(void);
void disassemble(FILE **fout, unsigned char *flag, unsigned char **data, unsigned long len, unsigned long start, unsigned long end, unsigned long pos, unsigned char hirom, unsigned char shadow, unsigned int dwidth, unsigned char tsrc);

#endif
