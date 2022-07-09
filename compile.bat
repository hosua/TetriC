output="TetriC"
flags="-lSDL2 -lSDL2_ttf -g -I."

gcc *.c $flags -o "$output" && ./"$output"
