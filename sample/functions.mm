#
# Description:
#   All about functions and applications
#

factorial_naive =	# Definitions are recursive!
  x -> {
    1                            if x < 2,
    x * factorial_naive(x - 1)   else
  }
;

[
  [@"2 + 3", ((a, b) -> a + b)(2, 3)],
  [@"6 + 1", (() -> do	# do-end does not create a new scope!
    plus_1 = x -> x + 1;
    plus_1(6)
  end)()],
  [@"factorial of 5", factorial_naive(5)]
]
