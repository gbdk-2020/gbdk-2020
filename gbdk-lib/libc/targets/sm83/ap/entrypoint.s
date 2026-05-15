

	.area _startjump (ABS)
	
	.org 0x100
	
	jr .code_start		; should be located around 0x150, which is in range of a jr
