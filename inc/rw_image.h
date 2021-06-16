//
// Created by pyskonus on 16.06.21.
//

#ifndef RANDOM_WALKER_RW_IMAGE_H
#define RANDOM_WALKER_RW_IMAGE_H

#include <cstdlib>
#include <cstdio>
#include <png.h>
#include <iostream>
#include <Eigen/Dense>

class PNG
{
public:
    unsigned int width, height;
    png_bytep* row_pointers;
    PNG(const char* filename);
    PNG() = delete;
    PNG(PNG const&) = delete;
    PNG(PNG&&) = delete;
    ~PNG() = default;
    void read_png_file();
    void write_png_file(char *filename) const;
    void process_png_file() const;  /// this function is in fact unnecessary
    void form_matrix(Eigen::MatrixXd& mat) const;
    void from_matrix(const Eigen::MatrixXd& mat) const;

private:
    png_byte color_type;
    png_byte bit_depth;
    png_structp png;
    png_infop info;
    FILE* fp;
};

#endif //RANDOM_WALKER_RW_IMAGE_H
