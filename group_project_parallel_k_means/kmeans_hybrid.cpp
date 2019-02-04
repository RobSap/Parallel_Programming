#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <chrono>
#include <map>
#include <unistd.h>

#include "kmeans_mpi.h"
#include "omp.h"
#include "mpi.h"

#define MASTER 0
#define VEC2D(typ) std::vector<std::vector<typ>>
#define NUM_THREADS 8

typedef std::chrono::high_resolution_clock::time_point TimePoint;
typedef std::chrono::high_resolution_clock Clock;

std::map<std::string, float> time_map;

Kmeans::Kmeans(int id, int num_tasks, int num_attr) {
	this->taskid = id;
	this->numtasks = num_tasks;
	this->numattr = num_attr;
}

float Kmeans::distance(std::vector<float> &a, std::vector<float> &b) {
	std::vector<float> difference;
	float dot_product = 0;
	float temp = 0;

	for(int i = 0; i < a.size(); i ++) {
		temp = a[i] - b[i]; 
		dot_product += (temp * temp);
	}

	return sqrt(dot_product);
}

/*
	Print a 2D vector.
*/
void Kmeans::print_2D_float(VEC2D(float) &input) {
	for(int i = 0; i < input.size() ; i++) {
		//std::cout << " I : " <<i <<std::endl;
			for(int j = 0; j < input[i].size() ; j++) {
				std::cout << input[i][j] << " ";
			}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}

void Kmeans::print_2D_int(VEC2D(int) &input) {
	for(int i = 0; i < input.size() ; i++) {
		//std::cout << " I : " << i << std::endl;
		for(int j = 0; j < input[i].size() ; j++) {
			std::cout << input[i][j] << " ";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}

/*
	Print a 3D vector.
*/
void Kmeans::print_3D(std::vector<VEC2D(float)> &input) {
	for(int i = 0; i < input.size() ; i++) {
		std::cout << "\n\nOn " << i << std::endl;
		for(int j= 0; j < input[i].size() ; j++) {
			for(int k = 0; k < input[i][j].size(); k++) { 
				std::cout << input[i][j][k] << " ";
			}
			std::cout << std::endl;
		}
	}
}


/*
	Compare every value in vec_a with the respective value in vec_b.
	
	Returns:
		0 if the difference between any point in vec_a and vec_b is greater than threshold,
		else 1.
*/
int Kmeans::check_convergence(VEC2D(float)& vec_a, VEC2D(float)& vec_b, float threshold) {
	int has_converged = 1;

	for(int i = 0; i < vec_a.size(); i++) {
		for(int j = 0; j < vec_a[i].size(); j++) {
			if(fabs(vec_a[i][j] - vec_b[i][j]) >= threshold) {
				has_converged = 0;
				break;
			}
		}
		if(!has_converged) break;
	}
	return has_converged;
}


/*
	Given a 2D vector `input`, select `num_elements` random elements from it and return those in a 2D vector.
	
	Returns:
		A 2D vector containing `num_elements` random elements from `input`.
*/
VEC2D(float) Kmeans::get_random_elements(VEC2D(float) &input, int num_elements) {
	VEC2D(float) output(num_elements);
	std::vector<int> starting_indexes(num_elements, -1);
	int rand_index = 0;
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
			//rand_index = rand() % input_size;
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

VEC2D(float) Kmeans::initialize_centers(int num_clusters, int num_attributes) {
	VEC2D(float) output(num_clusters, std::vector<float>(num_attributes));

	for(int i = 0; i < num_clusters; i++) {
		output[i] = std::vector<float>(num_attributes);

		for(int j = 0; j < num_attributes; j++) {
			output[i][j] = 0.0;
		}
	}
	
	return output;
}

VEC2D(int) Kmeans::initialize_clusters(VEC2D(float)& list, int num_clusters) {
	VEC2D(int) output(num_clusters);

	int group_size = list.size() / num_clusters;
	int left_over = list.size() % num_clusters;

	for(int i = 0; i < num_clusters; i++){
		output[i].resize(group_size);
		for(int j = 0; j < group_size; j++) {
			output[i][j] = (i * group_size) + j;
		}
	}

	for(int i = 0; i < left_over; i++){
		output[i].resize(group_size + 1);
		output[i][group_size] =(group_size * num_clusters) + i;
	}

	return output;
}

void Kmeans::update_timer(std::string key, std::chrono::time_point<std::chrono::high_resolution_clock> start_time) {
	auto end_time = Clock::now();
	time_map[key] += std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count();	
}

void Kmeans::print_timers() {
	for(std::map<std::string, float>::iterator it = time_map.begin(); it != time_map.end(); it++) {
//		printf("%s: %0.05f s\n", it->first.c_str(), (it->second/1000000000));
		printf("%0.05f\n", (it->second/1000000000));
		if(it->first == "cluster_assignment")
			printf("\n");

	}
}

void Kmeans::kmeans(VEC2D(float)& list, int num_clusters, int iterations, VEC2D(float)& initial_centers) {
	//special datamember for mpi implementation, I need to know if a custer has any points
	//assigned to it for communication back to the MASTER task
	follower_count.resize(num_clusters);
	for(int i = 0; i < num_clusters; ++i) {	
		follower_count[i] = 0;
	}
	
	int num_attributes = list[0].size();
	auto program_start_time = Clock::now();
	float convergence_threshold = 0.0001;

	VEC2D(float) new_centers = initialize_centers(num_clusters, num_attributes);
	VEC2D(float) prev_centers;

	VEC2D(int) clusters = initialize_clusters(list, num_clusters);
	
	if(taskid == MASTER){
		std::cout << "\ninitial centers" << std::endl;
		print_2D_float(initial_centers);
	}

   /* std::cout << "\nclusters " << std::endl;
	print_2D_int(clusters);
	std::cout << "\nnew_centers init " << std::endl;
	print_2D_float(new_centers);
	*/

	omp_set_num_threads(NUM_THREADS);


	//std::cout << "\nPrinting Centers" << std::endl;
	//print_2D(initial_centers);
	//TODO:Delete
	//Run for each iteration
	//TODO: Remove this sleep it is for easy reading debug statements
	sleep(taskid);

	for(int i = 0; i < iterations; i++)  {
		if(i == 0) {
			clusters = assign_to_cluster(list,initial_centers, clusters, num_clusters);
			new_centers = update_centers(list,initial_centers, clusters);
				if(taskid==MASTER){
				std::cout << "On iter=" << i << " of max " << iterations << std::endl;
				print_2D_float(new_centers);
			}
			prev_centers = initial_centers;
		}else {
			clusters = assign_to_cluster(list,new_centers, clusters, num_clusters);
			new_centers = update_centers(list,new_centers, clusters);
				if(taskid==MASTER){
				std::cout << "On iter=" << i << " of max " << iterations << std::endl;
				print_2D_float(new_centers);
			}
		}

		auto convergence_start = Clock::now();
  		if(check_convergence(new_centers, prev_centers, convergence_threshold)) {
  			//update_timer("convergence", convergence_start);
			if(taskid==MASTER){
				std::cout << "Ran " << i << " iterations" <<std::endl;
			}
  			break;
		}
				  
		//update_timer("convergence", convergence_start);
		prev_centers = new_centers;
		/*
		std::cout << "new clusters " << std::endl;
		print_2D_int(clusters);
		std::cout << "\nPrinting Centers" << std::endl;
		print_2D_float(new_centers);
		*/
	}


	if(taskid==MASTER){
		update_timer("program", program_start_time);
	
		print_timers();

		std::cout << "\nPrinting Centers" << std::endl;
		print_2D_float(new_centers);

		//std::cout << "\nPrinting Clusters" << std::endl;
			//print_2D_int(clusters);
		}
}

VEC2D(int)  Kmeans::assign_to_cluster(VEC2D(float)&original_list_points, VEC2D(float)& centoids, VEC2D(int) &points, int num_clusters) {

	//Assigns all of our points to new centers
	
	//clusters is our list of centoids
	// points is our list of all the points
	// num_clusters is the number of centoids we have
	
	// Iterate over all points
	auto cluster_assignment_start = Clock::now();

		//clusters will replace points
	VEC2D(int) clusters(num_clusters);

	for(int i = 0; i < num_clusters; i++) {
		clusters[i] = std::vector<int>();
	}

	int row_size = points.size();
	
	#pragma omp parallel shared(clusters)
	{
		float min_distance_to_cluster;
		float distance_from_cluster; 
		int belong_to_cluster;

		// Each thread creates its own private clusters 2D array
		VEC2D(int) private_clusters(num_clusters);

		for(int i = 0; i < num_clusters; i++) {
			private_clusters[i] = std::vector<int>();
		}

		int thread_id = omp_get_thread_num();

		for (int i = 0; i < points.size(); i++) {
			int row_size = points[i].size();

			int row_start = thread_id * (row_size / NUM_THREADS);
			int row_end = (thread_id == (NUM_THREADS - 1)) ? row_size : ((thread_id + 1) * (row_size / NUM_THREADS));

			for (int j = row_start; j < row_end; j++) {
				for (int cluster_index = 0; cluster_index < num_clusters; cluster_index++) {   
					//auto distance_start = Clock::now();
					distance_from_cluster = distance(centoids[cluster_index], original_list_points[points[i][j]]);

					//update_timer("distance_total", distance_start);

					// Allow cluster_index 0 to pass to set the initial `min_distance_to_cluster`, `belong_to_cluster` values
					if(cluster_index == 0 || min_distance_to_cluster > distance_from_cluster) {
						min_distance_to_cluster = distance_from_cluster;
						belong_to_cluster = cluster_index;
					}
				} 
				private_clusters[belong_to_cluster].push_back(points[i][j]);
			}
			
			
			#pragma omp for ordered schedule(static, 1)
			for(int t = 0; t < omp_get_num_threads(); t++) {
				
				#pragma omp ordered
				for(int cluster_index = 0; cluster_index < num_clusters; cluster_index++) {
					clusters[cluster_index].insert(clusters[cluster_index].end(), private_clusters[cluster_index].begin(), private_clusters[cluster_index].end());
					private_clusters[cluster_index].clear();
				}
			}
		}
	}

	//print_2D_int(clusters);

	update_timer("cluster_assignment", cluster_assignment_start);

	return clusters; 
}

//list,initial_center/new_centers,clusters
VEC2D(float) Kmeans::update_centers(VEC2D(float) &oringinal_points,VEC2D(float) &centoids,VEC2D(int) &clusters) {
	
	VEC2D(float) new_centers(centoids.size());
 	
	//For number of clusters, create array that size
	for (int i = 0; i < new_centers.size(); i++) {
		new_centers[i] = std::vector<float>(); 
	}
	
	//std::cout << "In function " << std::endl;
	//print_2D_float(new_centers); 
	auto mean_timer = Clock::now();
		
	for (int i = 0; i < clusters.size(); i++) {
		//std::cout << "On cluster " << i << std::endl;
		follower_count[i] = clusters[i].size(); //special info for mpi reduction
		new_centers[i] = mean(oringinal_points, clusters[i]);
	}


	//####################################################
	//MPI reduction of local centroids to world centroids
	//####################################################
	std::vector<float> flattened_centers, reduced_centers;
	std::vector<int> reduced_divider(follower_count.size());
	flatten_vector(new_centers,flattened_centers);
	reduced_centers.resize(flattened_centers.size());
	
	MPI_Reduce(flattened_centers.data(),reduced_centers.data(),flattened_centers.size(),MPI_FLOAT,MPI_SUM,MASTER,MPI_COMM_WORLD);
	MPI_Reduce(follower_count.data(),reduced_divider.data(),follower_count.size(),MPI_INT,MPI_SUM,MASTER,MPI_COMM_WORLD);
	
	int num_clusters = reduced_divider.size();
	if(taskid==MASTER){
		VEC2D(float) temp;
		inflate_vector(reduced_centers,temp,num_clusters,numattr);

		

		for(int i=0;i<num_clusters;++i){
			for(int j=0;j<numattr;++j){
				temp[i][j] = temp[i][j]/static_cast<float>(reduced_divider[i]);
			}
		}

		flatten_vector(temp,flattened_centers);
		MPI_Bcast(flattened_centers.data(),flattened_centers.size(),MPI_FLOAT,MASTER,MPI_COMM_WORLD);	
	}else{	
		MPI_Bcast(flattened_centers.data(),flattened_centers.size(),MPI_FLOAT,MASTER,MPI_COMM_WORLD);	
	}
	
	inflate_vector(flattened_centers,new_centers,num_clusters,numattr);
	//###############################################################
	//MPI Reduction finished, the work gang agrees on the new centers
	//###############################################################

	update_timer("mean", mean_timer);

	return new_centers;
}


std::vector<float> Kmeans::mean(VEC2D(float) &original_points, std::vector<int> &cluster) {
	int i, j;
	float cluster_size = cluster.size();

	//TODO:if(taskid == MASTER) std::cout << "Master sees mean.size() of " << original_points[0].size() << std::endl;
	std::vector<float> means(original_points[0].size(), 0.0);

	//std::cout<< "clusters size is: " << cluster.size() << std::endl;
	for(i = 0; i < cluster_size; i++) {
		//std::cout<< "itertating j for " << original_points[cluster[i]].size() << std::endl;
		for(j=0; j < original_points[cluster[i]].size();j++){
			means[j] += original_points[cluster[i]][j];
		}
	}

	return means;
	/*
 	*	all this has been removed because we want to wait to divide until after MPI_Reduce
 	*
	std::vector<float> output(means.size());
	//std::cout<< "output size" << output.size() << std::endl;

	for(i = 0; i < output.size(); i++) {
		//TODO:if(taskid == MASTER) std::cout << "Master see cluster of size " << cluster_size << std::endl;
		output[i] = means[i] / cluster_size;
		//std::cout << " output["<<i<<"]="<<output[i] <<std::endl;
	}
	
	   
	return output;
	*/
}

void flatten_vector(VEC2D(float) &src, std::vector<float> &dst){
	int rows=src.size(), cols=src[0].size();

	dst.resize(rows*cols);

	for(int i=0;i<rows;++i){
		for(int j=0;j<cols;++j){
			dst[i*cols+j] = src[i][j];
		}
	}
}

void inflate_vector(std::vector<float> &src, VEC2D(float) &dst, int rows, int cols){
	dst.resize(rows);

	for(int i=0;i<rows;++i){
		dst[i].resize(cols);
		for(int j=0;j<cols;++j){
			dst[i][j] = src[i*cols+j];
		}
	}
}
