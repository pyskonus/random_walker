/// GPL

#ifndef RANDOM_WALKER_RW_IMAGE_H
#define RANDOM_WALKER_RW_IMAGE_H

#include <cstdlib>
#include <cstdio>
#include "png.h"
#include <iostream>
#include <Eigen/Core>

class PNG
{
public:
    unsigned int width, height;
    Eigen::MatrixXd R;
    Eigen::MatrixXd G;
    Eigen::MatrixXd B;
    PNG(const char* filename);
    PNG() = delete;
    PNG(PNG const&) = delete;
    PNG(PNG&&) = delete;
    ~PNG() = default;
    void read_png_file();
    void write_out(char *filename) const;

private:
    png_bytep* row_pointers;
    png_byte color_type;
    png_byte bit_depth;
    png_structp png;
    png_infop info;
    FILE* fp;
    void form_matrix();
    void from_matrix() const;
};

#endif //RANDOM_WALKER_RW_IMAGE_H
