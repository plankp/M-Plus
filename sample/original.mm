#
# Description:
#   The original sample used to test the syntax tree and the parser
#

f = x -> x + 2 * -3^2;
g = x -> 3 * x;
f(10);

a = 0; b = 0;

do
  10 + 2 * -3^2 : (f . g)(3) : &&a() : (c = 0) : [];
  print(@DONE)
end;

a <- {
  10   if a,
  11   if b,
  12   if a == b or b == c or a == c,
  13   else
};

(a) -> (a);
(a, b) -> a + b;
(a, b, c) -> a + b + c;
(() -> do
  [];
  [1, 2, 3];
  a:[b]
end)()
