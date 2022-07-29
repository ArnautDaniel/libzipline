require 'ffi'

module Rzipline
  extend FFI::Library
  def self.load_lib(libpath = __dir__ + '/libzipline.so')
    ffi_lib libpath
    import_symbols()
  end

  def self.import_symbols()
    attach_function :zipline_create, [:string], :void
  end

  def self.create(pathname)
    zipline_create(pathname)
  end
end
