require 'mkmf'

good = true

libraries = %w[SDL SDL_image]

libraries.each do |lib|
  if not have_library(lib)
    good = false
  end
end

create_makefile("Rug") if good

