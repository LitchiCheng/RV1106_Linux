#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/mman.h>

uint16_t color = 0xff00;
#define mmap_size 128*160*2

int main()
{
	int fd = open("/dev/fb0", O_RDWR);
	uint16_t *pfb0 = mmap(NULL, mmap_size, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);

	for (int i = 0; i < (mmap_size / 2); i++)
		pfb0[i] = color;

	close(fd);
	return 0;
}
