int test_performance() {
    asic_t *device = asic_init(TI83p);

    struct timespec start, stop;
    unsigned long long t;
    int i;
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    for (i = 0; i < 1000000; i++) {
        i -= cpu_execute(device->cpu, 1);
    }
    clock_gettime(CLOCK_MONOTONIC_RAW, &stop);
    t = (stop.tv_sec*1000000000UL) + stop.tv_nsec;
    t -= (start.tv_sec * 1000000000UL) + start.tv_nsec;
    printf("executed 1,000,000 cycles in %llu microseconds (~%llu MHz)\n", t/1000, 1000000000/t);

    asic_free(device);
    return -1;
}
