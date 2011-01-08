module Rug
  # The scroll view is used to scroll across a layer that is larger than the window.
  # It will automatically handle scrolling via arrow keys.
  class ScrollView
    DEFAULT_SCROLL_SPEED = 20

    attr_reader :layer
    attr_accessor :x, :y, :scroll_speed

    # Two overloads:
    # initialize(width, height, auto_inputs = true)
    # initialize(filename, auto_inputs = true)
    #
    # If a filename is passed, it is used as a background image. The width and height
    # of the view will be the size of the background image.
    #
    # If auto_inputs is true, then the view will automatically register key-handling
    # behaviour.
    def initialize arg1, arg2 = nil, arg3 = nil
      width, height, auto_inputs, background_image =
        if arg1.is_a? String
          img = Rug::Image.new arg1
          [img.width, img.height, arg2 != false, img]
        else
          [arg1, arg2, arg3 != false, nil]
        end

      @layer = background_image || Rug::Layer.new(width, height)

      @x, @y = 0, 0
      @width, @height = width, height
      @scroll_speed = DEFAULT_SCROLL_SPEED

      if auto_inputs
        Rug.keydown do |key|
          case key
          when Rug::Key::Up
            scroll 0, -@scroll_speed
          when Rug::Key::Down
            scroll 0, @scroll_speed
          when Rug::Key::Left
            scroll -@scroll_speed, 0
          when Rug::Key::Right
            scroll @scroll_speed, 0
          end
        end
      end
    end

    def scroll dx, dy
      @x += dx if @x + dx >= 0 and @x + dx + Rug.width <= @width
      @y += dy if @y + dy >= 0 and @y + dy + Rug.height <= @height
    end

    def draw
      @layer.draw 0, 0, Rug.width, Rug.height, @x, @y
    end
  end

  class WatcherView < ScrollView
    # There are two overloads here:
    # initialize(body, width, height)
    # initialize(body, background_image)
    def initialize body, arg1, arg2 = nil
      @body = body

      if arg2 == nil
        super arg1, false
      else
        super arg1, arg2, false
      end
    end

    def update
      # Center the view on the body, but bound it by the edges of the image
      @x = [
        [0, @body.x - (Rug.width - @body.width) / 2].max,
        @layer.width - Rug.width
      ].min

      @y = [
        [0, @body.y - (Rug.height - @body.height) / 2].max,
        @layer.height - Rug.height
      ].min
    end
  end
end
