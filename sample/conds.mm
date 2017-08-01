#
# Description:
#   Ifs and elses
#

{
  @"case 1"   if 0,     # This shouldn't happen
  @"case 2"   if [],    # Also shouldn't happen
  @"case 3"   if @,     # Also shouldn't happen
  @"case 4"   else      # should happen
}
