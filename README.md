## miniball_ruby

**miniball_ruby** is a Ruby gem for calculating the minimum bounding sphere of a set of points in various dimensions.

It is a port of Bernd Gaertner's C++ library. The original can be found at http://www.inf.ethz.ch/personal/gaertner/miniball.html.

## Usage

*Gemfile*

```ruby
gem 'miniball_ruby', :require => 'miniball'
```

*Code*

```ruby
points = [
  [3.1, 1.4, 9.2],
  [2.7, 1.8, 2.8],
  [6.0, 2.2, 1.4]
]

miniball = Miniball.new(points)

miniball.center # => [4.55, 1.8, 5.3]
miniball.radius # => 4.18...
```

## Known issues

* Providing an array with fewer than three elements to Miniball results in a segfault during calculation
* Mixed point dimensions gives an ugly error

## License

(This is a derivative work of the original (copyright Bernd Gaertner), which is licensed under the GPL. Therefore, this must be as well.)

Copyright (C) 2011, Mike Marion

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA,
or download the License terms from prep.ai.mit.edu/pub/gnu/COPYING-2.0.
