#include <stdio.h>
#include <stdlib.h>
#include "pngwrap.h"
#include "bitstream.h"

int main(int argc, char **argv)
{
	unsigned int seed = atoi(argv[2]);

	rgba_image_t *host_image;

	error_e retval_host = E3A_OK;


	host_image = E3ACreateRGBAImage();


	retval_host = E3ALoadRGBAImage(argv[1], host_image);

	// Decoding
	printf("Decoding ...\n");
	ReverseEngineerTheJob_RGBA_Rand(host_image, seed);

	printf("Decoded.\n");
	

	E3AFreeRGBAImage(host_image);

	return 0;
}