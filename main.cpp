#include <cstdlib>
#include <iostream>
/*#include <Eigen/Sparse>*/
#include <Eigen/Core>
#include <vector>
#include <map>
#include <fstream>
#include "inc/rw_image.h"
#include "inc/main_computations.h"

int main(int argc, char *argv[]) {
    if(argc != 3) {std::cerr << "invalid number of arguments. Must be 2" << std::endl; abort();}

    PNG wrapper{argv[1]};
    wrapper.read_png_file();

    Eigen::MatrixXd gray{wrapper.height, wrapper.width};
    wrapper.form_matrix(gray);

    unsigned seed_types = 0;
    unsigned t1, t2, t3;
    std::vector<std::pair<unsigned, unsigned>> order;
    std::map<std::pair<unsigned, unsigned>, unsigned> seeds;

    std::ifstream ifs{"../seeds.dat"};
    ifs >> seed_types;
    while (ifs)
    {
        ifs >> t1 >> t2 >> t3;
        seeds[std::pair{t1, t2}] = t3;
    }
    ifs.close();

    for (const auto& el: seeds)
        order.emplace_back(el.first);  /// seeded pixels must be first. do not put this in while loop above

    for (unsigned i = 0; i < wrapper.height; ++i) {
        for (unsigned j = 0; j < wrapper.width; ++j) {
            auto temp = std::pair{i,j};
            if (seeds.find(temp) == seeds.end())
                order.emplace_back(temp);       /// now add unseeded
        }
    }

    /*auto solutions = new Eigen::MatrixXd[seed_types];*/
    Eigen::VectorXd b{order.size() - seeds.size()};
    for (unsigned cur_seed = 0; cur_seed < seed_types; ++cur_seed)
    {   /// L_u * x = b. Find x
        b.setZero();
        for (unsigned i = 0; i < b.size(); ++i) {
            b.coeffRef(i) = b_entry(i+seeds.size(), order, std::pair{wrapper.height, wrapper.width}, seeds, cur_seed, gray);
        }
    }

    wrapper.from_matrix(gray);
    wrapper.write_png_file(argv[2]);

    /*Eigen::SparseMatrix<double> sm(3,2);
    sm.insert(1, 1) = 9;
    sm.coeffRef(1, 1) = 8;
    std::cout << sm.coeffRef(1, 1);*/

    return 0;
    /// TODO: check row major/col major
}