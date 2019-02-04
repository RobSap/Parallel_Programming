GCC=mpic++

all: example.cpp
	$(GCC) -O3 -march=native ./page_rank.cpp -o main

run: all

# My own two test files
#	mpiexec -np 1 ./main test_undir.tab.txt test_domains.2.txt 5
#	mpiexec -np 2 ./main test_undir.tab.txt test_domains.2.txt 5
#	mpiexec -np 1 ./main test_domains.2.txt test_domains.2.txt 5
#	mpiexec -np 2 ./main test_domains.2.txt test_domains.2.txt 5
#
#undir file
	mpiexec -np 2 ./main fl_undir.tab.txt fl_domains.2.txt 5
#       mpiexec -np 2 ./main fl_undir.tab.txt fl_domains.1.txt 5
#Normal run files
#	mpiexec -np 2 ./main  fl_compact.tab fl_compact_part.2 5
#	mpiexec -np 4 ./main fl_compact.tab fl_compact_part.4 5
#	mpiexec -np 1 ./main fl_compact.tab fl_compact_part.1 5 

