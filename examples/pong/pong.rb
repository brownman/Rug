require File.dirname(__FILE__) + "/../../lib/Rug"

include Rug

BALL_RADIUS   = 5
PADDLE_HEIGHT = 100
PADDLE_WIDTH  = 10
BALL_SPEED    = 250
PADDLE_SPEED  = 200

class Paddle < AnimatedBody
  def initialize x, y
    super x, y, 0.0, 0.0, 0.0
    self.shape = Physics::Rectangle.new PADDLE_WIDTH, PADDLE_HEIGHT

    @animation = Image.new PADDLE_WIDTH, PADDLE_HEIGHT

    # I use the British spelling of colour here, but Rug can use
    # the American way as well.
    @animation.back_colour = Colour::White
    @animation.fill_rect 0, 0, PADDLE_WIDTH, PADDLE_HEIGHT
  end

  def collide other
    # Stop the paddles when they bump into the top or the bottom.
    case other
    when :top, :bottom
      @vy = 0
      revert_position
    end
  end
end

class Ball < AnimatedBody
  def initialize
    super()
    reset
    self.shape = Physics::Circle.new BALL_RADIUS
    @animation = Image.new BALL_RADIUS * 2, BALL_RADIUS * 2
    @animation.back_colour = Colour::White
    @animation.fill_circle BALL_RADIUS, BALL_RADIUS, BALL_RADIUS
  end

  def collide other
    # This method is called when the ball bumps into something.
    # By default Rug sends :top, :bottom, :left or :right when
    # the object contacts the edge of the screen.
    case other
    when :top, :bottom
      @vy = -@vy
      revert_position
    when :left
      $score2 += 1
      reset
    when :right
      $score1 += 1
      reset
    else
      # Hit something else, which is a paddle
      @vx = -@vx
      revert_position
    end
  end

  def reset
    self.x = Rug.width / 2
    self.y = Rug.height / 2
    self.vx = rand < 0.5 ? -BALL_SPEED : BALL_SPEED
    self.vy = (rand - 0.5) * BALL_SPEED
  end
end

Rug.conf do
  title "Pong in Rug"
end

Rug.load do
  $world = Physics::World.new

  $paddle1 = Paddle.new 10, (Rug.height - PADDLE_HEIGHT) / 2
  $paddle2 = Paddle.new Rug.width - PADDLE_WIDTH - 10, (Rug.height - PADDLE_HEIGHT) / 2

  $ball = Ball.new

  $world << $paddle1 << $paddle2 << $ball

  $score1 = 0
  $score2 = 0
end

Rug.draw do
  text 10, 10, "Score: #{$score1} : #{$score2}"

  $world.each do |obj|
    obj.draw
  end
end

Rug.update do |dt|
  $world.update dt
end

Rug.keydown do |key|
  # Controls:
  # W/S: move left paddle up/down
  # Up/Down: Move right paddle up/down
  case key
  when Key::Escape
    exit
  when Key::W
    $paddle1.vy = -PADDLE_SPEED
  when Key::S
    $paddle1.vy = PADDLE_SPEED
  when Key::Up
    $paddle2.vy = -PADDLE_SPEED
  when Key::Down
    $paddle2.vy = PADDLE_SPEED
  end
end

Rug.keyup do |key|
  case key
  when Key::W, Key::S
    $paddle1.vy = 0.0
  when Key::Up, Key::Down
    $paddle2.vy = 0.0
  end
end

Rug.start
