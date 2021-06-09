#include <ctype.h>
#include <stdbool.h>

bool islower(char c)
{
  if(c >= 'a' && c <= 'z')
    return 1;
  else
    return 0;
}
