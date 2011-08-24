for opt in "-O0" "-O1" "-O2" "-O3"
do
	echo "Optimization ${opt}"; g++ ${opt} -std=c++0x -o sensei ../src/sensei.cpp && du -h sensei && ./timefile.sh ../test/*; echo
done
