$:.push File.expand_path('../lib', __FILE__)
require 'miniball_ruby/version'

Gem::Specification.new do |s|
  s.name     = %q{miniball_ruby}
  s.version  = Miniball::VERSION
  s.authors  = ['Mike Marion']
  s.email    = %q{mike.marion@gmail.com}
  s.homepage = %q{https://github.com/marionm/miniball_ruby}

  s.summary     = %q{Ruby/C implementation of miniball}
  s.description = %q{This gem finds minimum bounding spheres of a set of points.
                     It is a port of Bernd Gaertner's C++ miniball library.}

  s.files = Dir.glob('lib/**/*.rb')
  s.extensions = ['ext/miniball_ruby/extconf.rb']

  s.add_development_dependency 'yajl-ruby'
end
