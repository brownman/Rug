require 'mkmf'

good = true

libraries = %w[SDL SDL_image SDL_gfx stdc++]

libraries.each do |lib|
  if not have_library(lib)
    good = false
  end
end

# this is needed to get SDL_gfx working
$CFLAGS << `sdl-config --cflags`

create_makefile("Rug") if good

