module load intel/17
module load intel/tbb
icc -std=c++11 -o a_tbb LabAssignment2.cpp -ltbb -w -lm -pthread -lpthread -lX11 -ljpeg
./a_tbb image_list.txt
icc -std=c++11 -o a_tbb LabAssignment2_part1.cpp -ltbb -w -lm -pthread -lpthread -lX11 -ljpeg
./a_tbb image_list.txt
icc -std=c++11 -o a_tbb LabAssignment2_part2.cpp -ltbb -w -lm -pthread -lpthread -lX11 -ljpeg
./a_tbb image_list.txt
icc -std=c++11 -o a_tbb LabAssignment2_part3.cpp -ltbb -w -lm -pthread -lpthread -lX11 -ljpeg
./a_tbb image_list.txt
