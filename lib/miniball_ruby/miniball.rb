require 'miniball_ruby/miniball_ruby'

class Miniball
  include MiniballC

  def initialize(points)
    @result = nil
    @points = points
  end

  def calculate(with_analysis = false)
    @result ||= calc(@points, with_analysis || false)
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

  def support_points
    calculate.support_points
  end

  #Must calculate with analytics on to get accuracy or slack

  def accuracy
    unless @accuracy
      a = calculate.accuracy
      @accuracy = a.nan? ? 0.0 : a
    end
    @accuracy
  end

  def slack
    unless @slack
      s = calculate.slack
      @slack = (s == false ? 0.0 : slack)
    end
    @slack
  end
end
