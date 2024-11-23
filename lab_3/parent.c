#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <string.h>
#include <sys/wait.h>

#define SHARED_MEM_NAME "/shared_mem"
#define SEM_PARENT_CHILD1 "/sem_parent_child1"
#define SEM_CHILD1_CHILD2 "/sem_child1_child2"
#define SEM_CHILD2_PARENT "/sem_child2_parent"
#define BUFFER_SIZE 1024

int main() {
    int fd;
    char *shared_mem;
    sem_t *sem_pc, *sem_c1c2, *sem_c2p;

    // Создаем и инициализируем семафоры
    sem_unlink(SEM_PARENT_CHILD1);
    sem_unlink(SEM_CHILD1_CHILD2);
    sem_unlink(SEM_CHILD2_PARENT);
    sem_pc = sem_open(SEM_PARENT_CHILD1, O_CREAT, 0666, 0);
    sem_c1c2 = sem_open(SEM_CHILD1_CHILD2, O_CREAT, 0666, 0);
    sem_c2p = sem_open(SEM_CHILD2_PARENT, O_CREAT, 0666, 0);

    // Создаем общий файл и отображаем его в память
    shm_unlink(SHARED_MEM_NAME);
    fd = shm_open(SHARED_MEM_NAME, O_CREAT | O_RDWR, 0666);
    ftruncate(fd, BUFFER_SIZE);
    shared_mem = mmap(NULL, BUFFER_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if (shared_mem == MAP_FAILED) {
        perror("mmap failed");
        exit(1);
    }

    // Создаем дочерние процессы
    pid_t pid1 = fork();

    if (pid1 == 0) {
        // Запускаем child_1
        execlp("./child_1", "./child_1", NULL);
        perror("exec failed");
        exit(1);
    }

    pid_t pid2 = fork();

    if (pid2 == 0) {
        // Запускаем child_2
        execlp("./child_2", "./child_2", NULL);
        perror("exec failed");
        exit(1);
    }

    // Основной процесс
    char input[BUFFER_SIZE];
    printf("Введите строку: ");
    fgets(input, BUFFER_SIZE, stdin);
    input[strcspn(input, "\n")] = '\0'; // Удаляем символ новой строки

    // Записываем ввод в общую память
    strcpy(shared_mem, input);

    // Сообщаем child_1, что ввод готов
    sem_post(sem_pc);

    // Ждем от child_2
    sem_wait(sem_c2p);

    // Читаем преобразованную строку из общей памяти
    printf("Результат: %s\n", shared_mem);

    // Очищаем ресурсы
    wait(NULL);
    wait(NULL);
    munmap(shared_mem, BUFFER_SIZE);
    close(fd);
    shm_unlink(SHARED_MEM_NAME);
    sem_close(sem_pc);
    sem_close(sem_c1c2);
    sem_close(sem_c2p);
    sem_unlink(SEM_PARENT_CHILD1);
    sem_unlink(SEM_CHILD1_CHILD2);
    sem_unlink(SEM_CHILD2_PARENT);

    return 0;
}