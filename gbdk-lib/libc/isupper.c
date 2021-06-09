#include <ctype.h>
#include <stdbool.h>

bool isupper(char c)
{
  if(c >= 'A' && c <= 'Z')
    return 1;
  else
    return 0;
}
