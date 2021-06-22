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
    unsigned int m_width, m_height;
    Eigen::MatrixXd m_R;
    Eigen::MatrixXd m_G;
    Eigen::MatrixXd m_B;
    PNG(const char* filename);
    PNG() = delete;
    PNG(PNG const&) = delete;
    PNG(PNG&&) = delete;
    ~PNG() = default;
    void readPngFile();
    void writeOut(char *filename) const;

private:
    png_bytep* m_rowPointers;
    png_byte m_colorType;
    png_byte m_bitDepth;
    png_structp m_png;
    png_infop m_info;
    FILE* m_fp;
    bool m_int_range;
    void toMatrix();
    void fromMatrix() const;
    void checkRange();
};

#endif //RANDOM_WALKER_RW_IMAGE_H
