#include <stdio.h>
#include <string.h>

#define BUFFER_SIZE 1024

int main() {
    char buffer[BUFFER_SIZE];

    fgets(buffer, BUFFER_SIZE, stdin);

    for (int i = 0; buffer[i] != '\0'; i++) {
        if (buffer[i] == ' ') {
            buffer[i] = '_';
        }
    }

    printf("%s", buffer);

    return 0;
}
