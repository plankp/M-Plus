#ifndef _C_MP_I_RT_UDT_H__
#define _C_MP_I_RT_UDT_H__

#include "rt_tags.h"
#include "i_rt_env.h"

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
    rt_tag_t tag;

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

    /* Test if object is equals. obj is not guaranteed to have the same
     * type as self.
     *
     * The runtime will not dispatch this method if obj is NULL, ERR or 
     * has the same memory location.
     *
     * This method *must* have the property of (x == y) == (y == x) as
     * INT(1) == UDT() will result in a dispatch to the UDT's equals
     * method.
     *
     * You are encouraged to provide implementation as the default only
     * checks if they have the same memory location.
     */
    bool (*equals)(const void *self, const rt_data_t *obj);

    /* Dispatch method: parameters are based on context
     *   - e      => Always the environment the dispatch is done under
     *   - p1
     *   : NULL => self( )       [1]
     *   : x    => self(x)       [2]
     *
     * [1]: A function call with no parameters
     * [2]: A function call with one parameter
     */
    rt_data_t *(*apply)(void *self, rt_env_t *e, rt_data_t *p);

    /* Destructor. If returns true, the block itself will be freed
     * by the runtime (or should be freed by its enclosing object).
     */
    bool (*dealloc)(void *self);
  } rt_udt_t;

  extern rt_udt_t create_base_udt(const char *type_name);

#ifdef __cplusplus
};
#endif

#endif /* !_C_MP_I_RT_UDT_H__ */
