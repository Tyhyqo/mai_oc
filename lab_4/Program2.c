#include <stdio.h>
#include <dlfcn.h>

int main() {
    char command;
    printf("Введите команду:\n");
    while (scanf(" %c", &command) != EOF) {
        if (command == '1') {
            float A, deltaX;
            scanf("%f %f", &A, &deltaX);

            void* handle = dlopen("./libDerivative1.so", RTLD_LAZY);
            if (!handle) {
                fprintf(stderr, "Ошибка загрузки библиотеки: %s\n", dlerror());
                return 1;
            }
            float (*Derivative)(float, float) = dlsym(handle, "Derivative");
            char* error = dlerror();
            if (error != NULL) {
                fprintf(stderr, "Ошибка загрузки функции: %s\n", error);
                dlclose(handle);
                return 1;
            }
            printf("Результат: %f\n", Derivative(A, deltaX));
            dlclose(handle);
        } else if (command == '2') {
            int K;
            scanf("%d", &K);

            void* handle = dlopen("./libPi1.so", RTLD_LAZY);
            if (!handle) {
                fprintf(stderr, "Ошибка загрузки библиотеки: %s\n", dlerror());
                return 1;
            }
            float (*Pi)(int) = dlsym(handle, "Pi");
            char* error = dlerror();
            if (error != NULL) {
                fprintf(stderr, "Ошибка загрузки функции: %s\n", error);
                dlclose(handle);
                return 1;
            }
            printf("Результат: %f\n", Pi(K));
            dlclose(handle);
        } else {
            printf("Неизвестная команда.\n");
        }
    }
    return 0;
}