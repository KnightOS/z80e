int test_performance() {
	if (getenv("TRAVIS") != NULL) {
		// Performance tests do not work on Travis
		printf("disabled on Travis builds\n");
		return -1;
	}
	asic_t *device = asic_init(TI83p, NULL);

	clock_t start, stop;
	int i;
	start = clock();
	for (i = 0; i < 1000000; i++) {
		i -= cpu_execute(device->cpu, 1);
	}
	stop = clock();
	double time = (double)(stop - start) / (CLOCKS_PER_SEC / 1000);
	double mHz = 1000.0 / time;
	printf("executed 1,000,000 cycles in %f milliseconds (~%f MHz)\n", time, mHz);

	asic_free(device);
	return -1;
}
