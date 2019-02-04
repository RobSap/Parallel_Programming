#include <iostream>
#include "kmeans_acc.h"
#include "loader.h"

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

std::vector<std::vector<float>> get_random_elements(std::vector<std::vector<float>>& input, int num_elements) {

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
		/*for (int k = 0; k < output[i].size(); k++)
		  std::cout<<output[i][k] << " ";
		  std::cout<<std::endl;*/
		starting_indexes[i] = rand_index;
	}
	return output;
}

int main()
{
	int clus[] = {3,4,8,10};
	int threads[] = {1,2,3,4,6,8,10,12};
	std::vector<DataSource> inputs { 
		//{"./iris.csv",150,4,0,','}, 
		{"./Frogs_MFCCs.csv",7195,22,0,','}, 
			//{"./household_power_consumption.txt",2075259,9,2,';'},
			//{"./household_power_consumption2.txt",2049280,9,2,';'}
	};
	std::vector<std::vector<float>> data;
	data.resize(inputs.size());


	for(int i=0;i<inputs.size();++i){ 
		load_data(data,inputs[i].filepath,inputs[i].data_size,inputs[i].num_attribs,inputs[i].skip_col,inputs[i].delimiter);
	}
	for(int i = 0; i < sizeof(clus)/sizeof(clus[0]); i++){
		for (int j = 0; j < sizeof(threads)/sizeof(threads[0]); j++){
			Kmeans *kmean = new Kmeans();
			int num_clusters = clus[i];
			int num_iterations = 20;
			//kmeans(data,num_custers,iterations)
			//kmean->kmeans(data[0],3,10000);
			//kmean->kmeans(data[2],3,100);
			std::vector<std::vector<float>> random_elements =  get_random_elements(data, num_clusters);
			kmean->kmeans(data, num_clusters, num_iterations, random_elements, threads[j]);
			//kmean->kmeans(data[1], num_clusters, num_iterations, get_random_elements(data[1]));
			delete(kmean);
		}
	}
}

