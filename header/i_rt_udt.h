#ifndef _C_MP_I_RT_UDT_H__
#define _C_MP_I_RT_UDT_H__

#include "rt_tags.h"

#include <stddef.h>

#ifndef __cplusplus
#include <stdbool.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

  typedef struct rt_udt_t	/* Inherit from this interface */
  {
    /* Always UDT */
    const rt_tag_t tag;

    /* Preferred type name */
    const char *type;

    /* Shallow clone routine, can be same as the deep clone */
    rt_data_t *(*sclone)(void *self);

    /* Deep clone routine */
    rt_data_t *(*dclone)(const void *self);

    /* String representation of object; string is dynamically allocated */
    char *(*to_str)(const void *self);

    /* Test if object is truthy. */
    bool (*to_bool)(const void *self);

    /* Dispatch method: parameters are based on context
     *   - e      => Always the environment the dispatch is done under
     *   - p1
     *   : NULL => self( )       [1]
     *   : x    => self(x)       [2]
     *
     * [1]: A function call with no parameters
     * [2]: A function call with one parameter
     */
    rt_data_t *(*apply)(rt_env_t *e, rt_data_t *p);

    /* Destructor. If returns true, the block itself will be freed
     * by the runtime (or should be freed by its enclosing object).
     */
    bool (*dealloc)(void *self);
  } rt_udt_t;

  inline
  rt_udt_t
  create_base_udt(const char *type_name)
  {
    /* Initialize all other fields as NULL */
    return (rt_udt_t)
      { .tag = UDT, .type = type_name };
  }

#ifdef __cplusplus
};
#endif

#endif /* !_C_MP_I_RT_UDT_H__ */
