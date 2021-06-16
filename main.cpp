#include <cstdlib>
#include <iostream>
#include <Eigen/Sparse>
#include "inc/rw_image.h"

int main(int argc, char *argv[]) {
    if(argc != 3) {std::cerr << "invalid number of arguments. Must be 2" << std::endl; abort();}

    PNG wrapper{argv[1]};
    wrapper.read_png_file();
    wrapper.process_png_file();
    wrapper.write_png_file(argv[2]);

    /*Eigen::SparseMatrix<double> sm(3,2);
    sm.insert(1, 1) = 9;
    sm.coeffRef(1, 1) = 8;
    std::cout << sm.coeffRef(1, 1);*/

    return 0;
    /// TODO: check row major/col major
}