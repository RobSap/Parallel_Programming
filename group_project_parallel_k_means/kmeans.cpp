#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <chrono>
#include <map>

#include "kmeans.h"
//#include <utility>

typedef std::chrono::high_resolution_clock Clock;
std::map<std::string, float> time_map;

float Kmeans::distance(std::vector<float> &a, std::vector<float> &b) {
	std::vector<float> difference;
	float dot_product = 0;
        float temp=0;
	for(int i = 0; i < a.size(); i ++) {
		temp = a[i] - b[i]; 
		dot_product += (temp * temp);
	}

	return sqrt(dot_product);
}

/*
	Print a 2D vector.
*/
void Kmeans::print_2D_float(std::vector<std::vector<float>> &input) {
	for(int i = 0; i < input.size() ; i++) {
		//std::cout << " I : " <<i <<std::endl;
                for(int j = 0; j < input[i].size() ; j++) {
			std::cout << input[i][j] << " ";
		}
		std::cout << std::endl;
	}
		std::cout << std::endl;
}

void Kmeans::print_2D_int(std::vector<std::vector<int>> &input) {
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
void Kmeans::print_3D(std::vector<std::vector<std::vector<float>>> &input) {
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
int Kmeans::check_convergence(std::vector<std::vector<float>>& vec_a, std::vector<std::vector<float>>& vec_b, float threshold) {
	int has_converged = 1;
        for(int i = 0; i < vec_a.size(); i++) {
		for(int j = 0; j < vec_a[i].size(); j++) {
			//printf("[%0.05f][%0.05f] = %0.05f\n", vec_a[i][j], vec_b[i][j], fabs(vec_a[i][j] - vec_b[i][j]));
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
std::vector<std::vector<float>> Kmeans::get_random_elements(std::vector<std::vector<float>> &input, int num_elements) {
	std::vector<std::vector<float>> output(num_elements);
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

std::vector<std::vector<float>> Kmeans::initialize_centers(int num_clusters, int num_attributes) {


	std::vector<std::vector<float>> output
		(num_clusters, std::vector<float>
			(num_attributes));

	for(int i = 0; i < num_clusters; i++) {
		output[i] = std::vector<float>(num_attributes);

		for(int j = 0; j < num_attributes; j++) {
			output[i][j] = 0.0;
		}
	}
	
	return output;
}

std::vector<std::vector<int>> Kmeans::initialize_clusters(std::vector<std::vector<float>>& list, int num_clusters) {
	
        std::vector<std::vector<int>> output(num_clusters);

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
		printf("%s: %0.05f s\n", it->first.c_str(), (it->second/1000000000));
	}
}

void Kmeans::kmeans(std::vector<std::vector<float>>& list, int num_clusters, int iterations, std::vector<std::vector<float>>& initial_centers) {


	int num_attributes = list[0].size();
	auto program_start_time = Clock::now();
        float convergence_threshold = 0.0001;

	std::vector<std::vector<float>> new_centers = initialize_centers(num_clusters, num_attributes);
	std::vector<std::vector<float>> prev_centers;

	std::vector<std::vector<int>> clusters = initialize_clusters(list, num_clusters);
       
         
        std::cout << "\ninitial centers " << std::endl;
        print_2D_float(initial_centers);
       /* std::cout << "\nclusters " << std::endl;
        print_2D_int(clusters);
        std::cout << "\nnew_centers init " << std::endl;
        print_2D_float(new_centers);
        */


	//std::cout << "\nPrinting Centers" << std::endl;
	//print_2D(initial_centers);
//TODO:Delete
	//Run for each iteration 
	for(int i = 0; i < iterations; i++)  {
		if(i == 0) {
			clusters = assign_to_cluster(list,initial_centers, clusters, num_clusters);
			new_centers = update_centers(list,initial_centers, clusters); 
	                  print_2D_float(new_centers);
            	prev_centers = initial_centers;
                } 
                else {
			clusters = assign_to_cluster(list,new_centers, clusters, num_clusters);
			new_centers = update_centers(list,new_centers, clusters);
                          print_2D_float(new_centers); 
		}

			auto convergence_start = Clock::now();
  			if(check_convergence(new_centers, prev_centers, convergence_threshold)) {
  		                	//update_timer("convergence", convergence_start);
  					std::cout << "Ran " << i << " iterations" <<std::endl;
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

	update_timer("program", program_start_time);
	
	print_timers();

	std::cout << "\nPrinting Centers" << std::endl;
	print_2D_float(new_centers);

	//std::cout << "\nPrinting Clusters" << std::endl;
        //print_2D_int(clusters);

}

std::vector<std::vector<int>>  Kmeans::assign_to_cluster(std::vector<std::vector<float>>&original_list_points, std::vector<std::vector<float>>& centoids, std::vector<std::vector<int>> &points, int num_clusters) {

        //clusters will replace points
	std::vector<std::vector<int>> clusters(num_clusters);
 
	for(int i = 0; i < num_clusters; i++) {
		clusters[i] = std::vector<int>();
	}

	//Assigns all of our points to new centers
	
	//clusters is our list of centoids
	// points is our list of all the points
	// num_clusters is the number of centoids we have
 
	float min_distance_to_cluster;
	float distance_from_cluster; 
	int belong_to_cluster;
	
	// Iterate over all points
	auto cluster_assignment_start = Clock::now();

        //std::cout << " points.size() " << points.size() << std::endl;
	for (int i = 0; i < points.size(); i++) {
        //std::cout << " points[i].size() " << points[i].size() << std::endl;
		for (int j = 0; j < points[i].size(); j++) {

			for (int cluster_index = 0; cluster_index < num_clusters; cluster_index++) {   
				auto distance_start = Clock::now();
                                             
                                //std::cout << " centoids[cluster_index] " << centoids[cluster_index].size() << "original_list_points[points[i][j]] " << original_list_points[points[i][j]].size() << std::endl;
				distance_from_cluster = distance(centoids[cluster_index], original_list_points[points[i][j]]);

				update_timer("distance_total", distance_start);
				// Allow cluster_index 0 to pass to set the initial `min_distance_to_cluster`, `belong_to_cluster` values
				if(cluster_index == 0 || min_distance_to_cluster > distance_from_cluster) {
					min_distance_to_cluster = distance_from_cluster;
					belong_to_cluster = cluster_index;
				}
			}
			clusters[belong_to_cluster].push_back(points[i][j]);
		}
	}
	
	update_timer("cluster_assignment", cluster_assignment_start);
        //print_2D_int(clusters); 
	return clusters; 
}


std::vector<std::vector<float> > Kmeans::update_centers(std::vector<std::vector<float> > & oringinal_points,std::vector<std::vector<float> > &centoids,std::vector<std::vector<int>>&clusters) {
	std::vector<std::vector<float>> new_centers(centoids.size());
 
	//For number of clusters, create array that size
	for (int i = 0; i < new_centers.size(); i++) {
		new_centers[i] = std::vector<float>();
	     
        }
        //std::cout << "In function " << std::endl;
        //print_2D_float(new_centers); 
	auto mean_timer = Clock::now();
        
	for (int i = 0; i < clusters.size(); i++) {
                //std::cout << "On cluster " << i << std::endl;
		new_centers[i] = mean(oringinal_points, clusters[i]);
	
        }
       
	update_timer("mean", mean_timer);

	return new_centers;
}


std::vector<float> Kmeans::mean(std::vector<std::vector<float>> &original_points, std::vector<int> &cluster) {
	int i, j;
	float cluster_size = cluster.size();

	std::vector<float> means(original_points[0].size(), 0.0);

        //std::cout<< "clusters size is: " << cluster.size() << std::endl;
	for(i = 0; i < cluster_size; i++) {
            //std::cout<< "itertating j for " << original_points[cluster[i]].size() << std::endl;
            for(j=0; j < original_points[cluster[i]].size();j++)
			means[j] += original_points[cluster[i]][j];
                 }
	
       
	std::vector<float> output(means.size());
        //std::cout<< "output size" << output.size() << std::endl;

	for(i = 0; i < output.size(); i++) {
		output[i] = means[i] / cluster_size;
                //std::cout << " output["<<i<<"]="<<output[i] <<std::endl;
	}
       
	return output;
}

