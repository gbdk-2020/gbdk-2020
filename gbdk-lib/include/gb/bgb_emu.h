#ifndef __BGB_EMU_INCLUDE
#define __BGB_EMU_INCLUDE

#define ADD_DOLLARD(A) ADD_DOLLARD1 (A)
#define ADD_DOLLARD1(A) A##$
#define BGB_MESSAGE(message_text) BGB_MESSAGE1(ADD_DOLLARD(__LINE__), message_text)
#define BGB_MESSAGE1(lbl, message_text) \
__asm \
  ld d, d \
  jr lbl \
  .dw 0x6464 \
  .dw 0x0000 \
  .ascii message_text \
lbl: \
__endasm

void BGB_profiler_message();

void * __BGB_PROFILER_INIT = &BGB_profiler_message();


#endif