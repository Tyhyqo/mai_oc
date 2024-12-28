import matplotlib.pyplot as plt

# Данные для графика 1
threads = [1, 4, 8]
times1 = [0.151009, 0.062256, 0.061879]

plt.figure()
plt.plot(threads, times1, marker='o')
plt.title('Время выполнения при различном количестве потоков')
plt.xlabel('Количество потоков')
plt.ylabel('Время выполнения (секунды)')
plt.grid(True)
plt.savefig('graphs/graph1.png')

# Данные для графика 2
matrix_sizes = [10, 50, 100]
times2 = [0.004964, 0.007954, 0.022311]

plt.figure()
plt.plot(matrix_sizes, times2, marker='o')
plt.title('Время выполнения при различном размере матрицы')
plt.xlabel('Размер матрицы (N)')
plt.ylabel('Время выполнения (секунды)')
plt.grid(True)
plt.savefig('graphs/graph2.png')

# Данные для графика 3
window_sizes = [3, 6, 10]
times3 = [0.012318, 0.039672, 0.074484]

plt.figure()
plt.plot(window_sizes, times3, marker='o')
plt.title('Время выполнения при различном размере окна')
plt.xlabel('Размер окна')
plt.ylabel('Время выполнения (секунды)')
plt.grid(True)
plt.savefig('graphs/graph3.png')