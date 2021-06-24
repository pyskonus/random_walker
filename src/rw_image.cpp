/// GPL

#include "../inc/rw_image.h"

PNG::PNG(const char *filename) : m_int_range(false)
{
    m_fp = fopen(filename, "rb");

    m_png = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if(!m_png) abort();
    m_info = png_create_info_struct(m_png);
    if(!m_info) abort();
    png_init_io(m_png, m_fp);

    png_read_info(m_png, m_info);


    m_width     = png_get_image_width(m_png, m_info);
    m_height    = png_get_image_height(m_png, m_info);
    m_colorType = png_get_color_type(m_png, m_info);
    m_bitDepth  = png_get_bit_depth(m_png, m_info);

    m_rowPointers = (png_bytep*)malloc(sizeof(png_bytep) * m_height);
    for(unsigned y = 0; y < m_height; y++)
    {
        m_rowPointers[y] = (png_byte*)malloc(png_get_rowbytes(m_png, m_info));
    }
    m_R = Eigen::MatrixXd{m_height, m_width};
    m_G = Eigen::MatrixXd{m_height, m_width};
    m_B = Eigen::MatrixXd{m_height, m_width};
}

void PNG::readPngFile()
{
    /*if(setjmp(png_jmpbuf(m_png))) abort();*/
    if(m_bitDepth == 16)
        png_set_strip_16(m_png);

    if(m_colorType == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(m_png);

    // PNG_COLOR_TYPE_GRAY_ALPHA is always 8 or 16bit depth.
    if(m_colorType == PNG_COLOR_TYPE_GRAY && m_bitDepth < 8)
        png_set_expand_gray_1_2_4_to_8(m_png);

    if(png_get_valid(m_png, m_info, PNG_INFO_tRNS))
        png_set_tRNS_to_alpha(m_png);

    // These m_colorType don't have an alpha channel then fill it with 0xff.
    if(m_colorType == PNG_COLOR_TYPE_RGB ||
       m_colorType == PNG_COLOR_TYPE_GRAY ||
       m_colorType == PNG_COLOR_TYPE_PALETTE)
        png_set_filler(m_png, 0xFF, PNG_FILLER_AFTER);

    if(m_colorType == PNG_COLOR_TYPE_GRAY ||
       m_colorType == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(m_png);

    png_read_update_info(m_png, m_info);

    png_read_image(m_png, m_rowPointers);
    checkRange();
    toMatrix();

    png_destroy_read_struct(&m_png, &m_info, nullptr);
    fclose(m_fp);     /// could close in destructor tho
}

void PNG::writeOut(char *filename) const
{
    FILE *fpw = fopen(filename, "wb");
    if(!fpw) abort();

    png_structp png1 = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png1) abort();

    png_infop info1 = png_create_info_struct(png1);
    if (!info1) abort();

    /*if (setjmp(png_jmpbuf(m_png))) abort();*/

    png_init_io(png1, fpw);

    // Output is 8bit depth, RGBA format.
    png_set_IHDR(
            png1,
            info1,
            m_width, m_height,
            8,
            PNG_COLOR_TYPE_RGBA,
            PNG_INTERLACE_NONE,
            PNG_COMPRESSION_TYPE_DEFAULT,
            PNG_FILTER_TYPE_DEFAULT
    );
    png_write_info(png1, info1);

    // To remove the alpha channel for PNG_COLOR_TYPE_RGB format,
    // Use png_set_filler().
    //png_set_filler(m_png, 0, PNG_FILLER_AFTER);

    if (!m_rowPointers) abort();

    fromMatrix();
    png_write_image(png1, m_rowPointers);
    png_write_end(png1, nullptr);

    for(unsigned k = 0; k < m_height; k++)
    {
        free(m_rowPointers[k]);
    }
    free(m_rowPointers);

    fclose(fpw);

    png_destroy_write_struct(&png1, &info1);
}

void PNG::toMatrix()
{
    for(unsigned y = 0; y < m_height; y++)
    {
        png_bytep row = m_rowPointers[y];
        for(unsigned x = 0; x < m_width; x++)
        {
            png_bytep px = &(row[x * 4]);
            if (m_int_range)
            {
                m_R.coeffRef(y, x) = *px / 255.0;
                m_G.coeffRef(y, x) = *(px + 1) / 255.0;
                m_B.coeffRef(y, x) = *(px + 2) / 255.0;
            } else
            {
                m_R.coeffRef(y, x) = *px;
                m_G.coeffRef(y, x) = *(px + 1);
                m_B.coeffRef(y, x) = *(px + 2);
            }
        }
    }
}

void PNG::fromMatrix() const
{
    for(unsigned y = 0; y < m_height; y++)
    {
        png_bytep row = m_rowPointers[y];
        for(unsigned x = 0; x < m_width; x++)
        {
            png_bytep px = &(row[x * 4]);
            *px = round(m_R.coeffRef(y, x) * 255);
            *(px+1) = round(m_G.coeffRef(y, x) * 255);
            *(px+2) = round(m_B.coeffRef(y, x) * 255);
        }
    }
}

void PNG::checkRange()
{
    for(unsigned y = 0; y < m_height; y++)
    {
        png_bytep row = m_rowPointers[y];
        for(unsigned x = 0; x < m_width; x++)
        {
            png_bytep px = &(row[x * 4]);
            if (*px > 1 || *(px+1) > 1 || *(px+2) > 1)
            {
                m_int_range = true;
                return;
            }
        }
    }
}