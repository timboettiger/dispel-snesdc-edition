/* dispel.h
 * Header file for DisPel
 * James Churchill
 * Created 240900
 */
 #include <ctype.h>

 typedef struct {
     int width;
     const char* text;
 } column;

 typedef struct {
     const char* pattern;
     const char* description;
 } TranslationEntry;

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
