#ifndef __GBDK_EMU_DEBUG_H_INCLUDE
#define __GBDK_EMU_DEBUG_H_INCLUDE

#if defined(__TARGET_gb) || defined(__TARGET_ap) || defined(__TARGET_sms) || defined(__TARGET_gg)
  #include <gb/emu_debug.h>
#elif defined(__TARGET_duck)
  #error Not implemented yet
#else
  #error Unrecognized port
#endif

#endif