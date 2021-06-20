/// GPL

#include "../inc/main_computations.h"

double b_entry(unsigned u_node, const std::vector<std::pair<unsigned, unsigned>>& order,
               std::pair<unsigned, unsigned> img_shape, const std::map<std::pair<unsigned, unsigned>, unsigned>& seeds,
               unsigned cur_seed, const PNG& wrapper) {
    double res = 0;
    auto adj = adjacent_nodes(order[u_node], img_shape);
    for (auto & i : adj) {
        if (seeds.find(i) != seeds.end()) {
            if (seeds.at(i) == cur_seed)
                res += weight(wrapper, order[u_node], i);
        }
    }
    return res;
}

std::vector<std::pair<unsigned, unsigned>> adjacent_nodes(std::pair<unsigned, unsigned> node,
                                                          std::pair<unsigned, unsigned> img_shape)
{
    std::vector<std::pair<unsigned, unsigned>> res;

    if (node.first != 0)
        res.emplace_back(std::pair<unsigned, unsigned>{node.first - 1, node.second});
    if (node.second != 0)
        res.emplace_back(std::pair<unsigned, unsigned>{node.first, node.second - 1});
    if (node.first != img_shape.first - 1)
        res.emplace_back(std::pair<unsigned, unsigned>{node.first + 1, node.second});
    if (node.second != img_shape.second - 1)
        res.emplace_back(std::pair<unsigned, unsigned>{node.first, node.second + 1});

    return res;
}

double weight(const PNG& wrapper, std::pair<unsigned, unsigned> node1, std::pair<unsigned, unsigned> node2) {
    double BETA = 2000; /// TODO: beta parameter
    double sq_diff_mean =
    (pow(wrapper.R.coeffRef(node1.first, node1.second) - wrapper.R.coeffRef(node2.first, node2.second), 2)+
     pow(wrapper.G.coeffRef(node1.first, node1.second) - wrapper.G.coeffRef(node2.first, node2.second), 2)+
     pow(wrapper.B.coeffRef(node1.first, node1.second) - wrapper.B.coeffRef(node2.first, node2.second), 2))/3;
    return exp(-BETA*sq_diff_mean);
}

Eigen::SparseMatrix<double> get_L_u(const std::vector<std::pair<unsigned, unsigned>>& order,
                        const std::map<std::pair<unsigned, unsigned>, unsigned>& seeds, const PNG& wrapper)
{
    unsigned l = seeds.size();
    Eigen::Index side = order.size()-l;
    Eigen::SparseMatrix<double/*, Eigen::RowMajor*/> res{side, side};   /// TODO: matrix major
    res.setZero();

    for (unsigned i = l; i < order.size(); ++i) {
        for (unsigned j = l; j < order.size(); ++j) {
            if (i==j) {
                auto adj = adjacent_nodes(order[i], std::pair{wrapper.height, wrapper.width});
                for (const auto& el: adj)
                    res.coeffRef(i-l, j-l) += weight(wrapper, order[i], el);
            } else if (adjacent(order[i], order[j])) {
                res.coeffRef(i-l, j-l) = -weight(wrapper, order[i], order[j]);
            }
        }
    }
    return res;
}

bool adjacent(std::pair<unsigned, unsigned> node1, std::pair<unsigned, unsigned> node2)
{
    if (node1.first == node2.first) {
        if (node1.second == node2.second + 1 || node1.second == node2.second - 1)
            return true;
    }
    if (node1.second == node2.second) {
        if (node1.first == node2.first + 1 || node1.first == node2.first - 1)
            return true;
    }
    return false;
}