DEPS="src/sensei.o"
redo-ifchange $DEPS
gcc -lboost_thread-mt -O3 -o bin/sensei $DEPS
