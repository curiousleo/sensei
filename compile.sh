CMD="gcc-4.4 -std=c++0x -O3 -o bin/sensei src/cover.cpp src/sensei.cpp"
echo "Compiling with command: $CMD"
$($CMD)
chmod +x bin/sensei
