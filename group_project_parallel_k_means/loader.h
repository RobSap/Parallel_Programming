#ifndef LOADER_H
#define LOADER_H

#include <string>
#include <vector>

enum classification { setosa, versicolor, virginica };

void load_data(std::vector<std::vector<float>> &dataset, std::string &path, int data_size, int num_attributes, int skip_col, char delimiter);

#endif
