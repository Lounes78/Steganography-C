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
void PrepSecImg(bwimage_t* secret_image, bitstream* of_secret, unsigned int seed)
{
    of_secret->size = 8*secret_image->width*secret_image->height + 3*32; //  3*32 for meta data 
    // Metadata  SEED -- height -- width
    unsigned char seed_bin[32]= {0};
    unsigned char height_bin[32]={0};
    unsigned char width_bin[32]={0};
    
    DecimalToBinary(of_secret->size, seed_bin); //En vrai on l utilise pas pour le moment 
    DecimalToBinary(secret_image->height, height_bin);
    DecimalToBinary(secret_image->width, width_bin);

    int index = 0;
    
    for(int i = 0; i < 32; i++)
    { 
        of_secret->data[i] = seed_bin[i];    
    }

    for(int i = 32; i < 64; i++)
    {
        of_secret->data[i] = height_bin[i-32];
    }
    
    for(int i = 2*32; i<3*32; i++)
    { 
        of_secret->data[i] = width_bin[i-64];
    }
    


    //DATA
    //int rot = 0;
    for (int i = 0; i < secret_image->height; i++) 
	{
    	for (int j = 0; j < secret_image->width; j++) 
		{
            unsigned char grayscale_value = secret_image->data[i][j];

            for (int k = 7; k >= 0; k--) 
            {
                unsigned char bit = ((grayscale_value >> k) & 1); 
                //overengineered int index = (i*secret_image->width + j + (7-k) + rot);
                of_secret->data[3*32 + index++] = bit;
            }
            //rot = rot + 7;
    	}
	}
    
}



// Actually coding it
void Lsb(unsigned char* pixel, bitstream* of_secret)
{
    if (of_secret->position < of_secret->size) //En gros une fois on a encodé tous les pixels de l image secrete on ne fait plus rien aux autres pixels de l image hote 
    {
        unsigned char bit = of_secret->data[of_secret->position];
        //printf("%d with %d ", *pixel, bit);

        *pixel &= 0b11111110; //he was not right
        *pixel |= bit;
        //printf("%d at %d \n", *pixel, of_secret->position);
        of_secret->position++;
    }
}




void HeDoesTheJob(bwimage_t* host_image, bitstream* of_secret)
{ 
    for (int i = 0; i < host_image->height; i++) 
	{
    	for (int j = 0; j < host_image->width; j++) 
		{
            Lsb(&host_image->data[i][j], of_secret);
        }        
    }
}






// Decoding part
unsigned char binaryToDecimal(char bitSequence[], int size) 
{
    unsigned char decimalValue = 0;

    for (int i = 0; i < size; i++) 
    {
        decimalValue = (decimalValue << 1) | bitSequence[i];
    }

    return decimalValue;
}

