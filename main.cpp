/// GPL

#include <cstdlib>
#include <iostream>
#include <Eigen/SparseCore>
#include <Eigen/Core>
#include <Eigen/SparseLU>
#include <Eigen/IterativeLinearSolvers>
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

    auto solutions = new Eigen::MatrixXd[seed_types];
    Eigen::VectorXd x{order.size() - seeds.size()};
    Eigen::VectorXd b{order.size() - seeds.size()};
    Eigen::ConjugateGradient<Eigen::SparseMatrix<double>> solver;   /// TODO: try different solver
    for (unsigned cur_seed = 0; cur_seed < seed_types; ++cur_seed)
    {   /// L_u * x = b. Find x
        b.setZero();
        for (unsigned i = 0; i < b.size(); ++i) {
            b.coeffRef(i) = b_entry(i+seeds.size(), order, std::pair{wrapper.height, wrapper.width}, seeds, cur_seed, gray);
        }
        auto L_u = get_L_u(order, seeds, gray, std::pair<unsigned, unsigned>{wrapper.height, wrapper.width});
        solver.compute(L_u);
        x = solver.solve(b);

        solutions[cur_seed] = Eigen::MatrixXd{wrapper.height, wrapper.width};
        for (const auto& el: seeds) {
            if (el.second == cur_seed)
                solutions[cur_seed].coeffRef(el.first.first, el.first.second) = 1;
            else
                solutions[cur_seed].coeffRef(el.first.first, el.first.second) = 0;
        }
        for (unsigned i = seeds.size(); i < order.size(); ++i) {
            solutions[cur_seed].coeffRef(order[i].first, order[i].second) = x.coeffRef(i-seeds.size());
        }
    }

    /// form final image
    unsigned idx_max = 0;
    double prob_max = 0;
    for (unsigned i = 0; i < wrapper.height; ++i) {
        for (unsigned j = 0; j < wrapper.width; ++j) {
            for (unsigned k = 0; k < seed_types; ++k) {
                if (solutions[k].coeffRef(i, j) > prob_max) {
                    idx_max = k;
                    prob_max = solutions[k].coeffRef(i, j);
                }
            }
            gray.coeffRef(i, j) = double(idx_max)/seed_types;
            idx_max = 0; prob_max = 0;
        }
    }


    wrapper.from_matrix(gray);
    wrapper.write_png_file(argv[2]);
    return 0;
    /// TODO: check row major/col major
}