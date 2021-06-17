/// GPL

#include <cstdlib>
#include <iostream>
#include <Eigen/SparseCore>
#include <Eigen/Core>
/*#include <Eigen/SparseLU>*/
#include <Eigen/IterativeLinearSolvers>
#include <vector>
#include <map>
#include <fstream>
#include <thread>
#include "inc/rw_image.h"
#include "inc/main_computations.h"

void main_alg(Eigen::VectorXd* xs, unsigned cur_seed, const std::vector<std::pair<unsigned, unsigned>>& order,
              const std::map<std::pair<unsigned, unsigned>, unsigned>& seeds, const PNG& wrapper) {
    /// L_u * x = b. Find x
    Eigen::ConjugateGradient/*SparseLU*/<Eigen::SparseMatrix<double>> solver;   /// TODO: solver type
    Eigen::VectorXd b{order.size() - seeds.size()};
    b.setZero();
    for (unsigned i = 0; i < b.size(); ++i) {
        b.coeffRef(i) = b_entry(i+seeds.size(), order, std::pair{wrapper.height, wrapper.width}, seeds, cur_seed, wrapper);
    }
    auto L_u = get_L_u(order, seeds, wrapper);
    solver.compute(L_u);
    xs[cur_seed] = solver.solve(b);
}

void form_final_img(const std::map<std::pair<unsigned, unsigned>, unsigned>& seeds, PNG& wrapper, unsigned seed_types,
                    Eigen::VectorXd* xs, const std::vector<std::pair<unsigned, unsigned>>& order) {
    /// set seeded
    for (const auto& coord_st: seeds) {
        wrapper.R.coeffRef(coord_st.first.first, coord_st.first.second) = double(coord_st.second)/seed_types;
        wrapper.G.coeffRef(coord_st.first.first, coord_st.first.second) = double(coord_st.second)/seed_types;
        wrapper.B.coeffRef(coord_st.first.first, coord_st.first.second) = double(coord_st.second)/seed_types;
    }
    /// set unseeded
    unsigned idx_max = 0;
    double prob_max = 0;
    auto l = seeds.size();
    for (unsigned uns_idx = 0; uns_idx < xs[0].size(); ++uns_idx) {
        for (unsigned st = 0; st < seed_types; ++st) {
            if (xs[st].coeffRef(uns_idx) > prob_max) {
                idx_max = st;
                prob_max = xs[st].coeffRef(uns_idx);
            }
        }
        wrapper.R.coeffRef(order[l+uns_idx].first, order[l+uns_idx].second) = double(idx_max)/seed_types;
        wrapper.G.coeffRef(order[l+uns_idx].first, order[l+uns_idx].second) = double(idx_max)/seed_types;
        wrapper.B.coeffRef(order[l+uns_idx].first, order[l+uns_idx].second) = double(idx_max)/seed_types;
        idx_max = 0; prob_max = 0;
    }
}
int main(int argc, char *argv[]) {
    if(argc != 3) {std::cerr << "invalid number of arguments. Must be 2" << std::endl; abort();}

    PNG wrapper{argv[1]};
    wrapper.read_png_file();

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

    auto xs = new Eigen::VectorXd[seed_types];
    std::vector<std::thread> threads{seed_types};
    for (unsigned cur_seed = 0; cur_seed < seed_types; ++cur_seed)
        threads[cur_seed] = std::thread{main_alg, xs, cur_seed, std::ref(order), std::ref(seeds), std::ref(wrapper)};

    for (unsigned cur_seed = 0; cur_seed < seed_types; ++cur_seed)
        threads[cur_seed].join();

    form_final_img(seeds, wrapper, seed_types, xs, order);

    delete[] xs;

    wrapper.write_out(argv[2]);
    return 0;
}