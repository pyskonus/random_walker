/// GPL

#ifndef RANDOM_WALKER_MAIN_COMPUTATIONS_H
#define RANDOM_WALKER_MAIN_COMPUTATIONS_H
#include <algorithm>
#include <vector>
#include <map>
#include <Eigen/Core>
#include <Eigen/SparseCore>
#include "rw_image.h"

double b_entry(unsigned, const std::vector<std::pair<unsigned, unsigned>>&, std::pair<unsigned, unsigned>,
        const std::map<std::pair<unsigned, unsigned>, unsigned>&, unsigned, const PNG&);

std::vector<std::pair<unsigned, unsigned>> adjacent_nodes(std::pair<unsigned, unsigned>, std::pair<unsigned, unsigned>);

double weight(const PNG&, std::pair<unsigned, unsigned>, std::pair<unsigned, unsigned>);

Eigen::SparseMatrix<double> get_L_u(const std::vector<std::pair<unsigned, unsigned>>&,
        const std::map<std::pair<unsigned, unsigned>, unsigned>&, const PNG&); /// this may be rewritten in the future

#endif //RANDOM_WALKER_MAIN_COMPUTATIONS_H
