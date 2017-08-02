#
# Description:
#   Ifs and elses
#

a = {
  @"case 1"   if 0,     # This shouldn't happen
  @"case 2"   if [],    # Also shouldn't happen
  @"case 3"   if @,     # Also shouldn't happen
  @"case 4"   else      # should happen
};
a <- [
  a,
  { @"case 5" if 1 == 2 or &1 == 1 },
  { @"case 6" if @A == @"A" and &[] == [] }
];
a                       # should be [@"case 4", @"case 5", @"case 6"]
