#include <stdio.h>
#include <stdlib.h>
#include <gb/gb.h>
#include <gb/bgb_emu.h>

#define INT_MAX 0x7fff
#define INT_MIN (-(INT_MAX) - 1)

volatile int magic;

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

	puts("Starting benchmark...");

	BGB_PROFILE_BEGIN(libr_abs);
	for (i = -0x7fff - 1; ; i++) {
		magic = abs(i);
		if (i == 0x7fff)
			break;
	}
	BGB_PROFILE_END(libr_abs);

	BGB_PROFILE_BEGIN(safe_abs);
	for (i = -0x7fff - 1; ; i++) {
		magic = safe_abs(i);
		if (i == 0x7fff)
			break;
	}
	BGB_PROFILE_END(safe_abs);

	puts("done");

	return 0;
}
