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

#define BGB_STR(A) #A
#define BGB_CONCAT(A,B) BGB_STR(A:B)
#define BGB_PROFILE_BEGIN(MSG) BGB_MESSAGE(BGB_CONCAT(MSG,%ZEROCLKS%));
#define BGB_PROFILE_END(MSG) BGB_MESSAGE(BGB_CONCAT(MSG,%-8+LASTCLKS%));
#define BGB_TEXT(MSG) BGB_MESSAGE(BGB_STR(MSG))


void BGB_profiler_message();

static void * __BGB_PROFILER_INIT = &BGB_profiler_message;

#endif