module load mpi/openmpi-x86_64

mpic++ -std=c++11 -o lab4 lab4.cpp
mpirun -np 1 ./lab4

#mpic++ -std=c++11 -o lab4_2X2 lab4_2X2.cpp
#mpirun -np 4 ./lab4_2X2

#mpic++ -std=c++11 -o lab4_3X3 lab4_3X3.cpp
#mpirun -np 9 ./lab4_3X3

#module load slurm
#srun --pty --nodes=2 --ntasks=4  mpirun lab4

#module load slurm
#srun --pty --nodes=2 --ntasks=4  mpirun lab4_2X2
