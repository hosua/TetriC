output="TetriC"
path="src"
flags="-lSDL2 -lSDL2_ttf -g -I\"$path\" -Wall"

gcc "$path"/*.c $flags -o "$output" && ./"$output"
