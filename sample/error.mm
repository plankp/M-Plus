#
# Description:
#   Triggers an error and done!
#

try
  try
    10 / 0    # Error thrown here
  catch -> a
    error(a)  # Re-throw
catch -> a do
  print(@"Errored with message ");
  print(a);
  newline()
end
