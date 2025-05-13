#!/usr/bin/env ruby

require "yaml"

GCC_IMAGES = {
  4 => "takei/libyu-dev:gcc4",
  5 => "takei/libyu-dev:gcc5",
  6 => "takei/libyu-dev:gcc6",
}
ALPINE_IMAGES = {
  "gcc" => "takei/libyu-dev:alpine3-gcc",
  "clang" => "takei/libyu-dev:alpine3-clang",
}

STANDARDS = [  nil, "c++11", "c++14", "c++17", "c++20", "c++23", ]
GCC_SUPPORTED = {
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
ALPINE_SUPPORTED = {
  "gcc" =>  [ true,    true,    true,    true,    true,    true, ],
  "clang" =>[ true,    true,    true,    true,    true,    true, ],
}
MACOS_SUPPORTED = {
  13 =>     [ true,    true,    true,    true,    true,    true, ],
  14 =>     [ true,    true,    true,    true,    true,    true, ],
  15 =>     [ true,    true,    true,    true,    true,    true, ],
}

OWNER = "takei-yuya"
REPOSITORY = "libyu"
BASE_URL = "https://github.com/#{OWNER}/#{REPOSITORY}/actions/workflows"
WORKFLOW_DIR = "#{File.dirname(__FILE__)}/workflows/"
BADGE_BRANCH = "main"

def gen_yaml(basename, runson, image, standard)
  template = <<~YAML
  name: XXX
  on:
    push:
      branches: [ main ]
    pull_request:
      branches: [ main ]
  jobs:
    build:
      runs-on: XXX
      steps:
      - uses: actions/checkout@v4
      - name: make check
        run: make check
  YAML
  spec = YAML.load(template)
  spec["name"] = basename
  spec["jobs"]["build"]["runs-on"] = runson
  spec["jobs"]["build"]["container"] = { "image" => image } if image
  spec["jobs"]["build"]["steps"][-1]["env"] = { "CXXFLAGS" => "-std=#{standard}" } if standard
  yaml = spec.to_yaml
  write_or_die("#{WORKFLOW_DIR}/#{basename}.yml", yaml)
end

def write_or_die(file, content)
  File.write(file, content)
rescue => e
  STDERR.puts "Failed to write: file=#{file}"
  throw
end

def badge_link(basename)
  workflow_url = "#{BASE_URL}/#{basename}.yml"
  badge_url = "#{workflow_url}/badge.svg?branch=#{BADGE_BRANCH}"
  return "[![#{basename}](#{badge_url})](#{workflow_url})"
end

def gen_gcc_yaml(gcc_version, standard)
  basename = "gcc#{gcc_version}-#{standard || "default"}"
  image = GCC_IMAGES[gcc_version] || "gcc:#{gcc_version}"

  gen_yaml(basename, "ubuntu-latest", image, standard)
  return basename
end

def gen_alpine_yaml(compiler, standard)
  basename = "alpine-#{compiler}-#{standard || "default"}"
  image = ALPINE_IMAGES[compiler] or throw "Unknown compiler for alpine"

  gen_yaml(basename, "ubuntu-latest", image, standard)
  return basename
end

def gen_mac_yaml(mac_version, standard)
  basename = "macos#{mac_version}-#{standard || "default"}"
  runson = "macos-#{mac_version}"
  cxxflags = standard ? "-std=#{standard}" : ""

  gen_yaml(basename, runson, nil, standard)
  return basename
end

GCC_SUPPORTED.each do |gcc_version, supported_row|
  row = []
  row << "gcc-#{gcc_version}"
  STANDARDS.zip(supported_row).each do |standard, supported|
    if supported
      basename = gen_gcc_yaml(gcc_version, standard)
      row << badge_link(basename)
    else
      row << "-"
    end
  end
  puts "| #{row.join(" | ")} |"
end

ALPINE_SUPPORTED.each do |compiler, supported_row|
  row = []
  row << "alpine-#{compiler}"
  STANDARDS.zip(supported_row).each do |standard, supported|
    if supported
      basename = gen_alpine_yaml(compiler, standard)
      row << badge_link(basename)
    else
      row << "-"
    end
  end
  puts "| #{row.join(" | ")} |"
end

MACOS_SUPPORTED.each do |mac_version, supported_row|
  row = []
  row << "macos-#{mac_version}"
  STANDARDS.zip(supported_row).each do |standard, supported|
    if supported
      basename = gen_mac_yaml(mac_version, standard)
      row << badge_link(basename)
    else
      row << "-"
    end
  end
  puts "| #{row.join(" | ")} |"
end
