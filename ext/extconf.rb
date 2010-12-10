require 'mkmf'

good = true

libraries = %w[SDL SDL_image SDL_gfx]

libraries.each do |lib|
  if not have_library(lib)
    good = false
  end
end

create_makefile("Rug") if good

