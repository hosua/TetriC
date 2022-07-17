output="TetriC"
path="src"
flags="-lSDL2 -lSDL2_mixer -lSDL2_ttf -lSDL2_image -g -I\"$path\" -Wall"

gcc "$path"/*.c $flags -o "$output" && ./"$output"
