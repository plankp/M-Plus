#include "rt_tags.h"

const char *
rt_tag_to_str(const rt_tag_t tag)
{
#define X(n) case n: return #n;
  switch (tag)
    { RT_TAGS }
#undef X
  return "ILLEGAL TYPE TAG";
}
