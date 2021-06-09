#include <ctype.h>
#include <stdbool.h>

bool isdigit(char c)
{
  if(c >= '0' && c <= '9')
    return 1;
  else
    return 0;
}
