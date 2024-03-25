#include <stdio.h>
#include <stdlib.h>
#include "pngwrap.h"
#include "message.h"

// ./main_file.c lena.png test1.txt
int main(int argc, char** argv)
{
    // Loading Host Image
    bwimage_t *host_image;
	error_e retval_host = E3A_OK;

	host_image = E3ACreateImage();
	retval_host = E3ALoadImage(argv[1], host_image);
    
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





    message* secret_message = CreateFromFile(argv[2]);
	
    // Encoding the data in host Image
    printf("Encoding the message in the Host Image...\n");
    HeDoesTheJob_Txt(host_image, secret_message);
    printf("Done\n");

    //Dump it
    E3ADumpImage("coded.png", host_image);

    E3AFreeImage(host_image);

    return 0;

}