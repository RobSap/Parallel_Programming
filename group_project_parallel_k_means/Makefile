CC = g++
MPI_CC = mpic++
ACC_CC = pgc++
CXXFLAGS = -w -Wall -std=c++11 
CXXACCFLAGS = -w -std=c++11

OBJS = main.cpp loader.cpp kmeans.cpp
LIBS = 

OMP_OBJS = main.cpp loader.cpp kmeans_omp.cpp
OMP_LIBS = -fopenmp

MPI_OBJS = main_mpi.cpp loader.cpp kmeans_mpi.cpp
MPI_LIBS =

HYBRID_OBJS = main_mpi.cpp loader.cpp kmeans_hybrid.cpp

ACC_OBJS = main_acc.cpp loader.cpp kmeans_acc.cpp
ACC_LIBS = -acc -mp -Minfo=all -ta=host

default: all
all: serial omp acc mpi hybrid

serial: $(OBJS)
	$(CC) -o $@ $(CXXFLAGS) $(OBJS) $(LIBS)

omp: $(OMP_OBJS)
	$(CC) -o $@ $(CXXFLAGS) $(OMP_OBJS) $(OMP_LIBS)

mpi: $(MPI_OBJS)
	$(MPI_CC) -o $@ $(CXXFLAGS) $(MPI_OBJS) $(MPI_LIBS)

hybrid: $(HYBRID_OBJS)
	$(MPI_CC) -o $@ $(CXXFLAGS) $(HYBRID_OBJS) $(MPI_LIBS) $(OMP_LIBS)

acc: $(ACC_OBJS)
	$(ACC_CC) -o $@ $(CXXACCFLAGS) $(ACC_OBJS) $(ACC_LIBS)

mpi_run:
	mpirun -np 3 ./mpi
clean:
	rm -f serial
	rm -f omp
	rm -f mpi
