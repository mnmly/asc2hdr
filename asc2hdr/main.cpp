//
//  main.cpp
//  asc2hdr
//
//  Created by HIROAKI YAMANE on 01/09/2019.
//  Copyright © 2019 HIROAKI YAMANE. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <limits>

#include "cmdline.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

int main(int argc, char * argv[]) {
    //int stbi_write_hdr(char const *filename, int w, int h, int comp, const float *data);
//    std::string path = "/Users/mnmly/Downloads/test.hdr";
//    std::string input = "/Users/mnmly/Downloads/Download_1285208/england-dtm-1m_3065997/sy/sy4998_dtm_1m.asc";
    
    // parse command line arguments
    cmdline::parser p;
    p.parse_check(argc, argv);
    
    if (p.rest().size() != 2) {
        std::cerr << "infile and outfile required" << std::endl << p.usage();
        std::exit(1);
    }
    
    // extract command line arguments
    const std::string input = p.rest()[0];
    const std::string path = p.rest()[1];
    
    std::ifstream file(input);
    std::vector<float> data;
    int i = 0;
    int ncols = -1;
    int nrows = -1;
    float cellSize = -1.f;
    float min = std::numeric_limits<float>::max();
    float max = std::numeric_limits<float>::min();

    if (file.is_open()) {
        std::string line;
        while (getline(file, line)) {
            if ( i < 6 ) {
                std::vector<std::string> result;
                std::istringstream iss(line);
                for(std::string s; iss >> s; ) {
                    result.push_back(s);
                }
                if ( result.size() == 2 ){
                    
                    if (result[0] == "ncols") ncols = atoi(result[1].c_str());
                    if (result[0] == "nrows") nrows = atoi(result[1].c_str());
                    if (result[0] == "cellsize") cellSize = atof(result[1].c_str());

                }
            } else {
                if (data.size() == 0 && ncols != -1 && nrows != -1) {
                    data.reserve(ncols * nrows);
                }
                std::istringstream is( line );
                float n;
                while( is >> n ) {
                    data.push_back(n);
                    if (max < n) max = n;
                    if (min > n) min = n;
                }
            }
            i++;
        }
        file.close();
    }
    
    std::cout << min << " - " << max << " - cellsize of " << cellSize << " col x row: " << ncols << " x " << nrows << std::endl;
    std::cout << "delta: " <<  max - min << std::endl;
    
    i = 0;
    std::transform (data.begin(), data.end(), data.begin(), [min, max](float d) -> float { return (d - min) / ((max - min) + 0.0001); });
   
    int comp = 1;
    stbi_write_hdr(path.c_str(), nrows, ncols, comp, data.data());
    return 0;
}
