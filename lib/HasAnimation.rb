module Rug
  module HasAnimation
    def draw x_offset = 0.0, y_offset = 0.0, layer = nil, &custom_render
      @animation.draw @x + x_offset, @y + y_offset, layer, &custom_render
    end

    def update_animation dt
      @animation.update
    end

    # This is typically overidden by classes that include this module,
    # but put it here anyway
    def update dt
      update_animation dt
    end
  end

  class Image
    # this is added so that we can put images into the HasAnimation class
    def update; end
  end
end
