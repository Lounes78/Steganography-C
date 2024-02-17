#include <stdio.h>
#include <stdlib.h>
#include "pngwrap.h"
#include "bitstream.h"

int main(int argc, char **argv)
{
	unsigned int seed = 45;
	// printf("Inserer la cle d'encodage: \n");
	// scanf("%d", &seed);
	


	bwimage_t *host_image;
	bwimage_t *secret_image;

	
	error_e retval_host = E3A_OK;
	error_e retval_secret = E3A_OK;

	host_image = E3ACreateImage();
	secret_image = E3ACreateImage();


	retval_host = E3ALoadImage("../img/tree3.png", host_image);
	retval_secret = E3ALoadImage("../img/tree.png", secret_image);





	// cheat code
	bwimage_t* decoded_image = CreateEmptyImageLike(secret_image);



	CanIHandleIt(host_image, secret_image);
   	
	unsigned int size_imageSecret = secret_image->width * secret_image->height;

	bitstream *secret_stream = CreateBitstreamOnData((char*)0, size_imageSecret);

	
	
	// Preprocessing the secret data
	printf("Preprocessing ...\n");
	PrepSecImg(secret_image, secret_stream, seed);

	// Coding
	printf("Coding ...\n");
//	HeDoesTheJob(host_image, secret_stream);
	HeDoesTheJob_Rand(host_image, secret_stream, seed);	
	
//	printf("8*%d*%d = %d | Position is %d \n", secret_image->width, secret_image->height, 8*size_imageSecret, secret_stream->position);
  

	printf("Coded Image Generation ...\n");
	E3ADumpImage("../img/coded.png", host_image);
	printf("Generated.\n\n");

	// Decoding
	printf("Decoding ...\n");
//	ReverseEngineerTheJob(host_image, decoded_image);
	ReverseEngineerTheJob_Rand(host_image, decoded_image, seed);


	printf("Decoded Image Generation ...\n");
	E3ADumpImage("../img/decoded.png", decoded_image);
	printf("Decoded.\n");



	FreeBitstream(secret_stream);
	E3AFreeImage(host_image);
	E3AFreeImage(secret_image);
	E3AFreeImage(decoded_image);
	
	return 0;
}