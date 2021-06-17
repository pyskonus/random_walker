/// GPL

#include "../inc/rw_image.h"

PNG::PNG(const char *filename)
{
    fp = fopen(filename, "rb");

    png = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if(!png) abort();
    info = png_create_info_struct(png);
    if(!info) abort();
    png_init_io(png, fp);

    png_read_info(png, info);


    width      = png_get_image_width(png, info);
    height     = png_get_image_height(png, info);
    color_type = png_get_color_type(png, info);
    bit_depth  = png_get_bit_depth(png, info);

    row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * height);
    for(unsigned y = 0; y < height; y++) {
        row_pointers[y] = (png_byte*)malloc(png_get_rowbytes(png,info));
    }
    R = Eigen::MatrixXd{height, width};
    G = Eigen::MatrixXd{height, width};
    B = Eigen::MatrixXd{height, width};
}

void PNG::read_png_file()
{
    /*if(setjmp(png_jmpbuf(png))) abort();*/

    // Read any color_type into 8bit depth, RGBA format.
    // See http://www.libpng.org/pub/png/libpng-manual.txt

    if(bit_depth == 16)
        png_set_strip_16(png);

    if(color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(png);

    // PNG_COLOR_TYPE_GRAY_ALPHA is always 8 or 16bit depth.
    if(color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
        png_set_expand_gray_1_2_4_to_8(png);

    if(png_get_valid(png, info, PNG_INFO_tRNS))
        png_set_tRNS_to_alpha(png);

    // These color_type don't have an alpha channel then fill it with 0xff.
    if(color_type == PNG_COLOR_TYPE_RGB ||
       color_type == PNG_COLOR_TYPE_GRAY ||
       color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_filler(png, 0xFF, PNG_FILLER_AFTER);

    if(color_type == PNG_COLOR_TYPE_GRAY ||
       color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(png);

    png_read_update_info(png, info);

    png_read_image(png, row_pointers);
    form_matrix();

    png_destroy_read_struct(&png, &info, nullptr);
    fclose(fp);     /// could close in destructor tho
}

void PNG::write_out(char *filename) const
{
    FILE *fpw = fopen(filename, "wb");
    if(!fpw) abort();

    png_structp png1 = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png1) abort();

    png_infop info1 = png_create_info_struct(png1);
    if (!info1) abort();

    /*if (setjmp(png_jmpbuf(png))) abort();*/

    png_init_io(png1, fpw);

    // Output is 8bit depth, RGBA format.
    png_set_IHDR(
            png1,
            info1,
            width, height,
            8,
            PNG_COLOR_TYPE_RGBA,
            PNG_INTERLACE_NONE,
            PNG_COMPRESSION_TYPE_DEFAULT,
            PNG_FILTER_TYPE_DEFAULT
    );
    png_write_info(png1, info1);

    // To remove the alpha channel for PNG_COLOR_TYPE_RGB format,
    // Use png_set_filler().
    //png_set_filler(png, 0, PNG_FILLER_AFTER);

    if (!row_pointers) abort();

    from_matrix();
    png_write_image(png1, row_pointers);
    png_write_end(png1, nullptr);

    for(unsigned k = 0; k < height; k++) {
        free(row_pointers[k]);
    }
    free(row_pointers);

    fclose(fpw);

    png_destroy_write_struct(&png1, &info1);
}

void PNG::form_matrix()
{
    for(unsigned y = 0; y < height; y++) {
        png_bytep row = row_pointers[y];
        for(unsigned x = 0; x < width; x++) {
            png_bytep px = &(row[x * 4]);
            R.coeffRef(y,x) = *px/255.0;
            G.coeffRef(y,x) = *(px+1)/255.0;
            B.coeffRef(y,x) = *(px+2)/255.0;
        }
    }
}

void PNG::from_matrix() const
{
    for(unsigned y = 0; y < height; y++) {
        png_bytep row = row_pointers[y];
        for(unsigned x = 0; x < width; x++) {
            png_bytep px = &(row[x * 4]);
            *px = round(R.coeffRef(y, x) * 255);
            *(px+1) = round(G.coeffRef(y, x) * 255);
            *(px+2) = round(B.coeffRef(y, x) * 255);
        }
    }
}