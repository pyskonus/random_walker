//
// Created by pyskonus on 16.06.21.
//

#include "../inc/main_computations.h"

double b_entry(unsigned u_node, const std::vector<std::pair<unsigned, unsigned>>& order,
               std::pair<unsigned, unsigned> img_shape, const std::map<std::pair<unsigned, unsigned>, unsigned>& seeds,
               unsigned cur_seed, const Eigen::MatrixXd& img) {
    double res = 0;
    auto adj = adjacent_nodes(u_node, order, img_shape);
    for (auto & i : adj) {
        if (seeds.find(i) != seeds.end()) {
            if (seeds.at(i) == cur_seed)
                res += weight(img, order[u_node], i);
        }
    }
    return res;
}

std::vector<std::pair<unsigned, unsigned>> adjacent_nodes(unsigned node_idx,
                                                          const std::vector<std::pair<unsigned, unsigned>>& order,
                                                          std::pair<unsigned, unsigned> img_shape)
{
    auto node = order[node_idx];
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

double weight(const Eigen::MatrixXd& img, std::pair<unsigned, unsigned> node1, std::pair<unsigned, unsigned> node2) {
    /// TODO: make it work for 3 channels
    double BETA = 2000;
    return exp(-BETA*pow(img.coeffRef(node1.first, node1.second) - img.coeffRef(node2.first, node2.second), 2));
}