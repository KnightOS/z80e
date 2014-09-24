#include <z80e/ti/asic.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int stop = 0;

void cpu_reset(void *device, uint8_t data) {
	printf("Jumped to 0x00!\n");
	exit(0);
}

uint8_t write_text(void *device) {
	asic_t *asic = device;

	if (asic->cpu->registers.C == 2) {
		printf("%c", asic->cpu->registers.E);
	} else if (asic->cpu->registers.C == 9) {
		int i;
		for (i = asic->cpu->registers.DE; asic->mmu->ram[i & 0xffff] != '$'; i++) {
			printf("%c", asic->mmu->ram[i & 0xffff]);
			if (asic->mmu->ram[i & 0xffff] == 0) {
				break;
			}
		}
	}
	fflush(stdout);
	return 0;
}

int main(int argc, char **argv) {
	if (argc != 2) {
		printf("zex `file` - run the Z80 instruction exerciser\n");
		return 1;
	}

	asic_t *device = asic_init(TI84p, NULL);

	int i = 0;
	for (i = 0; i < 0x100; i++) {
		device->cpu->devices[i].write_out = cpu_reset;
		device->cpu->devices[i].read_in = write_text;
		device->cpu->devices[i].device = device;
	}

	long file_size;
	FILE *file;

	if ((file = fopen(argv[1], "rb")) == NULL) {
		printf("%s\n", strerror(errno));
		return 1;
	}
	fseek(file, 0, SEEK_END);

	file_size = ftell(file);
	fseek(file, 0, SEEK_SET);
	fread(device->mmu->ram + 0x100, 1, file_size, file);
	fclose(file);

	device->mmu->ram[0] = 0xd3; /* OUT N, A */
	device->mmu->ram[1] = 0x00;

	device->mmu->ram[5] = 0xdb; /* IN A, N */
	device->mmu->ram[6] = 0x00;
	device->mmu->ram[7] = 0xc9; /* RET */    

	device->mmu->banks[0].page = 0;
	device->mmu->banks[1].page = 1;
	device->mmu->banks[2].page = 2;
	device->mmu->banks[3].page = 3;

	device->mmu->banks[0].flash = 0;
	device->mmu->banks[1].flash = 0;
	device->mmu->banks[2].flash = 0;
	device->mmu->banks[3].flash = 0;

	device->cpu->registers.PC = 0x100;

	while (!stop) {
		cpu_execute(device->cpu, 10000);
	}
	return 0;
}

