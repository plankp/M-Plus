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
  [@"6 + 1", (x -> x + 1)(6)],
  [@"6 + 1", (() -> do	# do-end does not create a new scope!
    plus_1 = x -> x + 1;
    plus_1(6)
  end)()],
  factorial_naive(5)
]
