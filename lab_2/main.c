#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#define MAX_SIZE 1000

int matrix[MAX_SIZE][MAX_SIZE];
int temp_matrix[MAX_SIZE][MAX_SIZE];
int N; // Размер матрицы
int window_size;
int K; // Количество повторений фильтра
int max_threads;
pthread_barrier_t barrier;

typedef struct {
    int thread_id;
    int num_threads;
} ThreadData;

int compare_ints(const void *a, const void *b)
{
    return (*(int *)a - *(int *)b);
}

void *median_filter(void *arg)
{
    ThreadData *data = (ThreadData *)arg;
    int thread_id = data->thread_id;
    int num_threads = data->num_threads;

    int half_window = window_size / 2;
    int max_values = window_size * window_size;
    int *values = malloc(max_values * sizeof(int));
    if (values == NULL) {
        perror("Ошибка выделения памяти");
        pthread_exit(NULL);
    }

    for (int k = 0; k < K; k++) {
        for (int i = thread_id; i < N; i += num_threads) {
            for (int j = 0; j < N; j++) {
                int count = 0;

                for (int x = i - half_window; x <= i + half_window; x++) {
                    for (int y = j - half_window; y <= j + half_window; y++) {
                        if (x >= 0 && x < N && y >= 0 && y < N) {
                            values[count++] = matrix[x][y];
                        } else {
                            values[count++] = 0;
                        }
                    }
                }

                qsort(values, count, sizeof(int), compare_ints);

                temp_matrix[i][j] = values[count / 2];
            }
        }

        pthread_barrier_wait(&barrier);

        for (int i = thread_id; i < N; i += num_threads) {
            for (int j = 0; j < N; j++) {
                matrix[i][j] = temp_matrix[i][j];
            }
        }

        pthread_barrier_wait(&barrier);
    }

    free(values);
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        printf("Использование: %s <количество потоков>\n", argv[0]);
        return 1;
    }

    max_threads = atoi(argv[1]);
    if (max_threads <= 0) {
        printf("Количество потоков должно быть положительным числом.\n");
        return 1;
    }

    printf("Введите размер матрицы N (максимум %d): ", MAX_SIZE);
    scanf("%d", &N);
    if (N <= 0 || N > MAX_SIZE) {
        printf("Недопустимый размер матрицы.\n");
        return 1;
    }

    // printf("Введите элементы матрицы:\n");
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            // scanf("%d", &matrix[i][j]);
            matrix[i][j] = rand() % 256;
        }
    }

    printf("Введите количество повторений фильтра K: ");
    scanf("%d", &K);
    if (K <= 0) {
        printf("Количество повторений должно быть положительным числом.\n");
        return 1;
    }

    printf("Введите размер окна: ");
    scanf("%d", &window_size);
    if (window_size <= 0 || window_size > N) {
        printf("Недопустимый размер окна.\n");
        return 1;
    }

    pthread_barrier_init(&barrier, NULL, max_threads);

    pthread_t threads[max_threads];
    ThreadData thread_data[max_threads];

    // Засекаем время начала
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    for (int i = 0; i < max_threads; i++) {
        thread_data[i].thread_id = i;
        thread_data[i].num_threads = max_threads;
        pthread_create(&threads[i], NULL, median_filter, &thread_data[i]);
    }

    for (int i = 0; i < max_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    // Засекаем время окончания
    clock_gettime(CLOCK_MONOTONIC, &end);
    double time_spent = (end.tv_sec - start.tv_sec) +
                        (end.tv_nsec - start.tv_nsec) / 1e9;

    printf("Результат после применения медианного фильтра:\n");
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }

    printf("Время выполнения программы: %f секунд\n", time_spent);
    pthread_barrier_destroy(&barrier);
    return 0;
}