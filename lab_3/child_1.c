#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <string.h>
#include <ctype.h>

#define SHARED_MEM_NAME "/shared_mem"
#define SEM_PARENT_CHILD1 "/sem_parent_child1"
#define SEM_CHILD1_CHILD2 "/sem_child1_child2"
#define BUFFER_SIZE 1024

int main() {
    int fd;
    char *shared_mem;
    sem_t *sem_pc, *sem_c1c2;

    // Открываем семафоры
    sem_pc = sem_open(SEM_PARENT_CHILD1, 0);
    sem_c1c2 = sem_open(SEM_CHILD1_CHILD2, 0);

    if (sem_pc == SEM_FAILED || sem_c1c2 == SEM_FAILED) {
        perror("sem_open failed");
        exit(1);
    }

    // Открываем общий файл и отображаем его в память
    fd = shm_open(SHARED_MEM_NAME, O_RDWR, 0666);
    shared_mem = mmap(NULL, BUFFER_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if (shared_mem == MAP_FAILED) {
        perror("mmap failed");
        exit(1);
    }

    // Ждем от родителя
    sem_wait(sem_pc);

    // Преобразуем строку в верхний регистр
    for (int i = 0; shared_mem[i]; i++) {
        shared_mem[i] = toupper(shared_mem[i]);
    }

    // Сообщаем child_2
    sem_post(sem_c1c2);

    // Очищаем ресурсы
    munmap(shared_mem, BUFFER_SIZE);
    close(fd);
    sem_close(sem_pc);
    sem_close(sem_c1c2);

    return 0;
}