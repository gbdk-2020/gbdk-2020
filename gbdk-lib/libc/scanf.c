#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>

static unsigned char scan_skip(char *s, unsigned char i)
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

static unsigned char scan_int(char *s, unsigned char i, unsigned char base, char *nb)
{
  char n = 0;
  unsigned char j, sign = 0;

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

static unsigned char scan_long(char *s, unsigned char i, unsigned char base, int *nb)
{
  int n = 0;
  unsigned char j, sign = 0;

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

char scanf(char *fmt, ...)
{
  va_list ap;
  char s[64];
  unsigned char i = 0;
  char nb = 0;

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
	i = scan_int(s, i, 10, va_arg(ap, char *));
	break;
      case 'o':
	/* octal int */
	i = scan_int(s, i, 8, va_arg(ap, char *));
	break;
      case 'x':
	/* hexadecimal int */
	i = scan_int(s, i, 16, va_arg(ap, char *));
	break;
      case 's':
	/* string */
	{
	  char j = 0;
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
	  i = scan_long(s, i, 10, va_arg(ap, int *));
	  break;
	case 'o':
	  /* octal long */
	  i = scan_long(s, i, 8, va_arg(ap, int *));
	  break;
	case 'x':
	  /* hexadecimal long */
	  i = scan_long(s, i, 16, va_arg(ap, int *));
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
