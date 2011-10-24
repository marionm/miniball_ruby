require 'miniball_ruby/miniball_ruby'

class Miniball
  include MiniballC

  def initialize(points)
    @result = nil
    @points = points
  end

  def calculate(with_analysis = false)
    @result ||= calc(@points)
  end

  def center
    calculate.center
  end

  def radius
    @radius ||= radius_squared ** 0.5
  end

  def radius_squared
    calculate.radius_squared
  end
end
