
#pragma std_c99

#include <stdint.h>

#ifdef __SDCC_LONGLONG

unsigned long long _rrulonglong(unsigned long long l, char s)
{

  uint8_t *const b = (uint8_t *)(&l);
  unsigned char shift,t1,t2;
  signed char zb,i;
  
  zb=s>>3;
  if(zb) {
    for(i=0;i<(8-zb);i++) {
      b[i]=b[zb+i];
    }
  
    for(;i<8;i++)
      b[i]=0;
  }
  
  shift=s&0x7;
  while(shift--) {
    t2=0;
    for(i=7-zb;i>=0;i--) {
      t1=b[i]&1;
      b[i]=(b[i]>>1)|t2;
      t2=t1?0x80:0;
    } 
  }
  
  return(l);
}

#endif
