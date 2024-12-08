float Pi(int K) {
    float pi = 1.0f;
    for (int i = 1; i <= K; i++) {
        pi *= ((2.0f * i) / (2.0f * i - 1.0f)) * ((2.0f * i) / (2.0f * i + 1.0f));
    }
    return pi * 2;
}