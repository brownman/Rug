class Animation
  class Frameset
    attr_reader :image, :frame_width, :timing, :width, :height
    def initialize args = {}
      @image = args[:image]
      @frame_width = args[:frame_width]
      @timing = args[:timing]

      @width = @image.width
      @height = @image.height
    end
  end

  def initialize animations = {}
    @framesets = {}
    default = :idle
    animations.each do |name, data|
      img = Rug::Image.new data[:filename]
      @frameset[name] = Frameset.new(
        :image => img,
        :frame_width => data[:frame_width],
        :timing => data[:timing]
      )
      if data[:default] == true
        default = name
      end
    end

    raise "No default animation set!" if !@framesets.key?(default)

    @current_frameset = @framesets[default]
  end

  def update
  end

  def animation= which
  end

  def draw x, y, layer = nil
  end
end