void DecimalToBinary(unsigned int crc, unsigned char src_bin[])
{
    // Codage sur 32 bits
    int i = 8*sizeof(unsigned int) - 1;
    while (i >= 0) {
        src_bin[i] = crc % 2;
        crc = crc / 2;
        i--;
    }
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

                decoded_image->data[ii][jj++] = binaryToDecimal(bitSequence, 8); 
                a = 0;

                if (jj < decoded_image->width)
                {
                    decoded_image->data[ii][jj] = binaryToDecimal(bitSequence, 8);
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
    int neededSpace = (int)8*secret_image->width*secret_image->height + 3*32; //3*32 is for metaData
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


// Algorithe deterministe
void HeDoesTheJob_Rand(bwimage_t* host_image, bitstream* of_secret, int seed)
{
//    Used pixel matrice creation
    char** usedpixel = (char**)malloc((host_image->height)*sizeof(char*));    
    if (usedpixel == NULL) {
        printf("Memory allocation failed\n");
        exit(1);
    }
    
    for (int i = 0; i < host_image->height; i++) {
        usedpixel[i] = (char *)malloc(host_image->width * sizeof(char));
        if (usedpixel[i] == NULL) {
            printf("Memory allocation failed\n");
            exit(1);
        }
    }


    
//    char usedpixel[host_image->height][host_image->width];



    // Initialization
    for (int i = 0; i < host_image->height; i++) {
        for (int j = 0; j < host_image->width; j++) {
            usedpixel[i][j] = 0;
        }
    }
        

    
    int i = 0, j = 0;
    
    srand((unsigned int) seed);
    while(of_secret->position < of_secret->size)
    {
        //printf("%d %d\n", i, j);
        i = rand()%host_image->height;
        j = rand()%host_image->width;
            
        while(usedpixel[i][j] == 1)
        {
            i = rand()%host_image->height;
            j = rand()%host_image->width;
        }
        usedpixel[i][j] = 1;
        
        Lsb(&host_image->data[i][j], of_secret);
    
    }

    // Deallocate usedpixelixel
    for (int i = 0; i < host_image->height; i++) {
        free(usedpixel[i]);
    }
    free(usedpixel);

}





void ReverseEngineerTheJob_Rand(bwimage_t* coded, bwimage_t* decoded_image, unsigned int seed)
{
    // Decoding the meta data
    // On a 3*32 bits au debut: size - heigth - width
    char** usedpixel = (char**)malloc((coded->height)*sizeof(char*));    
    if (usedpixel == NULL) {
        printf("Memory allocation failed\n");
        exit(1);
    }
    
    for (int i = 0; i < coded->height; i++) {
        usedpixel[i] = (char *)malloc(coded->width * sizeof(char));
        if (usedpixel[i] == NULL) {
            printf("Memory allocation failed\n");
            exit(1);
        }
    }

    //char usedpixel[coded->height][coded->width];


    // Initialization
    for (int i = 0; i < coded->height; i++) {
        for (int j = 0; j < coded->width; j++) {
            usedpixel[i][j] = 0;
        }
    }
        

    //
    int i = 0, j = 0;
    srand((unsigned int) seed);

    int a = 0, ii = 0, jj = 0;
    unsigned position = 0;
    int height = 0, width = 0;
    unsigned char bitSequenceMetaData[32] = {0};
    unsigned char bitSequence[8] = {0};


    while(position < 3*32) // recuperer les metadatas
    {
        position++;
        i = rand()%coded->height;
        j = rand()%coded->width;
            
        while(usedpixel[i][j] == 1)
        {
            i = rand()%coded->height;
            j = rand()%coded->width;
        }
        usedpixel[i][j] = 1;

        bitSequenceMetaData[a%32] = coded->data[i][j] & 1; //On recupere juste le lsb
        a++;
        
        if (a == 32) // Generation d'un pixel décodé
        {
            //printf("on trouvera bien un truc a faire avec ca\n");
        }
        
        else if (a == 2*32)
        {
            decoded_image->height = binaryToDecimal(bitSequenceMetaData, 32);
        }

        else if(a == 3*32)
        {
            decoded_image->width = binaryToDecimal(bitSequenceMetaData, 32);
            a = 0;
        }
        
    }

    

    while(position < 8*decoded_image->height*decoded_image->width) // recuperer les metadatas
    {
        position++;
        i = rand()%coded->height;
        j = rand()%coded->width;
            
        while(usedpixel[i][j] == 1)
        {
            i = rand()%coded->height;
            j = rand()%coded->width;
        }
        usedpixel[i][j] = 1;

        bitSequence[a%8] = coded->data[i][j] & 1; //On recupere juste le lsb
        a++;
        
        if (a == 8) // Generation d'un pixel décodé
        {
            if (ii == decoded_image->height) // job effectue
                break;

            decoded_image->data[ii][jj++] = binaryToDecimal(bitSequence, 8); 
            a = 0;

            if (jj < decoded_image->width)
            {
                decoded_image->data[ii][jj] = binaryToDecimal(bitSequence, 8);
            }

            else
            {
                jj = 0;
                ii++;
            }

        }
        
    }

    

    // // Deallocate usedpixelixel
    for (int i = 0; i < coded->height; i++) {
        free(usedpixel[i]);
    }
    free(usedpixel);


}











/*      OPTIMISATION        */
// Deterministic Scattering

// void HeDoesTheJobWithScattering(bwimage_t* host_image, bitstream* of_secret)
// {
//     int ii = 0; jj = 0;
//     for (int i = 0; i < host_image->height; i++) 
// 	{
//     	for (int j = 0; j < host_image->width; j++) 
// 		{           
//             if (of_secret->position < of_secret->size) //En gros une fois on a encodé tous les pixels de l image secrete on ne fait plus rien aux autres pixels de l image hote 
//             {
//                 ii = DeterministicDispersion(i, host_image);
//                 jj = DeterministicDispersion(j, host_image);
//                 Lsb(&host_image->data[ii][jj], of_secret);
//                 of_secret->position++;
//             }            
//         }
//    }
// }


// int DeterministicDispersion(int a, bwimage_t* host_image)
// {

// }