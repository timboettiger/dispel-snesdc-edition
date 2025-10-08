/* helper.c
 * helper tool functions
 * Created 230900 by James Churchill
 * Updated 190924 by Tim Böttiger
 * Last Modified 081025
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include "dispel.h"

char* trim(const char* str) {
    const char* start = str;
    const char* end;
    while (isspace((unsigned char)*start)) {
        start++;
    }
    if (*start == '\0') {
        return strdup("");
    }
    end = start + strlen(start) - 1;
    while (end > start && isspace((unsigned char)*end)) {
        end--;
    }
    size_t length = end - start + 1;
    char* trimmed = malloc(length + 1);
    if (trimmed == NULL) {
        return NULL;
    }
    strncpy(trimmed, start, length);
    trimmed[length] = '\0';
    return trimmed;
}

char* format(const char* format, ...) {
    va_list args;

    va_start(args, format);
    int size = vsnprintf(NULL, 0, format, args);
    va_end(args);

    if (size < 0) {
        return NULL;
    }
    char* result = malloc(size + 1);
    if (result == NULL) {
        return NULL;
    }
    va_start(args, format);
    vsnprintf(result, size + 1, format, args);
    va_end(args);
    return result;
}

char* tab(const char* text, int tab_count) {
    int text_len = strlen(text);
    int tab_width = 8; // Breite eines Tabs in Zeichen
    int total_tab_width = tab_count * tab_width;
    int padding_needed = (total_tab_width - text_len) / tab_width;
    if (text_len % tab_width == 0) padding_needed -= 1;
    int total_len = text_len + padding_needed + 1;
    char* result = malloc(total_len);
    if (result == NULL) return NULL;
    strcpy(result, text);
    for (int i = 0; i < padding_needed; i++) strcat(result, "\t");
    return result;
}

char* table(int column_count, ...) {
    va_list args;
    char* result = malloc(1024);
    if (result == NULL) {
        return NULL;
    }
    result[0] = '\0';

    va_start(args, column_count);
    for (int i = 0; i < column_count; i++) {
        column col = va_arg(args, column);
        const char* text = trim(col.text);

        if (col.width == 0) {
            strcat(result, text);
        } else {
            char *temp = tab(text, col.width);
            strcat(result, temp);
        }
    }
    va_end(args);

    return result;
}

int hexdump(unsigned char **data,unsigned long pos,unsigned long rpos, unsigned long len,char *inst, unsigned char dwidth)
{
	int i;
    sprintf(inst, "%02lX/%04lX:\t", (pos >> 16) & 0xFF, pos & 0xFFFF);
    for(i=0; i<dwidth && i+rpos<len; i++)
    {
    	sprintf(inst + i*2 + 9, "%02X", (*data)[rpos+i]);
    }
    return dwidth;
}

int AllASCII(unsigned char *b, int size)
{
	int i;
	for (i = 0; i < size; i++)
	   if (b[i] < 32 || b[i] > 126) return 0;
	return 1;
}

unsigned long getFileLength(FILE **fin) {
    #if defined(__APPLE__) || defined(__linux__)
        fseek(*fin, 0L, SEEK_END);
        unsigned long len = ftell(*fin);
        fseek(*fin, 0L, SEEK_SET);
    #else
        unsigned long len = filelength(fileno(*fin));
    #endif
    return len;
}

unsigned char *allocateMemory(unsigned long len) {
    unsigned char *data = malloc(len + 3);
    if (!data) {
        printf("Cannot allocate memory.\n");
        exit(1);
    }
    return data;
}
