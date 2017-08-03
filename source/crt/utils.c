#include "utils.h"

char *
clone_str (const char *str)
{
  if (!str) return NULL;
  size_t len = strlen(str) + 1;
  char *s = malloc(len * sizeof(char));
  memcpy(s, str, len);
  return s;
}
