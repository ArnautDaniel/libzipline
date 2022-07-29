lib = File.expand_path('../lib', __FILE__)
$LOAD_PATH.unshift(lib) unless $LOAD_PATH.include?(lib)


Gem::Specification.new do |s|
  s.name        = "rzipline"
  s.version     = "0.0.4"
  s.summary     = "Bindings to libzipline"
  s.description = "Allows for the creation of zipline packages."
  s.authors     = ["Arnaut Daniel"]
  s.email       = "jack@occiro.com"
  s.require_paths = ["lib"]
  s.add_runtime_dependency "ffi", "~> 1.15"
  s.homepage    =
    "https://rubygems.org/gems/rzipline"
  s.license       = "MIT"
  
  s.files = Dir.glob("lib/*")
end
