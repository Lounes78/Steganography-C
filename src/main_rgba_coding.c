#include <stdio.h>
#include <stdlib.h>
#include "pngwrap.h"
#include "bitstream.h"

int main(int argc, char **argv)
{
	unsigned int seed = atoi(argv[3]);

	rgba_image_t *host_image;
	rgba_image_t *secret_image;


	error_e retval_host = E3A_OK;
	error_e retval_secret = E3A_OK;


	host_image = E3ACreateRGBAImage();
	secret_image = E3ACreateRGBAImage();


	retval_host = E3ALoadRGBAImage(argv[1], host_image);
	retval_secret = E3ALoadRGBAImage(argv[2], secret_image);

	int neededSpace = (int)8*secret_image->width*secret_image->height + 3*32; //3*32 is for metaData
    int actualSpace = (int)host_image->width*host_image->height;
    
    float loseRate = neededSpace/(float)actualSpace;
    // char user = 0;
    if (actualSpace < neededSpace)
    {
        printf("Image Secrete trop grande\n");
        printf("Taux de perte = %.2f%%\n", 100*loseRate);
		return 1;
        // printf("Etes vous sur de vouloir continuer ?\n");

        // printf("Y/N : ");
        // scanf("%c", &user);

        // if (user == 'Y')
        // {
        //     printf("Tres Bien\n\n");
        // }
        // else
        // {
        //     printf("Reessayer avec une autre image hote \n");
        //     exit(0);
        // }
    }





//	CanIHandleItRGBA(host_image, secret_image);


	unsigned int size_imageSecret = 4*secret_image->width * secret_image->height;

	bitstream *secret_stream = CreateBitstreamOnData((char*)0, size_imageSecret);

	
	// Preprocessing the secret data
//	printf("Preprocessing ...\n");
	PrepSecImg_RGB(secret_image, secret_stream, seed);

	// Coding
//	printf("Coding ...\n");
	HeDoesTheJob_RGBA_Rand(host_image, secret_stream, seed);	


//	printf("Coded Image Generation ...\n");
	E3ADumpRGBAImage("coded.png", host_image);
//	printf("Generated.\n\n");


	FreeBitstream(secret_stream);
	E3AFreeRGBAImage(host_image);
	E3AFreeRGBAImage(secret_image);	
	return 0;
}