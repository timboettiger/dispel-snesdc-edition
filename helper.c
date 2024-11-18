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

void convertToHexFormat(const char* address, char* convertedAddress) {
    if (address[0] == '$') {
        sprintf(convertedAddress, "0x%s", address + 1);
    } else {
        strcpy(convertedAddress, address);
    }
}

void convertToDecFormat(const char* number, char* convertedNumber) {
    if (number[0] == '#' && number[1] == '$') {
        int decimalValue = (int)strtol(number + 2, NULL, 16);
        sprintf(convertedNumber, "constant '%d'", decimalValue);
    }
    else {
        strcpy(convertedNumber, number);
    }
}

void convertToFlagFormat(const char* number, char* convertedNumber) {
    if (number[0] == '#' && number[1] == '$') {
        int decimalValue = (int)strtol(number + 2, NULL, 16);
        char binaryString[9] = {0};
        for (int i = 7; i >= 0; i--) {
            binaryString[7 - i] = (decimalValue & (1 << i)) ? '1' : '0';
        }
        sprintf(convertedNumber, "%s (binary representation of '%d')", binaryString, decimalValue);
    } else if (number[0] == '#') {
        int decimalValue = (int)strtol(number + 1, NULL, 10);
        char binaryString[9] = {0};
        for (int i = 7; i >= 0; i--) {
            binaryString[7 - i] = (decimalValue & (1 << i)) ? '1' : '0';
        }
        sprintf(convertedNumber, "%s (binary representation of '%d')", binaryString, decimalValue);
    } else {
        strcpy(convertedNumber, number);
    }
}

void process_template(const char* descriptionTemplate, const char* replacement, char* output, size_t output_size) {
    size_t i = 0, j = 0;
    bool escaping = false;
    bool replaced = false;
    char temp[256] = {0};

    while (descriptionTemplate[i] != '\0' && j < output_size - 1) {
        if (descriptionTemplate[i] == '\\' && !escaping) {
            escaping = true;
            i++;
            continue;
        } else if (descriptionTemplate[i] == '@' && descriptionTemplate[i + 1] == 's' && !escaping && !replaced) {
            static char replacementValue[200];
            convertToDecFormat(replacement, replacementValue);
            strncat(temp, replacementValue, sizeof(temp) - strlen(temp) - 1);
            j = strlen(temp);
            i += 2;
            replaced = true;
            continue;
        } else if (descriptionTemplate[i] == '@' && descriptionTemplate[i + 1] == 'b' && !escaping && !replaced) {
            static char replacementValue[200];
            convertToFlagFormat(replacement, replacementValue);
            strncat(temp, replacementValue, sizeof(temp) - strlen(temp) - 1);
            j = strlen(temp);
            i += 2;
            replaced = true;
            continue;
        } else {
            temp[j++] = descriptionTemplate[i++];
        }
        escaping = false;
    }
    temp[j] = '\0';
    strncpy(output, temp, output_size);
    output[output_size - 1] = '\0';
}

void extract_placeholder(const char *descriptionTemplate, char *placeholder) {
    int length = strlen(descriptionTemplate);
    int i = 0;
    placeholder[0] = '@';
    placeholder[1] = 's';
    placeholder[2] = '\0';

    while (i < length - 1) {
        if (descriptionTemplate[i] == '@' &&
            ((descriptionTemplate[i + 1] >= 'A' && descriptionTemplate[i + 1] <= 'Z') ||
             (descriptionTemplate[i + 1] >= 'a' && descriptionTemplate[i + 1] <= 'z'))) {
            placeholder[0] = descriptionTemplate[i];
            placeholder[1] = descriptionTemplate[i + 1];
            placeholder[2] = '\0';
            return;
        }
        i++;
    }
}
