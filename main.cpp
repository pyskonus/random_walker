/// GPL

#include <cstdlib>
#include <iostream>
#include <Eigen/SparseCore>
#include <Eigen/Core>
#include <Eigen/SparseLU>
/*#include <Eigen/IterativeLinearSolvers>*/
#include <vector>
#include <map>
#include <thread>
#include "inc/rw_image.h"
#include "inc/main_computations.h"

void main_alg(Eigen::VectorXd& X, const std::vector<std::pair<unsigned, unsigned>>& order,
              const std::map<std::pair<unsigned, unsigned>, unsigned>& seeds, const PNG& wrapper)
{
    /// L_u * x = b. Find x
    Eigen::/*ConjugateGradient*/SparseLU<Eigen::SparseMatrix<double>> solver;   /// TODO: solver type
    Eigen::VectorXd b{order.size() - seeds.size()};
    b.setZero();
    for (unsigned i = 0; i < b.size(); ++i)
    {
        b.coeffRef(i) = b_entry(i+seeds.size(), order, std::pair{wrapper.m_height, wrapper.m_width}, seeds, 0, wrapper);
    }
    auto L_u = get_L_u(order, seeds, wrapper);
    solver.compute(L_u);
    X = solver.solve(b);
}

void form_final_img(PNG& image, const PNG& mask, const Eigen::VectorXd& X, unsigned ss,
                    const std::vector<std::pair<unsigned, unsigned>>& order)
{
    /// set seeded
    for (unsigned y = 0; y < image.m_height; ++y)
    {
        for (unsigned x = 0; x < image.m_width; ++x)
        {
            if (mask.m_R.coeffRef(x, y) == 0)   /// definitely background
            {
                image.m_R.coeffRef(x, y) = 0;
                image.m_G.coeffRef(x, y) = 0;
                image.m_B.coeffRef(x, y) = 0;
            }
        }
    }
    /// set unseeded
    for (unsigned x_idx = 0; x_idx < X.size(); ++x_idx)
    {
        if (X.coeffRef(x_idx) > 0.5)
        {
            image.m_R.coeffRef(order[x_idx + ss].first, order[x_idx + ss].second) = 0;
            image.m_G.coeffRef(order[x_idx + ss].first, order[x_idx + ss].second) = 0;
            image.m_B.coeffRef(order[x_idx + ss].first, order[x_idx + ss].second) = 0;
        }
    }
}

int main(int argc, char *argv[])
{
    if(argc != 4) {std::cerr << "invalid number of arguments. Must be 3" << std::endl; abort();}

    PNG image{argv[1]};
    image.readPngFile();
    PNG mask{argv[2]};
    mask.readPngFile();

    /*unsigned seed_types = 2;*/
    std::vector<std::pair<unsigned, unsigned>> order;
    std::map<std::pair<unsigned, unsigned>, unsigned> seeds;

    /// initialize seeds
    for (unsigned y = 0; y < mask.m_height; ++y)
    {
        for (unsigned x = 0; x < mask.m_width; ++x)
        {
            if (mask.m_R.coeffRef(x, y) == 1) /// white
            {
                for (const auto& neighbour: adjacent_nodes(std::pair<unsigned, unsigned>{x,y},
                                                           std::pair{mask.m_height, mask.m_width}))
                {
                    if (mask.m_R.coeffRef(neighbour.first, neighbour.second) != 0 && mask.m_R.coeffRef(neighbour.first, neighbour.second) != 1)
                        seeds[std::pair<unsigned, unsigned>{x,y}] = 0;  /// 0 means it's the thing
                }
            } else if (mask.m_R.coeffRef(x, y) == 0)  /// black
            {
                for (const auto& neighbour: adjacent_nodes(std::pair<unsigned, unsigned>{x,y},
                                                           std::pair{mask.m_height, mask.m_width}))
                {
                    if (mask.m_R.coeffRef(neighbour.first, neighbour.second) != 0 && mask.m_R.coeffRef(neighbour.first, neighbour.second) != 1)
                        seeds[std::pair<unsigned, unsigned>{x,y}] = 1;  /// 1 means it's the background
                }
            }
        }
    }

    /// initualize order
    for (const auto& el: seeds)
        order.emplace_back(el.first);  /// seeded pixels must be first

    for (unsigned i = 0; i < image.m_height; ++i)
    {
        for (unsigned j = 0; j < image.m_width; ++j)
        {
            auto temp = std::pair{i,j};
            if (seeds.find(temp) == seeds.end())
                order.emplace_back(temp);       /// now add unseeded
        }
    }

    /// main algorithm
    auto X = Eigen::VectorXd{};
    main_alg(X, std::ref(order), std::ref(seeds), std::ref(image));
    form_final_img(image, mask, X, seeds.size(), order);
    image.writeOut(argv[3]);
    return 0;
}
