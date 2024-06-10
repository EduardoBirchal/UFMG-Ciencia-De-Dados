#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STR 100

int main (int argc, char **argv) {
    char str[MAX_STR], c1, c2;

    fgets(str, MAX_STR, stdin);
    scanf("%c\n%c", &c1, &c2);

    for (int i=0; str[i] != '\0'; i++) {
        if (str[i] == c1) {
            str[i] = c2;
            break;
        }
    }

    printf("%s", str);

    return EXIT_SUCCESS;
}