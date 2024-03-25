#include <stdio.h>
#include <stdlib.h>
#include "pngwrap.h"
#include "bitstream.h"

int main(int argc, char **argv)
{
	unsigned int seed = 45;
	//printf("Inserer la cle d'encodage: ");
	//scanf("%d", &seed);
	
	// bwimage_t *host_image;
	// bwimage_t *secret_image;
	// bwimage_t *decoded_image;



	rgba_image_t *host_image;
	rgba_image_t *secret_image;


	error_e retval_host = E3A_OK;
	error_e retval_secret = E3A_OK;


	host_image = E3ACreateRGBAImage();
	secret_image = E3ACreateRGBAImage();


	retval_host = E3ALoadRGBAImage("../img/saturne.png", host_image);
	retval_secret = E3ALoadRGBAImage("../img/lena.png", secret_image);




	// host_image = E3ACreateImage();
	// secret_image = E3ACreateImage();

	// retval_host = E3ALoadImage("../img/tree3.png", host_image);
	// retval_secret = E3ALoadImage("../img/tree2.png", secret_image);


	CanIHandleIt(host_image, secret_image);
   	
	unsigned int size_imageSecret = 4*secret_image->width * secret_image->height;

	bitstream *secret_stream = CreateBitstreamOnData((char*)0, size_imageSecret);

	
	// Preprocessing the secret data
	printf("Preprocessing ...\n");
	//PrepSecImg(secret_image, secret_stream, seed);
	PrepSecImg_RGB(secret_image, secret_stream, seed);

	// Coding
	printf("Coding ...\n");
	//HeDoesTheJob(host_image, secret_stream);
     HeDoesTheJob_Rand(host_image, secret_stream, seed);	




	printf("Coded Image Generation ...\n");
	//E3ADumpImage("../img/coded.png", host_image);
	E3ADumpRGBAImage("../img/coded.png", host_image);
	printf("Generated.\n\n");



	// Decoding
	printf("Decoding ...\n");
	//ReverseEngineerTheJob(host_image, decoded_image);
	ReverseEngineerTheJob_Rand(host_image, seed, secret_image);

	printf("Decoded.\n");

	


	FreeBitstream(secret_stream);
	E3AFreeRGBAImage(host_image);
	E3AFreeRGBAImage(secret_image);	

	// E3AFreeImage(host_image);
	// E3AFreeImage(secret_image);
	
	return 0;
}