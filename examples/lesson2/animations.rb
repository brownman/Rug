require File.dirname(__FILE__) + "/../../lib/Rug"

# Configuration settings for our window:
# - put a title for our window
# - make the window 1024x768 pixels
Rug.conf do
  title  "Lesson 2 - Animations"
  width  1024
  height 768
end

# The load method is called once at the beginning of the application.
Rug.load do
  # Animations have a series of frame sets. For this animation we will only
  # have one, which is called :idle.
  $bear = Rug::Animation.new :idle => {
    :filename => "bear_idle.png",  # The filename of the animation
    :timing => 500                 # The number of milliseconds per frame
  }
end

# Attach an event to the mouse movement to keep track of the cursor location.
Rug.mousemove do |x, y|
  $x, $y = x, y
end

# Draw the bear at the mouse location
Rug.draw do
  $bear.draw $x, $y
end

# Update any game objects, including animations. This will move the bear
# animation to the next frame if necessary.
Rug.update do
  $bear.update
end

Rug.start

