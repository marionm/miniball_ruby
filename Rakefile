DATA_DIR = File.expand_path('../test/data', __FILE__)
INPUTS = "#{DATA_DIR}/inputs.json"
EXPECTATIONS = "#{DATA_DIR}/expectations.json"
GEN_DIR = "#{DATA_DIR}/generation"

require "#{GEN_DIR}/generator"
require 'fileutils'

require 'bundler'
Bundler::GemHelper.install_tasks

task :default => :test

task :test => ['test:data:generate'] do
  Dir[File.expand_path('../test/test_*.rb', __FILE__)].each do |test|
    require test
  end
end

namespace :test do
  namespace :data do

    def generate(max_points, max_dim, force = false)
      if force || !File.exists?(INPUTS) || !File.exists?(EXPECTATIONS)
        Dir.chdir(GEN_DIR) do
          `g++ generator.cpp -o generator`
          Generator.generate(max_points, max_dim, INPUTS, EXPECTATIONS)
        end
      end
    end

    desc "Generate test data (assumes g++ is installed)"
    task :generate, [:max_points, :max_dim] do |t, args|
      args.with_defaults(:max_points => 10, :max_dim => 10)
      generate args[:max_points].to_i, args[:max_dim].to_i
    end

    namespace :generate do
      task :force, [:max_points, :max_dim] do |t, args|
      args.with_defaults(:max_points => 10, :max_dim => 10)
        generate args[:max_points].to_i, args[:max_dim].to_i, true
      end
    end

  end
end
