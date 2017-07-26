#ifndef _MP_AST_HXX__
#define _MP_AST_HXX__

#include "mp_value.hxx"

#include <string>

// Fwrd decl
class visitor;

namespace syntree
{
  class ast : public rt::mp_value
  {
  public:
    virtual void accept(visitor &v) = 0;
    virtual std::string type_name(void) const = 0;
    virtual std::string to_str(void) const
    {
      return type_name();
    };
  };
};

#endif /* !_MP_AST_HXX__ */
