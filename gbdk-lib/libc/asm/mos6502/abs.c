
int abs(int j) __naked
{
  (void)j;
  __asm__("cpx #0x00");
  __asm__("bpl skip");
  __asm__("___negax::");
  __asm__("sec");
  __asm__("eor #0xff");
  __asm__("adc #0x00");
  __asm__("pha");
  __asm__("txa");
  __asm__("eor #0xff");
  __asm__("adc #0x00");
  __asm__("tax");
  __asm__("pla");
  __asm__("skip:");
  __asm__("rts");
  //	return (j < 0) ? -j : j;
}
