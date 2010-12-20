module Rug
  module Physics
    class Circle
      attr_accessor :body, :radius
      def initialize body, r; @body, @radius = body, r; end

      def overlap shape
        if shape.is_a? Circle
          dx = body.x - shape.x
          dy = body.y - shape.y
          dr = @radius + shape.radius

          dx * dx + dy * dy <= dr * dr
        end
      end

      def x; body.x; end
      def y; body.y; end
    end

    class World
      # ppm is pixels_per_metre
      # shape is a shape object
      attr_accessor :ppm, :shape

      def initialize
        # TODO: come up with a better data structure for objects
        @objects = Array.new
        @ppm = 30.0
        @gravity = 9.8
      end

      def << obj
        @objects << obj
        obj.world = self
      end

      def update dt
        @objects.each do |obj|
          obj.update dt

          obj.apply_force 0, -@gravity * obj.mass * dt / 1000.0
        end
      end

      def each &b
        @objects.each &b
      end

      def collision? which
        obj = @objects.find do |o|
          o != which and o.overlap which
        end

        if obj
          collision.call obj, which
        else
          false
        end
      end

      def remove body
        @objects.delete body
      end

      # sets a function to call when a collision occurs
      #
      # the function must return true if the collision should stop
      # the objects from moving, false if not
      def collision &func
        @collision_func = func
      end

      def to_world_coords x, y = nil
        x, y = x if y == nil
        [x / @ppm, (Rug.height - y) / @ppm]
      end

      def to_screen_coords x, y = nil
        x, y = x if y == nil
        [x * @ppm, Rug.height - y * @ppm]
      end
    end

    class Body
      attr_accessor :world, :shape
      attr_reader :mass, :x, :y, :vx, :vy

      # Create a new body with a specified mass, location, and velocity
      # The mass is in kg, x and y are in pixels, and velocities are in
      # metres per second.
      def initialize world, mass, xy, vxy = [0.0, 0.0]
        @world = world
        @x, @y = xy
        @vx, @vy = vxy.map(&:to_f)
        @mass = mass

        world << self
      end

      def update dt
        x, y = @x, @y
        @x += @vx * dt / 1000.0
        @y += @vy * dt / 1000.0

        if @world.collision? self
          @x, @y = x, y
        end
      end

      def apply_force fx, fy
        return if @mass == 0 # massless particles aren't affected by mass
        @vx += fx / @mass
        @vy += fy / @mass
      end

      def shape= shape
        shape.body = self
        @shape = shape
      end

      def overlap body
        @shape.overlap body.shape
      end

      def screen_coords
        @world.to_screen_coords @x, @y
      end
    end
  end
end
