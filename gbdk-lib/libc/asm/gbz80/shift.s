        ;;
__rrulong_rrx_s::                
        ld      hl,#2
        add     hl,sp

        ld      a,(hl+)
        ld      e, a
        ld      a,(hl+)
        ld      d, a
        ld      a,(hl+)
        ld      c, a
        ld      a,(hl+)
        ld      b, a
        ld      a,(hl)

        ld      l,c
        ld      h,b
1$:
        or      a,a
        ret     z

        rr      h
        rr      l
        rr      d
        rr      e

        dec     a
        jp      1$

__rrslong_rrx_s::        
        ld      hl,#2
        add     hl,sp
                
        ld      a,(hl+)
        ld      e, a
        ld      a,(hl+)
        ld      d, a
        ld      a,(hl+)
        ld      c, a
        ld      a,(hl+)
        ld      b, a
        ld      a,(hl)

        ld      l,c
        ld      h,b
1$:
        or      a,a
        ret     z

        sra     h
        rr      l
        rr      d
        rr      e

        dec     a
        jp      1$
        
__rlslong_rrx_s::                
__rlulong_rrx_s::                
        ld      hl,#2
        add     hl,sp
        
        ld      a,(hl+)
        ld      e, a
        ld      a,(hl+)
        ld      d, a
        ld      a,(hl+)
        ld      c, a
        ld      a,(hl+)
        ld      b, a
        ld      a,(hl)

        ld      l,c
        ld      h,b
1$:
        or      a,a
        ret     z

        rl      e
        rl      d
        rl      l
        rl      h

        dec     a        
        jp      1$
