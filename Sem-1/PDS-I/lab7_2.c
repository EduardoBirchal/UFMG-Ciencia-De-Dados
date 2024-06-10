#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STR 100

void removeChar (char* str, int index) {
    int i;

    for (i = index; str[i] != '\0'; i++) {
        str[i] = str[i + 1];
    }
}

void removeNewlines (char* str, int len) {
    for (int i = 0; i < len; i++) {
        if (str[i] == '\n') removeChar (str, i);
    }
}

int main (int argc, char **argv) {
    char str[MAX_STR];

    fgets(str, MAX_STR, stdin);

    removeNewlines (str, MAX_STR);

    for(int i=strlen(str); i >= 0; i--) {
        printf("%c", str[i]);
    }

    return EXIT_SUCCESS;
}