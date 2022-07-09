output="TetriC"
flags="-lSDL2 -lSDL2_ttf -g -I. -Wall"

gcc *.c $flags -o "$output" && ./"$output"
