require 'rubygems'
require 'rake/gempackagetask'

spec = Gem::Specification.new do |s|
  s.name         = "Rug"
  s.version      = "0.0.2"
  s.author       = "Rob Britton"
  s.email        = "rob.s.brit@gmail.com"
  s.platform     = Gem::Platform::RUBY
  s.summary      = "A simple game framework."
  s.files        = FileList["Rakefile", "lib/*.rb", "ext/*.{c,h,rb}", "README"].to_a
  s.require_path = "lib"
  s.has_rdoc     = false
  s.extensions   = ["ext/extconf.rb"]
  s.homepage     = "https://github.com/robbrit/Rug"
  s.description  = "A simple Ruby gaming library."
end

Rake::GemPackageTask.new(spec) do |pkg|
  pkg.need_tar = true
end
