#include "cons_arr.hxx"

namespace syntree
{
  void
  cons_arr::accept(visitor &v)
  {
    v.visit_cons_arr(*this);
  }
};
