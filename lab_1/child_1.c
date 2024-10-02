#include <stdio.h>
#include <ctype.h>
#include <string.h>

#define BUFFER_SIZE 1024

int main() {
    char buffer[BUFFER_SIZE];

    fgets(buffer, BUFFER_SIZE, stdin);

    for (int i = 0; buffer[i] != '\0'; i++) {
        buffer[i] = toupper(buffer[i]);
    }

    printf("%s", buffer);

    return 0;
}
