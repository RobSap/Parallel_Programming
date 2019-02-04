#include <iostream>
#include "kmeans_mpi.h"
#include "loader.h"
#include "mpi.h"

#define  MASTER   0

struct _datasource {
    std::string filepath;
    int data_size;
    int num_attribs;
    int skip_col;
    char delimiter;
};

typedef struct _datasource DataSource;

void dump_input(std::vector<std::vector<float>> &data){
  for(int i=0;i<data.size();++i){
    for(int j=0;j<data[0].size();++j){
      std::cout << data[i][j] <<  " ";
    }
    std::cout << std::endl;
  }
}

std::vector<std::vector<float>> get_random_elements(std::vector<std::vector<float>> input, int num_elements) {
	std::vector<std::vector<float>> output(num_elements);
	std::vector<int> starting_indexes(num_elements, -1);
	int rand_index;
	int input_size = input.size();
	
	if(input_size < num_elements) {
		fprintf(stderr, "The input size of the list is less than the requested number of random elements.");
		exit(-1);
	}

	// Seed the random number generator
	srand(1100);
	
	int index_already_used;
	for(int i = 0; i < num_elements; i++) {
		while(true) {
			index_already_used = 0;
			rand_index = rand() % input_size;
			for(int j = 0; j < starting_indexes.size(); j++) {
				if(starting_indexes[j] == -1) break;
				if(starting_indexes[j] == rand_index) {
					index_already_used = 1;
					break;
				}
			}
			
			if(index_already_used) continue;
			else break;
		}
		
		output[i] = input[rand_index];
		starting_indexes[i] = rand_index;
	}

	return output;
}

int main(int argc, char *argv[])
{

  //MPI stuff
  int   numtasks, taskid, len;
  char hostname[MPI_MAX_PROCESSOR_NAME];

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
  MPI_Comm_rank(MPI_COMM_WORLD,&taskid);
  MPI_Get_processor_name(hostname, &len);
  
  //DataSource inputs {"./iris.csv",150,4,0,','};
  //DataSource inputs {"./Frogs_MFCCs.csv",7195,22,0,','};
  //DataSource inputs {"./household_power_consumption.txt",2075259,9,2,';'};
  DataSource inputs {"./household_power_consumption2.txt",2049280,9,2,';'};
  
  std::vector<std::vector<float>> data;
  std::vector<float> flat_data;
  std::vector<int> counts;
  std::vector<int> displs;
  
  int subdata_size = inputs.data_size/numtasks;
  int remainder = inputs.data_size%numtasks;
  if(MASTER==taskid){
	//get data and make a flat version of it
  	load_data(data,inputs.filepath,inputs.data_size,inputs.num_attribs,inputs.skip_col,inputs.delimiter);
	flatten_vector(data,flat_data);
  }

  //this narly chunk of code calculates the side each taskid should receive and the displacement
  //to read from in the buffer when the MPI_Scatterv goes out.
  int offset = 0;
  int num_attr = inputs.num_attribs - inputs.skip_col;
  for(int i=0;i<numtasks;++i){
	if(remainder>i){
		counts.push_back((subdata_size+1)*num_attr);
		displs.push_back(offset);
		offset += (subdata_size+1)*num_attr;
	}else{
		counts.push_back(subdata_size*num_attr);
		displs.push_back(offset);
		offset += (subdata_size*num_attr);
	}
  }

  //some debug prints
  if(taskid==MASTER){
  	for(int i=0;i<numtasks;++i){
		std::cout << "c=" << counts[i] << "    d=" << displs[i] << std::endl;
	}
  }
  
  //calculate how much data I am getting and size my personal flat buffer and 2d buffer
  int my_data_size = counts[taskid]/num_attr;
  std::cout << "taskid=" << taskid << " my_data_size=" << my_data_size << std::endl;
  std::vector<float> my_flat_data(counts[taskid]);
  std::vector<std::vector<float>> my_data(my_data_size,std::vector<float>(num_attr,0.0));

  //finally ready to scatter the data out the all tasks
  MPI_Scatterv(flat_data.data(),counts.data(),displs.data(),MPI_FLOAT,my_flat_data.data(),my_flat_data.size(),MPI_FLOAT,MASTER,MPI_COMM_WORLD);
  flat_data.clear();

  //turn the flat data from the MPI_Scatterv back into a 2d vector 
  inflate_vector(my_flat_data,my_data,my_flat_data.size()/num_attr,num_attr);

  /*std::cout << "taskid=" << taskid << "  tail= ";
  for(int i=0;i<num_attr;++i){
  	std::cout << my_data[my_data_size-1][i] << ", ";
  }
  std::cout << std::endl;

  std::cout << "taskid=" << taskid << "  head= ";
  for(int i=0;i<num_attr;++i){
  	std::cout << my_data[0][i] << ", ";
  }
  std::cout << std::endl;
  */
  Kmeans *kmean = new Kmeans(taskid,numtasks,num_attr);
  int num_clusters = 6;
  int num_iterations = 100;

  std::vector<std::vector<float>> world_init_centers;  
  if(taskid==MASTER){
	std::vector<std::vector<float>> temp = get_random_elements(data,num_clusters);
  	flatten_vector(temp,flat_data);
	MPI_Bcast(flat_data.data(),flat_data.size(),MPI_FLOAT,MASTER,MPI_COMM_WORLD);
  }else{
	flat_data.resize(num_clusters*num_attr);
	MPI_Bcast(flat_data.data(),flat_data.size(),MPI_FLOAT,MASTER,MPI_COMM_WORLD);
  }

  inflate_vector(flat_data,world_init_centers,flat_data.size()/num_attr,num_attr);

  /*std::cout << "taskid=" << taskid << " my_centers    ";
  for(int i=0;i<world_init_centers.size();++i){
	for(int j=0;j<world_init_centers[i].size();++j){
  		std::cout << world_init_centers[i][j] << ", ";
	}
  }
  std::cout << std::endl;
  */
  kmean->kmeans(my_data, num_clusters, num_iterations, world_init_centers);
   

  MPI_Finalize();
 
}

