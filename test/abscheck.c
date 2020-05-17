#include <stdio.h>
#include <stdlib.h>
#include <gb/gb.h>

#define INT_MAX 0x7fff
#define INT_MIN (-(INT_MAX) - 1)

int safe_abs(int i)
{
	if (i < 0)
		return -i;
	else
		return i;
}

int main(void)
{
	int i;

	for (i = -0x7fff - 1; ; i++) {
		if (abs(i) != safe_abs(i))
			printf("error %d\n", i);
		if (i == 0x7fff)
			break;
	}

	puts("done");

	return 0;
}
