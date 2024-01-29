#ifndef __E3A_PNGWRAP_H__
#define __E3A_PNGWRAP_H__

#ifdef __cplusplus
extern "C"
{
#endif

  typedef enum
  {
    E3A_OK,
    E3A_ENOFILE,
    E3A_EPNGLIB,
    E3A_ENOMEM
  } error_e;

  typedef struct
  {
    unsigned int width;
    unsigned int height;
    unsigned char **data; /* Access rgba pixels as image.data[row][col*4+i] where i=0, 1, 2, 3 for R, G, B and alpha */
    unsigned char *rawdata;
  } rgba_image_t;

  /* Creates the structure on the heap and initialize it to the defaults */
  rgba_image_t *E3ACreateRGBAImage();

  /* Loads image from the open tiff file. Data are allocated on the heap. */
  error_e E3ALoadRGBAImage(const char *infile_path, rgba_image_t *image);

  /* Dumps the image in the new tiff file */
  error_e E3ADumpRGBAImage(const char *outfile_path, const rgba_image_t *image);

  /* Deallocates the resources */
  error_e E3AFreeRGBAImage(rgba_image_t *image); 



  typedef struct
  {
    unsigned int width;
    unsigned int height;
    unsigned char **data; /* Access intensities as image.data[row][col] */
    unsigned char *rawdata;
  } bwimage_t;

  /* Creates the structure on the heap and initialize it to the defaults */
  bwimage_t *E3ACreateImage();

  /* Loads image from the open tiff file. Data are allocated on the heap. */
  error_e E3ALoadImage(const char *infile_path, bwimage_t *image);

  /* Dumps the image in the new tiff file */
  error_e E3ADumpImage(const char *outfile_path, const bwimage_t *image);

  /* Deallocates the resources */
  error_e E3AFreeImage(bwimage_t *image);


  bwimage_t* CreateEmptyImageLike(bwimage_t* referenceImage);


#ifdef __cplusplus
}
#endif

#endif //__E3A_PNGWRAP_H__
