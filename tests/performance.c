int test_performance() {
    asic_t *device = asic_init(TI83p);

    clock_t start, stop;
    int i;
    start = clock();
    for (i = 0; i < 1000000; i++) {
        cpu_execute(device->cpu, 1);
    }
    stop = clock();
    double time = (double)(stop - start) / (CLOCKS_PER_SEC / 1000);
    double mHz = 1000.0 / time;
    printf("executed 1,000,000 cycles in %f milliseconds (~%f mHz)\n", time, mHz);

    asic_free(device);
    return -1;
}
