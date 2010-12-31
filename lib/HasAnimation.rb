module Rug
  module HasAnimation
    def draw layer = nil
      @animation.draw @x, Rug.height - @y, layer
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
    def update dt; end
  end
end
