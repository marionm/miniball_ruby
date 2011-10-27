require 'test/unit'
require 'rubygems'
require 'yajl'

require File.expand_path('../../lib/miniball', __FILE__)

class ComparisonTest < Test::Unit::TestCase
  def setup
    inputs_json = IO.read(File.expand_path('../data/inputs.json', __FILE__))
    expectations_json = IO.read(File.expand_path('../data/expectations.json', __FILE__))

    @inputs = Yajl::Parser.parse(inputs_json)
    @expectations = Yajl::Parser.parse(expectations_json)
    @delta = 0.01
  end

  def parse_point(str)
    str.tr('() ','').split(',').map { |f| f.to_f }
  end

  def assert_point_in_delta(expected, actual, delta = @delta)
    expected = parse_point(expected) if expected.is_a?(String)
    assert_equal expected.length, actual.length
    expected.each_with_index do |coord, i|
      assert_in_delta coord, actual[i], delta
    end
  end

  def test_against_original
    @inputs.each_with_index do |points, i|
      begin
        exp = @expectations[i]
        miniball = Miniball.new(points)

        assert_point_in_delta exp['center'], miniball.center
        assert_in_delta exp['squared_radius'], miniball.radius_squared, @delta
        assert_in_delta exp['squared_radius'] ** 0.5, miniball.radius, @delta
        exp['support_points'].each_with_index do |exp_support_point, i|
          assert_point_in_delta exp_support_point, miniball.support_points[i]
        end
        #TODO: Optional output
      rescue => e
        raise "Failure on input #{i}: #{e.message}"
      end
    end
  end
end
