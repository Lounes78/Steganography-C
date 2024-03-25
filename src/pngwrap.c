#include <stdio.h>
#include <stdlib.h>
#include <png.h>
#include "pngwrap.h"


/*****************************************************************************************************/
/*                                               RGBA Images                                         */
/*****************************************************************************************************/


rgba_image_t *E3ACreateRGBAImage()
{
    rgba_image_t *retval = (rgba_image_t *)malloc(sizeof(rgba_image_t));
    retval->width = 0;
    retval->height = 0;
    retval->data = 0;
    retval->rawdata = 0;
    return retval;
}

error_e E3AFreeRGBAImage(rgba_image_t *image)
{
    if (image->rawdata)
        free(image->rawdata);
    if (image->data)
        free(image->data);
    free(image);
    return E3A_OK;
}

error_e E3ALoadRGBAImage(const char *infile_path, rgba_image_t *image)
{
    FILE *fp = fopen(infile_path, "rb");
    if (!fp) {
        printf("No input File\n");
        return E3A_ENOFILE;
    }

    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png) {
        return E3A_EPNGLIB;
    }

    png_infop info = png_create_info_struct(png);
    if (!info) {
        return E3A_EPNGLIB;
    }

    png_init_io(png, fp);
    png_read_info(png, info);

    int width = png_get_image_width(png, info);
    int height = png_get_image_height(png, info);
    int color_type = png_get_color_type(png, info);
    int bit_depth = png_get_bit_depth(png, info);

    if (color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(png);
    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
        png_set_expand_gray_1_2_4_to_8(png);
    if (png_get_valid(png, info, PNG_INFO_tRNS))
        png_set_tRNS_to_alpha(png);
    if (color_type == PNG_COLOR_TYPE_RGB ||
        color_type == PNG_COLOR_TYPE_RGB_ALPHA)
        png_set_filler(png, 0xff, PNG_FILLER_AFTER);
    if (color_type == PNG_COLOR_TYPE_GRAY ||
        color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(png);

    png_read_update_info(png, info);

    // Allocate memory for the RGB image:
    image->width=width;
    image->height=height;
    image->data=(unsigned char**)malloc(height*sizeof(unsigned char*));
    if(!image->data){
        return E3A_ENOMEM;
    }
    image->rawdata=(unsigned char*)malloc(height*width*4*sizeof(unsigned char));
    if(!image->rawdata){
        free(image->data);
        return E3A_ENOMEM;
    }
    // Assign pointers
    for (int y = 0; y < height; y++){
        image->data[y] = image->rawdata+y*width*4*sizeof(unsigned char);
    }


    // Read RGB image
    png_read_image(png, image->data);

    return E3A_OK;
}

error_e E3ADumpRGBAImage(const char *outfile_path, const rgba_image_t *image)
{
    FILE *fp  = fopen(outfile_path, "wb");
    if (!fp) {
        return E3A_ENOFILE;
    }

    png_structp png_write = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_write) {
        return E3A_EPNGLIB;
    }

    png_infop info_write = png_create_info_struct(png_write);
    if (!info_write) {
        return E3A_EPNGLIB;
    }

    png_init_io(png_write, fp);
    png_set_IHDR(png_write, info_write, image->width, image->height, 8, PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE,
    PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

    png_write_info(png_write, info_write);
    png_write_image(png_write, image->data);
    png_write_end(png_write, NULL);

    fclose(fp);
    return E3A_OK;
}

/*****************************************************************************************************/
/*                                           Graylevel Images                                        */
/*****************************************************************************************************/


bwimage_t *E3ACreateImage()
{
    bwimage_t *retval = (bwimage_t *)malloc(sizeof(bwimage_t));
    retval->width = 0;
    retval->height = 0;
    retval->data = 0;
    retval->rawdata = 0;
    return retval;
}

error_e E3AFreeImage(bwimage_t *image)
{
    if (image->rawdata)
        free(image->rawdata);
    if (image->data)
        free(image->data);
    free(image);
    return E3A_OK;
}

error_e E3ALoadImage(const char *infile_path, bwimage_t *image){
    int rv;
    // Load RGBA image first
    rgba_image_t *rgba=E3ACreateRGBAImage();
    rv=E3ALoadRGBAImage(infile_path, rgba);
    if(rv!=E3A_OK){
        return rv;
    }

    // Allocate memory in bwimage_t
    image->width=rgba->width;
    image->height=rgba->height;
    image->rawdata=(unsigned char*)malloc((rgba->width) * (rgba->height) * sizeof(unsigned char));
    if(!image->rawdata){
        free(image);
        E3AFreeRGBAImage(rgba);
        return E3A_ENOMEM;
    }
    image->data=(unsigned char**)malloc(rgba->height*sizeof(unsigned char**));
    if(!image->data){
        free(image->rawdata);
        free(image);
        E3AFreeRGBAImage(rgba);
        return E3A_ENOMEM; 
    }

    // Assign pointers
    for (int y = 0; y < image->height; y++){
        image->data[y] = image->rawdata+y*image->width*sizeof(unsigned char);
    }

    // Transfer data from the RGB image to bwimage_t
    for(int y=0; y<rgba->height; y++){
        for(int x=0; x<rgba->width; x++){
            unsigned char* p=rgba->data[y]+4*x;
            // from RGBA to grayscale:
            image->data[y][x]=((unsigned int)p[0]+(unsigned int)p[1]+(unsigned int)p[2])/3;
        }
    }

    // Free RGB image
    E3AFreeRGBAImage(rgba);
}

error_e E3ADumpImage(const char *outfile_path, const bwimage_t *image){
    // Allocate memory for RGBA image
    rgba_image_t *rgba=E3ACreateRGBAImage();
    rgba->width=image->width;
    rgba->height=image->height;
    rgba->data=(unsigned char**)malloc(image->height*sizeof(unsigned char*));
    if(!rgba->data){
        free(rgba);
        return E3A_ENOMEM;
    }
    rgba->rawdata=(unsigned char*)malloc(image->height*image->width*4*sizeof(unsigned char*));
    if(!rgba->rawdata){
        free(rgba->data);
        free(rgba);
        return E3A_ENOMEM;
    }

    // Assign pointers
    for (int y = 0; y < rgba->height; y++){
        rgba->data[y] = rgba->rawdata+y*rgba->width*4*sizeof(unsigned char);
    }

    // Copy graylevel data to the RGBA image
    for(int y=0; y<rgba->height; y++){
        for(int x=0; x<rgba->width; x++){
            unsigned char* p=rgba->data[y]+4*x;
            // from grayscale to RGBA:
            p[0]=image->data[y][x];
            p[1]=image->data[y][x];
            p[2]=image->data[y][x];
            p[3]=0xFF; // Alpha channel
        }
    }

    // Write image to the file
    E3ADumpRGBAImage(outfile_path, rgba);
    
    // Free the RGBA data
    E3AFreeRGBAImage(rgba);
}



rgba_image_t* CreateEmptyImageLike_RGBA(unsigned int height, unsigned int width)
{
    rgba_image_t* emptyImage = E3ACreateRGBAImage();

    if (emptyImage != NULL) {
        // Set the dimensions of the empty image
        emptyImage->width = width;
        emptyImage->height = height;

        // Allocate memory for the image data
        emptyImage->data = (unsigned char**)malloc(emptyImage->height * sizeof(unsigned char*));

        if (emptyImage->data != NULL) {
            for (unsigned int i = 0; i < emptyImage->height; i++) {
                emptyImage->data[i] = (unsigned char*)malloc(4*emptyImage->width * sizeof(unsigned char));

                // Initialize the image data to some default value (e.g., 0)
                for (unsigned int j = 0; j < 4*emptyImage->width; j++) {
                    emptyImage->data[i][j] = 0;
                }
            }
        } else {
            // Handle memory allocation failure
            // Free any allocated memory and set emptyImage to NULL
            E3AFreeRGBAImage(emptyImage);
            emptyImage = NULL;
        }
    }

    return emptyImage;
}




bwimage_t* CreateEmptyImageLike(unsigned int height, unsigned int width)
{
    bwimage_t* emptyImage = E3ACreateImage();

    if (emptyImage != NULL) {
        // Set the dimensions of the empty image
        emptyImage->width = width;
        emptyImage->height = height;

        // Allocate memory for the image data
        emptyImage->data = (unsigned char**)malloc(emptyImage->height * sizeof(unsigned char*));

        if (emptyImage->data != NULL) {
            for (unsigned int i = 0; i < emptyImage->height; i++) {
                emptyImage->data[i] = (unsigned char*)malloc(emptyImage->width * sizeof(unsigned char));

                // Initialize the image data to some default value (e.g., 0)
                for (unsigned int j = 0; j < emptyImage->width; j++) {
                    emptyImage->data[i][j] = 0;
                }
            }
        } else {
            // Handle memory allocation failure
            // Free any allocated memory and set emptyImage to NULL
            E3AFreeImage(emptyImage);
            emptyImage = NULL;
        }
    }

    return emptyImage;
}
