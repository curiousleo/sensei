redo-ifchange $1.cpp
gcc-4.4 -std=c++0x -O3 -MD -MF $1.dep -c -o $3 $1.cpp
read DEPS <$1.dep
redo-ifchange ${DEPS#*:}
