//
// Created by pyskonus on 16.06.21.
//

#ifndef RANDOM_WALKER_RW_IMAGE_H
#define RANDOM_WALKER_RW_IMAGE_H

#include <cstdlib>
#include <cstdio>
#include <png.h>
#include <iostream>

void read_meta(FILE* fp, unsigned int& width, unsigned int& height, png_byte& color_type, png_byte& bit_depth, const png_structp& png, const png_infop& info);

void read_png_file(FILE* fp, png_bytep *row_pointers, unsigned int& width, unsigned int& height, png_byte& color_type, png_byte& bit_depth, png_structp& png, png_infop& info);

void write_png_file(char *filename, png_bytep *row_pointers, unsigned int width, unsigned int height, png_byte& bit_depth);

void process_png_file(png_bytep *row_pointers, unsigned int width, unsigned int height);

#endif //RANDOM_WALKER_RW_IMAGE_H
