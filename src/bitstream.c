#include <stdlib.h>
#include <stdio.h>
#include "pngwrap.h"
#include "bitstream.h"

bitstream* CreateEmptyBitstream()
{
    bitstream* b = (bitstream*)malloc(sizeof(bitstream));
    if (b != NULL)
    {
        b->size = 0;
        b->position = 0;
    }
    
    return b;
}


bitstream* CreateBitstreamOnData(char *data, unsigned int size)
{
    bitstream* b = CreateEmptyBitstream();

    b->size = size;
    b->data = (char*)malloc(8*b->size*sizeof(char));

    return b;
}


void ResetPosition(bitstream *b) 
{
    b->position = 0;
}


void WriteBits(bitstream *b, char o, unsigned int nbbits)
{
    // Change the order of operations to set the LSB correctly
    b->data[nbbits + b->position] &= 0b11111110;
    b->data[nbbits + b->position] |= (o & 1);

    b->position++;
}



void FreeBitstream(bitstream* b)
{
    free(b->data);
}



// Data Preprocessing
void PrepSecImg(bwimage_t* secret_image, bitstream* of_secret)
{
    of_secret->size = 8*secret_image->width*secret_image->height;

    int rot = 0;
    for (int i = 0; i < secret_image->height; i++) 
	{
    	for (int j = 0; j < secret_image->width; j++) 
		{
            unsigned char grayscale_value = secret_image->data[i][j];

            for (int k = 7; k >= 0; k--) 
            {
                unsigned char bit = ((grayscale_value >> k) & 1); 
                int index = (i*secret_image->width + j + (7-k) + rot);
                of_secret->data[index] = bit;
            }
            rot = rot + 7;
    	}
	}
}



// Actually coding it
void lsb(unsigned char* pixel, bitstream* of_secret)
{
    if (of_secret->position < of_secret->size)
    {
        unsigned char bit = of_secret->data[of_secret->position];
        //printf("%d with %d ", *pixel, bit);

        *pixel &= 0b11111110; //he was not right
        *pixel |= bit;

        of_secret->position++;
        //printf("%d at %d \n", *pixel, of_secret->position -1);
    }        
}

void HeDoesTheJob(bwimage_t* host_image, bitstream* of_secret)
{ 
    for (int i = 0; i < host_image->height; i++) 
	{
    	for (int j = 0; j < host_image->width; j++) 
		{
            lsb(&host_image->data[i][j], of_secret);
        }        
    }
}







// Decoding part
unsigned char binaryToDecimal(char bitSequence[8]) 
{
    unsigned char decimalValue = 0;

    for (int i = 0; i < 8; i++) 
    {
        decimalValue = (decimalValue << 1) | bitSequence[i];
    }

    return decimalValue;
}


void ReverseEngineerTheJob(bwimage_t* coded, bwimage_t* decoded_image)
{
    int a = 0, ii = 0, jj = 0;
    unsigned char bitSequence[8] = {0};

    for (int i = 0; i < coded->height; i++) 
    {
        for (int j = 0; j < coded->width; j++) 
        {
            // Generation de bitSequence
            bitSequence[a] = coded->data[i][j] & 1;
            a++;

            if (a == 8) // Generation d'un pixel décodé
            {
                if (ii == decoded_image->height) // job effectue
                    goto exitLoops;

                decoded_image->data[ii][jj++] = binaryToDecimal(bitSequence); 
                a = 0;

                if (jj < decoded_image->width)
                {
                    decoded_image->data[ii][jj] = binaryToDecimal(bitSequence);
                }

                else
                {
                    jj = 0;
                    ii++;
                }
            }
        }       
    }

    exitLoops:

}


int CanIHandleIt(bwimage_t* host_image, bwimage_t* secret_image)
{
    int neededSpace = (int)8*secret_image->width*secret_image->height;
    int actualSpace = (int)host_image->width*host_image->height;
    
    float loseRate = neededSpace/(float)actualSpace;
    char user = 0;
    if (actualSpace < neededSpace)
    {
        printf("Image Secrete trop grande\n");
        printf("Taux de perte = %.2f%%\n", 100*loseRate);
        printf("Etes vous sur de vouloir continuer ?\n");

        printf("Y/N : ");
        scanf("%c", &user);

        if (user == 'Y')
        {
            printf("Tres Bien\n\n");
        }
        else
        {
            printf("Reessayer avec une autre image hote \n");
            exit(0);
        }
    }

}