require 'test/unit'

require File.expand_path('../../lib/miniball', __FILE__)

class EdgeCaseTest < Test::Unit::TestCase
  def test_empty_set
    begin
      Miniball.new([]).calculate
      fail 'Should have raised an ArgumentError'
    rescue ArgumentError
      #Pass
    end
  end

  def test_empty_point
    m = Miniball.new([[]])
    assert_equal [], m.center
    assert_equal 0.0, m.radius
  end

  def test_empty_points
    m = Miniball.new([[],[]])
    assert_equal [], m.center
    assert_equal 0.0, m.radius
  end

  def test_single_point
    point = [1,2,3]
    m = Miniball.new([point])
    assert_equal point, m.center
    assert_equal 0.0, m.radius
  end

  def test_inconsistent_dimensions
    begin
      Miniball.new([[1],[2,2]]).calculate
      fail 'Should have raised an ArgumentError'
    rescue ArgumentError
      #Pass
    end
  end

  def test_invalid_parameter
    begin
      Miniball.new('not an array').calculate
      fail 'Should have thrown a TypeError'
    rescue TypeError
      #Pass
    end
  end

  def test_invalid_parameters
    begin
      Miniball.new(['not an inner array']).calculate
      fail 'Should have thrown a TypeError'
    rescue TypeError
      #Pass
    end
  end
end
