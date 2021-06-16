#include <cstdlib>
#include <cstdio>
#include <png.h>
#include <iostream>
#include <Eigen/Dense>
#include "inc/rw_image.h"

int main(int argc, char *argv[]) {
    if(argc != 3) {std::cerr << "invalid number of arguments. Must be 2" << std::endl; abort();}

    unsigned int width, height;
    png_byte color_type;
    png_byte bit_depth;
    FILE* infile = fopen(argv[1], "rb");

    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if(!png) abort();
    png_infop info = png_create_info_struct(png);
    if(!info) abort();

    read_meta(infile, width, height, color_type, bit_depth, png, info);
    auto row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * height);
    for(int y = 0; y < height; y++) {
        row_pointers[y] = (png_byte*)malloc(png_get_rowbytes(png,info));
    }
    read_png_file(infile, row_pointers, width, height, color_type, bit_depth, png, info);
    process_png_file(row_pointers, width, height);
    write_png_file(argv[2], row_pointers, width, height, bit_depth);
    fclose(infile);     /// seems to work ok without this line tho

    return 0;
}