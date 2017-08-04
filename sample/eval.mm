#
# Description:
#   Making things ugly-complicated! Again!
#

delegate_eval =
  (env, expr) -> do
    print(@Evaluating); print(expr); newline();
    (&eval' : env : expr : ())()
  end;

delegate_apply =
  (base, plist) ->
    (&apply' : base : plist : ())();

b = 10;
delegate_eval(current_env(), &(b + 1))
