#include <stdio.h>
#include <stdlib.h>
#include "pngwrap.h"
#include "message.h"

int main(int argc, char** argv)
{
    // Loading Host Image
    bwimage_t *host_image;
	error_e retval_host = E3A_OK;

	host_image = E3ACreateImage();
	retval_host = E3ALoadImage(argv[1], host_image);
    
    // Decoding the data from host image
    printf("Decoding...\n");
    ReverseEngineerTheJob_Txt(host_image);
    printf("Done");


    E3AFreeImage(host_image);

    return 0;
}