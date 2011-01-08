module Rug
  module GUI
    def self.load
      Rug.keydown { |key| keydown key }
      Rug.keyup { |key| keyup key }
      Rug.mousedown { |x, y, button| mousedown x, y, button }
      Rug.mouseup { |x, y, button| mouseup x, y, button }
      Rug.mousemove { |x, y| mousemove x, y }
    end

    def self.keydown key
    end

    def self.keyup key
    end

    def self.mousedown x, y, button
    end

    def self.mouseup x, y, button
    end

    def self.mousemove x, y
    end
  end
end
