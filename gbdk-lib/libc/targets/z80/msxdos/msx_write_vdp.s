        .include        "global.s"

        .title  "write vdp"
        .module write_vdp

        .ez80

        .area   _CODE

_WRITE_VDP_CMD::        
        VDP_WRITE_CMD h, l
        ret
_WRITE_VDP_DATA::        
        VDP_WRITE_DATA h, l
        ret
