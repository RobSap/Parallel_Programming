#include <iostream>
#include <vector>
#include <chrono>

class Kmeans {
	public:
		void kmeans( std::vector<std::vector<float>>& list, int clusters, int iterations, std::vector<std::vector<float>>& initialize_centers);
		void print_3D(std::vector<std::vector<std::vector<float>>>& input);
		void print_2D_float(std::vector<std::vector<float>>& input);
		void print_2D_int(std::vector<std::vector<int>>& input);
		
		void update_timer(std::string key, std::chrono::time_point<std::chrono::high_resolution_clock> start_time);
		void print_timers();

	private:
		std::vector<std::vector<float>> initialize_centers(int num_clusters, int num_attributes);
		std::vector<std::vector<int>> initialize_clusters(std::vector<std::vector<float>>& list, int num_clusters);

		std::vector<std::vector<float>> get_random_elements(std::vector<std::vector<float>>& input, int num_elements);
		int check_convergence(std::vector<std::vector<float>>& vec_a, std::vector<std::vector<float>>& vec_b, float threshold);
		
		std::vector<std::vector<float> >  update_centers(std::vector<std::vector<float>>& points, std::vector<std::vector<float>>& centoids,std::vector<std::vector<int>>& clusters);
		std::vector<std::vector<int>> assign_to_cluster(std::vector<std::vector<float>>& points, std::vector<std::vector<float>>& list, std::vector<std::vector<int>>& list2, int num_clusters);
		std::vector<float> mean(std::vector<std::vector<float>>& points,std::vector<int>& cluster);
		float distance(std::vector<float>& a, std::vector<float>& b);
};
