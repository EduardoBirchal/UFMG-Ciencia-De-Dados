#include <stdio.h>
#include <stdlib.h>

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
    char str1[MAX_STR], str2[MAX_STR];

    fgets(str1, MAX_STR, stdin);
    fgets(str2, MAX_STR, stdin);

    removeNewlines (str1, MAX_STR);

    printf("%s%s", str1, str2);

    return EXIT_SUCCESS;
}