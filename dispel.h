/* dispel.h
 * Header file for DisPel
 * James Churchill
 * Created 240900
 */
 #include <ctype.h>
 #include <stdio.h>

 static const char *version = "1.1.0";

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
     AR_PATCH_MODE = 16
 };

 typedef struct {
     int width;
     const char* text;
 } column;

 typedef struct {
     const char* pattern;
     const char* description;
 } TranslationEntry;

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
     ParamOption *valid_values;
 } FlagParam;

 // main.c
 int patch_mode;

 // 65816.c
 int disasm(unsigned char *mem, unsigned long pos, unsigned char *flag, char *inst, unsigned char tsrc);

 // describe.c
 const char* describe(const char* input, const char *translationTemplate);

 // helper.c
 char* trim(const char* str);
 char* format(const char* format, ...);
 char* table(int column_count, ...);
 char* tab(const char* text, int tab_count);
 void process_template(const char* descriptionTemplate, const char* replacement, char* output, size_t output_size);
 void extract_placeholder(const char *descriptionTemplate, char *placeholder);
 void convertToHexFormat(const char* address, char* convertedAddress);
 void convertToDecFormat(const char* number, char* convertedNumber);
 void convertToFlagFormat(const char* number, char* convertedNumber);
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
 int validate_value(int num_options, ParamOption *valid_values, const char *value);
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
 void disassemble(FILE **fout, unsigned char **data, unsigned long len, unsigned long start, unsigned long end, unsigned long pos, unsigned char hirom, unsigned char shadow, unsigned int dwidth, unsigned char tsrc);
