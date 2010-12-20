module Rug
  module Physics
    def self.collide_circle_circle c1, c2
      dx = c1.x - c2.x
      dy = c1.y - c2.y
      dr = c1.radius + c2.radius

      dx * dx + dy * dy <= dr * dr
    end

    def self.collide_circle_rect c, r
      point_in_rect(r, c.x, c.y) or
      point_in_circle(c, r.x, r.y) or
      point_in_circle(c, r.x, r.y + r.h) or
      point_in_circle(c, r.x + r.w, r.y) or
      point_in_circle(c, r.x + r.w, r.y + r.h)
    end

    def self.collide_rect_rect r1, r2
      left = r1.x < r2.x ? r1 : r2
      right = r1.x < r2.x ? r2 : r1
      top = r1.h < r2.h ? r1 : r2
      bottom = r1.h < r2.h ? r2 : r1

      left.x + left.w >= right.x and top.y + top.h >= bottom.y
    end

    def self.point_in_circle c, px, py
      dx = c.x - px
      dy = c.y - py

      dx * dx + dy * dy <= c.radius * c.radius
    end

    def self.point_in_rect r, px, py
      px >= r.x and
      px <= r.x + r.w and
      py >= r.y and
      py <= r.y + r.h
    end

    class Shape
      attr_accessor :body
      def initialize body
        @body = body
        body.shape = self
      end
      def x; body.x; end
      def y; body.y; end
    end

    class Circle < Shape
      attr_accessor :radius
      def initialize body, r; super body; @radius = r; end

      def overlap shape
        if shape.is_a? Circle
          Physics.collide_circle_circle self, shape
        elsif shape.is_a? Rectangle
          Physics.collide_circle_rect self, shape
        end
      end
    end

    class Rectangle < Shape
      attr_accessor :w, :h

      def initialize body, w, h; super body; @w, @h = w, h; end

      def overlap shape
        if shape.is_a? Circle
          Physics.collide_circle_rect shape, self
        elsif shape.is_a? Rectangle
          Physics.collide_rect_rect self, shape
        end
      end
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
          @collision_func.call obj, which
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

        world << self unless world == nil
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