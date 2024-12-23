#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdlib.h>
#include <string.h>

char* strndup(const char* str, size_t n) {
    size_t len = 0;
    while (len < n && str[len] != '\0') {
        len++;
    }

    char* result = (char*)malloc(len + 1);
    if (!result) {
        return NULL;
    }

    for (size_t i = 0; i < len; i++) {
        result[i] = str[i];
    }

    result[len] = '\0';
    return result;
}

char* strdup_c(const char* str) {
    size_t len = strlen(str);
    char* new_str = malloc(len + 1);
    if (new_str == NULL) {
        return NULL;
    }
    strcpy(new_str, str);
    return new_str;
}
