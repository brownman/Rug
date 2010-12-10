class Animation
  class Frameset
    attr_reader :image, :frame_width, :timing, :width, :height, :num_frames
    def initialize args = {}
      @image = args[:image]
      @frame_width = args[:frame_width]
      @timing = args[:timing]
      @num_frames = @image.width / @frame_width

      @width = @image.width
      @height = @image.height
    end
  end

  def initialize animations = {}
    @framesets = {}
    default = :idle
    animations.each do |name, data|
      img = Rug::Image.new data[:filename]
      data[:frame_width] = 1 if data[:frame_width] == 0
      @framesets[name] = Frameset.new(
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
    @current_frame = 0
    @last_update = Rug.get_time
  end

  def update
    now = Rug.get_time
    if @last_update + @current_frameset.timing[@current_frame] < now
      @last_update = now
      @current_frame = (@current_frame + 1) % @current_frameset.num_frames
    end
  end

  def animation= which
    if @framesets.key? which
      @current_frameset = @framesets[which]
      @current_frame = 0
    else
      raise "No frameset named #{which}!"
    end
  end

  def draw x, y, layer = nil
    # if a block is passed, transform the image
    img = 
      if block_given?
        yield @current_frameset.image
      else
        @current_frameset.image
      end

    img.draw x, y, @current_frameset.frame_width, img.height,
      @current_frameset.frame_width * @current_frame, 0, layer
  end
end
