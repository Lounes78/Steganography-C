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
	retval_host = E3ALoadImage("../img/lena.png", host_image);
    
    // 
    message* secret_message = CreateFromFile("../txt/test1.txt");   
	
    // Encoding the data in host Image
    printf("Encoding the message in the Host Image...\n");
    HeDoesTheJob_Txt(host_image, secret_message);
    printf("Done\n");


    // Decoding the data from host image
    printf("Decoding...\n");
    ReverseEngineerTheJob_Txt(host_image);
    printf("Done");

    return 0;
}