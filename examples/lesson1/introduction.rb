require File.dirname(__FILE__) + "/../../lib/Rug"

# Configuration settings for our window:
# - put a title for our window
# - make the window 1024x768 pixels
Rug.conf do
  title       "Lesson 1 - Introduction to Rug"
  width       1024
  height      768
  show_cursor false
end

Rug.draw do
  # Draw some text on the screen
  text 10, 10, "Hello, world!"
end

Rug.start
