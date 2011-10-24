require 'mkmf'

#TODO: Try to include the non-dynamic versions for 2 and 3 dimensions
#TODO: Switch to ffi? Also check out ffi-inliner
#TODO: Can the original c++ be used instead? Try it
create_makefile 'miniball_ruby/miniball_ruby'
