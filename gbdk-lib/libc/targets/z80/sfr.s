_MEMORY_CTL     = 0x3E

_JOY_CTL        = 0x3F
_VCOUNTER       = 0x7E
_PSG            = 0x7F
_HCOUNTER       = 0x7F

_VDP_DATA       = 0xBE
_VDP_CMD        = 0xBF
_VDP_STATUS     = 0xBF

_JOY_PORT1      = 0xDC
_JOY_PORT2      = 0xDD

_FMADDRESS      = 0xF0
_FMDATA         = 0xF1
_AUDIOCTRL      = 0xF2

.globl _MEMORY_CTL
.globl _JOY_CTL, _VCOUNTER, _PSG, _HCOUNTER, _JOY_PORT1, _JOY_PORT2
.globl _VDP_DATA, _VDP_CMD, _VDP_STATUS
.globl _FMADDRESS, _FMDATA, _AUDIOCTRL