#!/usr/bin/env ruby

IMAGES = {
  4 => "takei/libyu-dev:gcc4",
  5 => "takei/libyu-dev:gcc5",
  6 => "takei/libyu-dev:gcc6",
}
STANDARDS = [  nil, "c++11", "c++14", "c++17", "c++20", "c++23", ]
SUPPORTED = {
  4  =>     [false,    true,    true,   false,   false,   false, ],
  5  =>     [false,    true,    true,    true,   false,   false, ],
  6  =>     [ true,    true,    true,    true,   false,   false, ],
  7  =>     [ true,    true,    true,    true,   false,   false, ],
  8  =>     [ true,    true,    true,    true,   false,   false, ],
  9  =>     [ true,    true,    true,    true,   false,   false, ],
  10 =>     [ true,    true,    true,    true,    true,   false, ],
  11 =>     [ true,    true,    true,    true,    true,    true, ],
  12 =>     [ true,    true,    true,    true,    true,    true, ],
  13 =>     [ true,    true,    true,    true,    true,    true, ],
  14 =>     [ true,    true,    true,    true,    true,    true, ],
  15 =>     [ true,    true,    true,    true,    true,    true, ],
}
BASE_URL = "https://github.com/takei-yuya/libyu/actions/workflows"
WORKFLOW_DIR = "#{File.dirname(__FILE__)}/workflows/"

def gen_yaml(gcc_version, standard)
  image = IMAGES[gcc_version] || "gcc:#{gcc_version}"
  cxxflags = standard ? "-std=#{standard}" : ""
  basename = "gcc#{gcc_version}-#{standard || "default"}"

  yaml = <<~YAML
  name: gcc-#{gcc_version} #{standard || "default"}
  on:
    push:
      branches: [ main ]
    pull_request:
      branches: [ main ]
  jobs:
    build:
      runs-on: ubuntu-latest
      container:
        image: #{image}
      steps:
      - uses: actions/checkout@v3
      - name: make check
        run: make check CXXFLAGS=#{cxxflags}
  YAML
  File.write("#{WORKFLOW_DIR}/#{basename}.yml", yaml)
  return basename
end

SUPPORTED.each do |gcc_version, supported_row|
  row = []
  row << "gcc-#{gcc_version}"
  STANDARDS.zip(supported_row).each do |standard, supported|
    if supported
      basename = gen_yaml(gcc_version, standard)
      workflow_url = "#{BASE_URL}/#{basename}.yml"
      badge_url = "#{workflow_url}/badge.svg"
      row << "[![#{basename}](#{badge_url})](#{workflow_url})"
    else
      row << "-"
    end
  end
  puts "| #{row.join(" | ")} |"
end

p WORKFLOW_DIR
