#include "mp_value.hxx"

namespace rt
{
  mp_value::mp_value(rt::type_tag _t)
    : tag(_t)
  {
  }

  mp_value::mp_value(const rt::mp_value &ref)
    : tag(ref.tag)
  {
  }

  mp_value::mp_value(rt::mp_value &&mref)
    : mp_value()
  {
    swap(*this, mref);
  }

  rt::type_tag
  mp_value::get_type_tag(void) const
  {
    return tag;
  }

  bool
  mp_value::is_truthy(void) const
  {
    // Default all values are true
    return true;
  }

  void
  swap(rt::mp_value &a, rt::mp_value &b)
  {
    using std::swap;

    swap(a.tag, b.tag);
  }
};

std::string
to_string (const rt::type_tag &ref)
{
#define X(n) case rt::type_tag::n: return #n;
  switch (ref)
    { /* Macro expansion here! */ RT_TYPE_TAGS }
#undef X
  // Returns an empty string by default
  return "";
}
