require File.dirname(__FILE__) + (RUBY_PLATFORM =~ /win32/ ? "/../bin/Rug.dll" : '/../ext/Rug.so')
require File.dirname(__FILE__) + '/Animation'
require File.dirname(__FILE__) + '/Colour'
require File.dirname(__FILE__) + '/HasAnimation'
require File.dirname(__FILE__) + '/Physics'
require File.dirname(__FILE__) + '/Gui'

module Rug
  class AnimatedBody
    include Rug::Physics::Body
    include Rug::HasAnimation

    def update dt
      update_body dt
      update_animation dt
    end
  end
end
