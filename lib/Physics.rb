module Rug
  module Physics
    # A circle-circle intersection algorithm
    # This works by seeing if the distance between
    # the two centres is greater than the sum of the
    # radii or not
    def self.collide_circle_circle c1, c2
      dx = c1.x - c2.x
      dy = c1.y - c2.y
      dr = c1.radius + c2.radius

      dx * dx + dy * dy <= dr * dr
    end

    # A circle-rectangle intersection algorithm
    # There is an intersection if the centre of the circle
    # is in the rectangle, or if one of the sides of the
    # rectangle intersects the circle
    def self.collide_circle_rect c, r
      point_in_rect(r, c.x, c.y) or
      line_circle_intersect(c, r.x, r.y, r.x, r.y + r.h) or  # left side
      line_circle_intersect(c, r.x, r.y, r.x + r.w, r.y) or  # top side
      line_circle_intersect(c, r.x, r.y + r.h, r.x + r.w, r.y + r.h) or # bottom side
      line_circle_intersect(c, r.x + r.w, r.y, r.x + r.w, r.y + r.h)    # right side
    end

    # A rectangle-rectangle intersection algorithm
    def self.collide_rect_rect r1, r2
      left = r1.x < r2.x ? r1 : r2
      right = r1.x < r2.x ? r2 : r1
      top = r1.h < r2.h ? r1 : r2
      bottom = r1.h < r2.h ? r2 : r1

      left.x + left.w >= right.x and top.y + top.h >= bottom.y
    end

    # A point-in-circle algorithm
    # Checks the distance between the point and the centre against the radius
    def self.point_in_circle c, px, py
      dx = c.x - px
      dy = c.y - py

      dx * dx + dy * dy <= c.radius * c.radius
    end

    # A point-in-rectangle algorithm
    def self.point_in_rect r, px, py
      px >= r.x and
      px <= r.x + r.w and
      py >= r.y and
      py <= r.y + r.h
    end

    # A line-circle intersection algorithm
    # If one of the endpoints is in the circle then it intersects,
    # otherwise it finds the point on the line closest to the centre.
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

    # This is a base class used for collision detection.
    class Shape
      attr_accessor :body
      def initialize body
        @body = body
        body.shape = self
      end
      def x; body.x; end
      def y; body.y; end
      def check_edge; nil; end
    end

    class Circle < Shape
      attr_accessor :radius
      def initialize r; @radius = r; end

      def overlap? shape
        if shape.is_a? Circle
          Physics.collide_circle_circle self, shape
        elsif shape.is_a? Rectangle
          Physics.collide_circle_rect self, shape
        else
          shape.overlap? self
        end
      end

      # Circle uses centre for x, y instead of top-left
      def x; body.x + @radius; end
      def y; body.y + @radius; end

      def check_edge
        if body.x < 0
          :left
        elsif body.x + @radius * 2 >= Rug.width
          :right
        elsif body.y < 0
          :top
        elsif body.y + @radius * 2 >= Rug.height
          :bottom
        else
          nil
        end
      end
    end

    class Rectangle < Shape
      attr_accessor :w, :h

      def initialize w, h; @w, @h = w, h; end

      def overlap? shape
        if shape.is_a? Circle
          Physics.collide_circle_rect shape, self
        elsif shape.is_a? Rectangle
          Physics.collide_rect_rect self, shape
        else
          shape.overlap? self
        end
      end

      def check_edge
        if body.x < 0
          :left
        elsif body.x + @w >= Rug.width
          :right
        elsif body.y < 0
          :top
        elsif body.y + @h >= Rug.height
          :bottom
        else
          nil
        end
      end
    end

    class World
      attr_accessor :gravity, :collide_with_window

      def initialize
        # TODO: use a better data structure for objects like a quadtree
        @objects = Array.new
        @gravity = 200.0
        @collide_with_window = true
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

      def check_for_collision which
        obj = @objects.find do |o|
          o != which and o.overlap? which
        end

        if obj
          obj.collide which
          which.collide obj
        elsif @collide_with_window
          edge = which.shape.check_edge
          which.collide edge if edge
        end
      end

      def remove body
        @objects.delete body
      end
    end

    module Body
      attr_accessor :world, :shape, :mass, :x, :y, :vx, :vy

      def initialize x = 0.0, y = 0.0, vx = 0.0, vy = 0.0, mass = 0.0
        @x, @y, @vx, @vy, @mass = x, y, vx, vy, mass
      end

      def update_body dt
        @last_x, @last_y = @x, @y
        @x += @vx * dt / 1000.0
        @y += @vy * dt / 1000.0

        @world.check_for_collision self
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

      def overlap? body
        if @shape and body.shape
          @shape.overlap? body.shape
        else
          false
        end
      end

      def collide other
      end

      # This is typically overidden by classes that include this module,
      # but put it here anyway
      def update dt
        update_body dt
      end

      def revert_position
        @x, @y = @last_x, @last_y
      end
    end
  end
end
