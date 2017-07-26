#ifndef _MP_UDEF_FUNC_HXX__
#define _MP_UDEF_FUNC_HXX__

#include "ast.hxx"
#include "function.hxx"

#include <map>
#include <memory>
#include <string>
#include <algorithm>

namespace rt
{
  class udef_func : public function
  {
  private:
    std::map<std::string, std::shared_ptr<rt::mp_value>> locals;
    std::string param_name;
    std::shared_ptr<rt::mp_value> body;

    udef_func() = default;

  public:
    udef_func(std::string param_name,
	      std::shared_ptr<rt::mp_value> body,
	      std::map<std::string, std::shared_ptr<rt::mp_value>> = {});
    udef_func(const rt::udef_func &ref);
    udef_func(rt::udef_func &&mref);
    virtual ~udef_func() = default;

    rt::udef_func &operator=(rt::udef_func obj);

    virtual std::unique_ptr<rt::mp_value> clone(void) const;
    virtual std::string to_str(void) const;
    virtual std::unique_ptr<rt::mp_value> on_call(void);
    virtual std::unique_ptr<rt::mp_value> on_call(std::unique_ptr<rt::mp_value> param);

    friend void swap(rt::udef_func &a, rt::udef_func &b);
  };
};

#endif /* !_MP_UDEF_FUNC_HXX__ */
