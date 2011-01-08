require File.dirname(__FILE__) + "/../../lib/Rug"

include Rug

JUMP_FORCE = -300.0
BEAR_SPEED = 100.0

# Create a simple class to represent the platforms that the
# bear will jump on
class Platform < AnimatedBody
  def initialize x, y
    super x, y
    @animation = $platform_image
    self.shape = Physics::Rectangle.new(@animation.width, @animation.height)
  end
end

class Bear < AnimatedBody
  def initialize x, y
    # Give the bear a mass so that gravity will apply to it
    super x, y, 0.0, 0.0, 1.0
    @animation = $bear_animation
    self.shape = Physics::Rectangle.new(@animation.width, @animation.height)

    # We need to keep track of when the bear is in the air, otherwise we'd be
    # able to jump while we're in the air!
    @in_air = true
  end

  def collide other
    # if we've bumped into a platform, stop moving
    revert_position :y
    @vy = 0
    @in_air = false
  end

  def move dir
    case dir
    when :left, :right
      @facing = dir
      @vx = dir == :left ? -BEAR_SPEED : BEAR_SPEED
      set_animation :walking
    else
      @vx = 0.0
      @facing = nil
      set_animation :idle
    end
  end

  def jump
    unless @in_air
      self.apply_force 0.0, JUMP_FORCE
      @in_air = true
    end
  end

  def set_animation which
    @animation.animation = which
  end

  def draw x_offset, y_offset
    # We need to include an x and y offset since the WatcherView
    # has a view offset based on where the viewport is in the world.
    if @facing == :left
      # flip the animation if we are moving left. This is done by passing
      # a block to the parent's draw method, which is an image transformation
      # function.
      super &:flip_h
    else
      super
    end
  end
end

Rug.conf do
  title      "Side Scroller in Rug"
end

Rug.load do
  $platform_image = Image.new "platform.png"

  # In this example the animation has two frame sets: idle and walking, so we
  # specify both of them here.
  $bear_animation = Animation.new :idle => {
      :filename => "bear_idle.png",
      :timing => 500
    },
    :walking => {
      :filename => "bear_walk.png",
      :timing => [166, 333] # Here since the different frames have different timings,
                            # we can pass an array of milliseconds
    }

  $world = Physics::World.new

  # By default, objects collide with the edge of the window. Let's disable this
  # for the side scroller.
  $world.collide_with_window = false

  $platforms = [
    [200, 400],
    [500, 300],
    [800, 350],
    [1100, 450],
    [1400, 300]
  ].map { |(x, y)| Platform.new(x, y) }

  $bear = Bear.new 200 + $platform_image.width / 2, 400 - $bear_animation.height - 50

  $world << $bear
  
  $platforms.each do |p|
    $world << p
  end

  # We use the WatcherView class to keep track of our view port. This class follows a
  # Body object as it moves throughout the world, but is bounded by the edges of a
  # specified image - in this case, background.png.
  $view = WatcherView.new $bear, "background.png"
end

Rug.draw do
  $view.draw
  $world.each do |b|
    # Draw with an offset based on where the view is
    b.draw -$view.x, -$view.y
  end
end

Rug.update do |dt|
  $world.update dt
  $view.update
end

Rug.keydown do |key|
  case key
  when Key::Escape
    exit
  when Key::Left
    $bear.move :left
  when Key::Right
    $bear.move :right
  when Key::Space
    $bear.jump
  end
end

Rug.keyup do |key|
  case key
  when Key::Left, Key::Right
    $bear.move nil
  end
end

Rug.start
