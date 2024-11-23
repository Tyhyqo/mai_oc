#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <string.h>

#define SHARED_MEM_NAME "/shared_mem"
#define SEM_CHILD1_CHILD2 "/sem_child1_child2"
#define SEM_CHILD2_PARENT "/sem_child2_parent"
#define BUFFER_SIZE 1024

int main() {
    int fd;
    char *shared_mem;
    sem_t *sem_c1c2, *sem_c2p;

    // Открываем семафоры
    sem_c1c2 = sem_open(SEM_CHILD1_CHILD2, 0);
    sem_c2p = sem_open(SEM_CHILD2_PARENT, 0);

    if (sem_c1c2 == SEM_FAILED || sem_c2p == SEM_FAILED) {
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

    // Ждем от child_1
    sem_wait(sem_c1c2);

    // Заменяем пробелы на '_'
    for (int i = 0; shared_mem[i]; i++) {
        if (shared_mem[i] == ' ') {
            shared_mem[i] = '_';
        }
    }

    // Сообщаем родителю
    sem_post(sem_c2p);

    // Очищаем ресурсы
    munmap(shared_mem, BUFFER_SIZE);
    close(fd);
    sem_close(sem_c1c2);
    sem_close(sem_c2p);

    return 0;
}