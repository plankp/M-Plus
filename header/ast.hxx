#ifndef _MP_AST_HXX__
#define _MP_AST_HXX__

#include <string>

// Fwrd decl
class visitor;

namespace syntree
{
  class ast
  {
  public:
    virtual void accept(visitor &v) = 0;
    virtual std::string type_name(void) const = 0;
  };
};

#endif /* !_MP_AST_HXX__ */
