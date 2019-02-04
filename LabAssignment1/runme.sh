module load ispc
ispc -o LabAssignment1_ispc.o -h LabAssignment1_ispc.h LabAssignment1_ispc.ispc --woff
g++  -std=c++11 -c -I/opt/X11/include -o LabAssignment1.o LabAssignment1.cpp -L/opt/X11/lib -lm -pthread -lpthread -lX11 -ljpeg
g++  -std=c++11 LabAssignment1.o LabAssignment1_ispc.o -L/opt/X11/lib -lm -pthread -lpthread -lX11 -ljpeg
./a.out
