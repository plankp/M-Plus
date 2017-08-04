#
# Description:
#   Triggers an error and done!
#

try
  error(10)
catch -> a do
  print(@"Errored with message ");
  print(a);
  newline()
end
