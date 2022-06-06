
#pragma std_c99

#include <stdint.h>

#ifdef __SDCC_LONGLONG

long long _rrslonglong(long long l, char s)
{

  uint8_t *const b = (uint8_t *)(&l);
  unsigned char shift,t1,t2, sign;
  signed char zb,i;

  sign=b[7]&0x80;
  
  zb=s>>3;
  if(zb) {
    i=0;
    for(;i<(8-zb);i++) {
      b[i]=b[zb+i];
    }
    for(;i<8;i++)
      b[i]=sign?0xff:0x00;
  }
  
  shift=s&0x7;
  while(shift--) {
    t2=sign;
    for(i=7-zb;i>=0;i--) {
      t1=b[i]&1;
      b[i]=(b[i]>>1)|t2;
      t2=t1?0x80:0;
    } 
  }
  
  return(l);
}

#endif
