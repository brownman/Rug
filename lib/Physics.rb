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
      line_circle_intersect(c, r.x, r.y, r.x, r.y + r.h) or  # left side
      line_circle_intersect(c, r.x, r.y, r.x + r.w, r.y) or  # top side
      line_circle_intersect(c, r.x, r.y + r.h, r.x + r.w, r.y + r.h) or # bottom side
      line_circle_intersect(c, r.x + r.w, r.y, r.x + r.w, r.y + r.h)    # right side
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

    def self.line_circle_intersect c, x1, y1, x2, y2
      if point_in_circle(c, x1, y1) or point_in_circle(c, x2, y2)
        true
      else
        dx = x1 - x2
        dy = y1 - y2
        fx = c.x - x1
        fy = c.y - y1

        a = dx * dx + dy * dy
        b = 2 * (fx * dx + fy * dy)
        c = (fx * fx + fy * fy) - c.radius * c.radius

        disc = b * b - 4 * a * c

        if disc < 0
          false
        else
          disc = Math.sqrt disc
          t1 = (-b + disc) / (2 * a)
          t2 = (-b - disc) / (2 * a)

          t1 >= 0 and t1 <= 1 and t2 >= 0 and t2 <= 1
        end
      end
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
      def initialize r; @radius = r; end

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

      def initialize w, h; @w, @h = w, h; end

      def overlap shape
        if shape.is_a? Circle
          Physics.collide_circle_rect shape, self
        elsif shape.is_a? Rectangle
          Physics.collide_rect_rect self, shape
        end
      end
    end

    class World
      attr_accessor :gravity

      def initialize
        # TODO: come up with a better data structure for objects
        @objects = Array.new
        @gravity = 200.0
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
          which.collide obj
          obj.collide which
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
    end

    class Body
      attr_accessor :world, :shape, :x, :y, :vx, :vy, :mass

      # Create a new body with a specified mass, location, and velocity
      # The mass is in kg, x and y are in pixels, and velocities are in
      # metres per second.
      def initialize mass, x, y, vx = 0.0, vy = 0.0
        @x, @y = x, y
        @vx, @vy = vx.to_f, vy.to_f
        @mass = mass
        @collision_func = nil
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

      def collide other = nil, &func
        if func != nil
          @collision_func = func
        elsif @collision_func != nil
          @collision_func.call other
        end
      end

      def stop
        @vx = @vy = 0
      end

      def position
        [@x, @y]
      end
    end
  end
end
