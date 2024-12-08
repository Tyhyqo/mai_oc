float Pi(int K) {
    float pi = 0.0f;
    for (int i = 0; i < K; i++) {
        pi += (i % 2 == 0 ? 1.0f : -1.0f) / (2 * i + 1);
    }
    return pi * 4;
}