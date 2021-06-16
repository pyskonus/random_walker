/// GPL

#ifndef RANDOM_WALKER_MAIN_COMPUTATIONS_H
#define RANDOM_WALKER_MAIN_COMPUTATIONS_H
#include <vector>
#include <map>
#include <Eigen/Core>
#include <Eigen/SparseCore>

double b_entry(unsigned, const std::vector<std::pair<unsigned, unsigned>>&, std::pair<unsigned, unsigned>,
        const std::map<std::pair<unsigned, unsigned>, unsigned>&, unsigned, const Eigen::MatrixXd&);

std::vector<std::pair<unsigned, unsigned>> adjacent_nodes(unsigned, const std::vector<std::pair<unsigned, unsigned>>&,
        std::pair<unsigned, unsigned>);

double weight(const Eigen::MatrixXd&, std::pair<unsigned, unsigned>, std::pair<unsigned, unsigned>);

Eigen::SparseMatrix<double> get_L_u(const std::vector<std::pair<unsigned, unsigned>>&,
        const std::map<std::pair<unsigned, unsigned>, unsigned>&, const Eigen::MatrixXd&,
        std::pair<unsigned, unsigned>); /// this may be rewritten in the future

bool adjacent(std::pair<unsigned, unsigned>, std::pair<unsigned, unsigned>);

#endif //RANDOM_WALKER_MAIN_COMPUTATIONS_H
