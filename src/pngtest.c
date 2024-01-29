#include <stdio.h>
#include <stdlib.h>
#include "pngwrap.h"
#include "bitstream.h"

int main(int argc, char **argv)
{
	bwimage_t *host_image;
	bwimage_t *secret_image;
	bwimage_t *decoded;

	error_e retval_host = E3A_OK;
	error_e retval_secret = E3A_OK;

	host_image = E3ACreateImage();
	secret_image = E3ACreateImage();
	
	retval_host = E3ALoadImage("../img/tree3.png", host_image);
	retval_secret = E3ALoadImage("../img/saturne.png", secret_image);


	CanIHandleIt(host_image, secret_image);

	bwimage_t* decoded_image = CreateEmptyImageLike(secret_image);


	unsigned int size_imageSecret = secret_image->width * secret_image->height;

	bitstream *secret_stream = CreateBitstreamOnData((char*)0, size_imageSecret);

	// Preprocessing the secret data
	printf("Preprocessing ...\n");
	PrepSecImg(secret_image, secret_stream);

	// Coding
	printf("Coding ...\n");
	HeDoesTheJob(host_image, secret_stream);	
	printf("8*%d*%d = %d | Position is %d \n", secret_image->width, secret_image->height, 8*size_imageSecret, secret_stream->position);
  
	printf("Coded Image Generation ...\n");
	E3ADumpImage("../img/coded.png", host_image);


	// Decoding
	printf("Decoding ...\n");
	ReverseEngineerTheJob(host_image, decoded_image);

	printf("Si tu vois ca c que ca a marche well at least un peu\n");



	printf("Decoded Image Generation ...\n");
	E3ADumpImage("../img/decoded.png", decoded_image);




	FreeBitstream(secret_stream);
	E3AFreeImage(host_image);
	E3AFreeImage(secret_image);
	E3AFreeImage(decoded);
	
	return 0;
}