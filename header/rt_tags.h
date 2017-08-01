#ifndef _C_MP_RT_TAGS_H__
#define _C_MP_RT_TAGS_H__

#ifdef __cplusplus
extern "C"
{
#endif

#define RT_TAGS					\
  X(CHAR)					\
  X(INT)					\
  X(FLOAT)					\
  X(ATOM)					\
  X(STR)					\
  X(ERR)					\
  X(FUNC)					\
  X(ENV)					\
  X(ARRAY)					\
  X(LIST)					\
  X(UDT)

#define X(n) n,
  typedef enum rt_tag_t { RT_TAGS } rt_tag_t;
#undef X

  typedef union rt_data_t rt_data_t;

#ifdef __cplusplus
};
#endif

#endif /* !_C_MP_RT_TAGS_H__ */
