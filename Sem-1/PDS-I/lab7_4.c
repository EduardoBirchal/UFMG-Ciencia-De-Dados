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
    char str[MAX_STR], subStr[MAX_STR];
    int j = 0;

    fgets(str, MAX_STR, stdin);
    fgets(subStr, MAX_STR, stdin);

    removeNewlines (subStr, MAX_STR); 

    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == subStr[j]) {
            j++;

            if (subStr[j] == '\0') {
                printf("É substring\n");
                break;
            }
        }
        else {
            j = 0;
        }
    }

    if (j == 0)
        printf("Não é substring\n");

    return EXIT_SUCCESS;
}