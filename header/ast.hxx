#ifndef _MP_AST_HXX__
#define _MP_AST_HXX__

// Fwrd decl
class visitor;

namespace syntree
{
  class ast
  {
  public:
    virtual void accept(visitor &v) = 0;
  };
};

#endif /* !_MP_AST_HXX__ */
