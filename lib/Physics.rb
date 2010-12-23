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

          obj.apply_force 0, @gravity * obj.mass * dt / 1000.0
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
    end

    module Body
      attr_accessor :world, :shape, :mass, :x, :y, :vx, :vy

      def update_body dt
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

      def collide other
        # TODO: don't completely stop, but stop in the direction that the object is blocked
        true
      end

      # This is typically overidden by classes that include this module,
      # but put it here anyway
      def update dt
        update_body dt
      end
    end
  end
end
