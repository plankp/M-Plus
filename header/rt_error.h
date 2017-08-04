#ifndef _C_MP_RT_ERR_H__
#define _C_MP_RT_ERR_H__

#include "rt_tags.h"

/* Getting rid of <sal.h> C4001 warnings */
#if _MSC_VER && !__INTEL_COMPILER
#pragma warning(push)
#pragma warning(disable : 4001)
#endif

#include <stdlib.h>

#if _MSC_VER && !__INTEL_COMPILER
#pragma warning(pop)
#endif

#ifdef __cplusplus
extern "C"
{
#endif

  typedef struct rt_error_t
  {
    rt_tag_t tag;
    size_t refs;
    rt_data_t *content;
  } rt_error_t;

  /* ownership is transfered from the caller */

  extern rt_data_t *create_error(rt_data_t *data);

#ifdef __cplusplus
};
#endif

#endif /* !_C_MP_RT_ERR_H__ */
