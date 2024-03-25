	.include	"global.s"

	.title	"DIV_REG emulation"
	.module	DIV_REG_emu

	.area	_HOME

_get_r_reg::
	LD	A, R
	RET
