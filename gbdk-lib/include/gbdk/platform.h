#ifndef __PLATFORM_H_INVCLUDE
#define __PLATFORM_H_INVCLUDE

#if defined(__TARGET_gb) || defined(__TARGET_ap)
  #include <gb/gb.h>
  #include <gb/cgb.h>
  #include <gb/sgb.h>
#elif defined(__TARGET_sms) || defined(__TARGET_gg)
  #include <sms/sms.h>
#else
  #error Unrecognized port
#endif

#endif