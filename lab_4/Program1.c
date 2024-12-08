#include <stdio.h>

extern float Derivative(float, float);
extern float Pi(int);

int main() {
    char command;
    printf("Введите команду:\n");
    while (scanf(" %c", &command) != EOF) {
        if (command == '1') {
            float A, deltaX;
            scanf("%f %f", &A, &deltaX);
            printf("Результат: %f\n", Derivative(A, deltaX));
        } else if (command == '2') {
            int K;
            scanf("%d", &K);
            printf("Результат: %f\n", Pi(K));
        } else {
            printf("Неизвестная команда.\n");
        }
    }
    return 0;
}