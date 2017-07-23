#ifndef _MP_CONS_ARR_HXX__
#define _MP_CONS_ARR_HXX__

#include "visitor.hxx"

#include <algorithm>

namespace syntree
{
  class cons_arr : public ast
  {
  public:
    cons_arr() = default;
    ~cons_arr() = default;

    virtual void accept(visitor &v);
    virtual std::string type_name(void) const;
  };
};

#endif /* !_MP_CONS_ARR_HXX__ */
