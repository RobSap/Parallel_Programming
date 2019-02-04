#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include "loader.h"

//pulled from https://stackoverflow.com/questions/447206/c-isfloat-function
bool isFloat(std::string myString ) {
    std::istringstream iss(myString);
    float f;
    iss >> std::noskipws >> f; // noskipws considers leading whitespace invalid
    // Check the entire string was consumed and if either failbit or badbit is set
    return iss.eof() && !iss.fail(); 
}

void load_data(std::vector<std::vector<float>> &dataset, std::string &path, int data_size, int num_attributes, int skip_col, char delimiter){
    std::ifstream file;
    std::string line;
    int counter = 0;

    file.open(path.c_str());

    if(file.good()){
        dataset.resize(data_size);

        for(int i=0;i<data_size;++i){
            int col_to_skip = skip_col;
            std::getline(file,line);
            if(line.length()<num_attributes){
                std::cout << "line " << i << "is bad" << std::endl;
                continue;
            }
            std::stringstream ss(line);
            std::vector<float> result;
            result.resize(num_attributes-skip_col);

            for(int i=0;i<num_attributes;++i){
                std::string substr;
                std::getline(ss,substr,delimiter);
                if(col_to_skip==0){
                    if(isFloat(substr))
                        result[i] = std::stof(substr);
                    else
                        result[i] = 0.0;
                }
                else
                    col_to_skip--;
            }
            
            dataset[i] = result;
        }
    }
    file.close();
}

