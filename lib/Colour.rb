module Rug
  class Colour
    attr_accessor :r, :g, :b, :a
    def initialize r, g, b, a = 255
      @r, @g, @b, @a = r, g, b, a
    end

    Red     = Colour.new 255,   0,   0
    Blue    = Colour.new   0,   0, 255
    Green   = Colour.new   0, 255,   0
    Black   = Colour.new   0,   0,   0
    White   = Colour.new 255, 255, 255
    Cyan    = Colour.new   0, 255, 255
    Magenta = Colour.new 255,   0, 255
    Yellow  = Colour.new 255, 255,   0
  end

  # to remove any confusion about the spelling of colour
  Color = Colour
end
