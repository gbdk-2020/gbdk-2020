#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>

static uint8_t scan_skip(char *s, uint8_t i)
{
oncemore:
  while(isspace(s[i]))
    i++;
  if(s[i] == 0) {
    gets(s);
    i = 0;
    goto oncemore;
  }
  return i;
}

static uint8_t scan_int(char *s, uint8_t i, uint8_t base, int8_t *nb)
{
  int8_t n = 0;
  uint8_t j, sign = 0;

  switch(s[i])
    {
    case '-':
      sign++;
      /* and fall through */
    case '+':
      ++i;
      break;
    }
  while(1) {
    if(isdigit(s[i]))
      j = s[i] - '0';
    else if(isalpha(s[i]))
      j = toupper(s[i]) - 'A' + 10;
    else
      break;
    if(j >= base)
      break;
    n = base * n + j;
    i++;
  }
  *nb = (sign == 0 ? n : -n);
  return i;
}

static uint8_t scan_long(char *s, uint8_t i, uint8_t base, int16_t *nb)
{
  int16_t n = 0;
  uint8_t j, sign = 0;

  switch(s[i])
    {
    case '-':
      sign++;
      /* and fall through */
    case '+':
      ++i;
      break;
    }
  while(1) {
    if(isdigit(s[i]))
      j = s[i] - '0';
    else if(isalpha(s[i]))
      j = toupper(s[i]) - 'A' + 10;
    else
      break;
    if(j >= base)
      break;
    n = base * n + j;
    i++;
  }
  *nb = (sign == 0 ? n : -n);
  return i;
}

int8_t scanf(char *fmt, ...)
{
  va_list ap;
  char s[64];
  uint8_t i = 0;
  int8_t nb = 0;

  gets(s);
  va_start(ap, fmt);
  for(; *fmt; fmt++) {
    if(isspace(*fmt))
      continue;
    i = scan_skip(s, i);
    if(*fmt == '%') {
      switch(*++fmt) {
      case 'c':
	/* char */
	*va_arg(ap, char *) = s[i++];
	break;
      case 'd':
	/* decimal int */
      case 'u':
	/* unsigned int */
	i = scan_int(s, i, 10, va_arg(ap, int8_t *));
	break;
      case 'o':
	/* octal int */
	i = scan_int(s, i, 8, va_arg(ap, int8_t *));
	break;
      case 'x':
	/* hexadecimal int */
	i = scan_int(s, i, 16, va_arg(ap, int8_t *));
	break;
      case 's':
	/* string */
	{
	  int8_t j = 0;
	  char *d = va_arg(ap, char *);
	  while((d[j++] = s[i++]) != 0)
	    ;
	}
      break;
      case 'l':
	/* long */
	switch(*++fmt) {
	case 'd':
	  /* decimal long */
	case 'u':
	  /* unsigned long */
	  i = scan_long(s, i, 10, va_arg(ap, int16_t *));
	  break;
	case 'o':
	  /* octal long */
	  i = scan_long(s, i, 8, va_arg(ap, int16_t *));
	  break;
	case 'x':
	  /* hexadecimal long */
	  i = scan_long(s, i, 16, va_arg(ap, int16_t *));
	  break;
	}
	break;
      default:
	if(s[i] != *fmt)
	  return -1;
	break;
      }
      nb++;
    } else
      if(s[i] != *fmt)
	return -1;
  }
  va_end(ap);

  return nb;
}
