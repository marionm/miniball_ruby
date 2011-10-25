require 'rubygems'
require 'yajl'

class Generator
  def self.generate(max_points, max_dim, inputs_file, expectations_file)
    inputs = []
    expected = []

    (1..max_points).each do |count|
      (1..max_dim).each do |dim|
        points = []

        count.times do
          point = []
          dim.times do
            point << rand * 100 - 200
          end
          points << point;
        end

        inputs << points
        result = nil
        args = points.map { |p| p.join(' ') }.join(' ')
        Dir.chdir(File.dirname(__FILE__)) do
          result = `./generator #{dim} #{args}`
        end
        expected << result.gsub('-nan','0')

      end
    end

    File.open(inputs_file, 'w') do |f|
      f.write Yajl::Encoder.encode(inputs)
    end

    File.open(expectations_file, 'w') do |f|
      f.write "[#{expected.join(',')}]"
    end
  end
end
