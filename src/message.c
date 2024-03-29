#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "pngwrap.h"
#include "message.h"



int binaryToDecimal_bis(char *binary, int size) {
    int decimal = 0;
    int power = 0;

    for (int i = size - 1; i >= 0; i--) {
        if (binary[i] == 1) {
            decimal += pow(2, power);
        }
        power++;
    }

    return decimal;
}


unsigned char binaryToDecimal(char bitSequence[], int size) 
{
    unsigned char decimalValue = 0;
    for (int i = 0; i < size; i++) 
    {
        decimalValue = (decimalValue << 1) | bitSequence[i];
    }

    return decimalValue;
}



unsigned int ComputeCRC(message* m)
{
    unsigned int crc=0 ;
    for(int i = 0 ;i < 24; i++)
    {
        //printf("%d ", m->data[i]);
        crc ^= m->data[i];
    }

    //printf("\nCRC = %d\n", crc);  
    return crc;
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




message* CreateFromFile(char *filename)
{
    message* createdmessage = malloc(sizeof(message));

    FILE* file = fopen(filename, "rb");

    if (file == NULL)
        printf("ERREUR lors de l'ouverture du fichier\n");



    //  --- file name --- Encoding
    createdmessage->filename = (char*)malloc(8*strlen(filename));

    int index = 0;
   
    for(int i = 0; i < strlen(filename); i++)
    {
        for(int j=7; j>=0; j--)
        {
            createdmessage->filename[index++] = (filename[i]>>j) & 1; 
        }
    }


    // --- size of filename --- Encoding
    createdmessage->size_of_filename = (unsigned char*) malloc(8*sizeof(unsigned int));
    unsigned int size_of_filename = strlen(filename);
    DecimalToBinary(size_of_filename, createdmessage->size_of_filename);


    char byte;
    fseek(file, 0, SEEK_END);
    // --- size of data --- encoding 
    createdmessage->size_of_data = (unsigned char*) malloc(8*sizeof(unsigned int));
    unsigned int size_of_data = ftell(file);
    DecimalToBinary(size_of_data, createdmessage->size_of_data);
    


    // --- data --- Encoding
    createdmessage->data = (char*)malloc(8*ftell(file)*sizeof(char));
    int size = 0;

    rewind(file);
    while ((byte = fgetc(file)) != EOF) {
    
     for (int i = 7; i >= 0; i--) 
        {
            createdmessage->data[size++] = (byte >> i) & 1;
        }
    }
    rewind(file);

 
    // Computing the Check Sum
    createdmessage->crc = (unsigned char*) malloc(8*sizeof(unsigned int));
    
    unsigned int crc = ComputeCRC(createdmessage);
    DecimalToBinary(crc, createdmessage->crc);

    fclose(file);
    return createdmessage; 
}




// Actually Coding it 
// CRC  FN_SIZE  D_SIZE  FileName  Data
void LsbTxt(unsigned char* pixel, message* secret_message, int index)
{   
    unsigned char bit = 0;
    // CRC Encoding
    if (index < 8*sizeof(unsigned int))
    {
        bit = secret_message->crc[index];
    }

    // File name SIZE Encoding
    else if (index < 8*(sizeof(unsigned int) + sizeof(unsigned int)))
    {
        bit = secret_message->size_of_filename[index - 8*sizeof(unsigned int)];
    }

    // Data SIZE Encoding
    else if (index < 8*(sizeof(unsigned int) + sizeof(unsigned int) + sizeof(unsigned int)))
    {
        bit = secret_message->size_of_data[index - 8*2*sizeof(unsigned int)];
    }

    // file name Encoding
    else if (index < 8*(binaryToDecimal_bis(secret_message->size_of_filename, 8*sizeof(unsigned int)) + 3*sizeof(unsigned int)))
    {
        bit = secret_message->filename[index - 8*3*sizeof(unsigned int)];
    }

    // data encoding
    else if (index < 8*(binaryToDecimal_bis(secret_message->size_of_filename, 8*sizeof(unsigned int)) + binaryToDecimal_bis(secret_message->size_of_data, 8*sizeof(unsigned int)) + 3*sizeof(unsigned int))) 
    {
        bit = secret_message->data[index - 8*(binaryToDecimal(secret_message->size_of_filename, 8*sizeof(unsigned int)) + 3*sizeof(unsigned int))];
    }




  if (index < 8*(binaryToDecimal_bis(secret_message->size_of_filename, 8*sizeof(unsigned int)) + binaryToDecimal_bis(secret_message->size_of_data, 8*sizeof(unsigned int)) + 3*sizeof(unsigned int))) 
    {
        *pixel &= 0b11111110; 
        *pixel |= bit;
    }


  
}



// Coding in the Image
void HeDoesTheJob_Txt(bwimage_t* host_image, message* secret_message)
{
    int index = 0;
    for (int i = 0; i < host_image->height; i++) 
	{
    	for (int j = 0; j < host_image->width; j++) 
	    {
            LsbTxt(&host_image->data[i][j], secret_message, index);
            index++;
        }
    }
	E3ADumpImage("../img/coded.png", host_image);
}




// Decoding from the Image
void ReverseEngineerTheJob_Txt(bwimage_t* coded)
{
    FILE* decfile = fopen("decoded.txt", "wb");
    int a = 0, separator = 0;
    unsigned char bitSequence[32] = {0};

    int fnsize = 0, dsize = 0;

    for (int i = 0; i < coded->height; i++) 
    {
        for (int j = 0; j < coded->width; j++) 
        {
            // Generation de bitSequence
            bitSequence[a] = coded->data[i][j] & 1;
            a++;

            // Metadata decoding    
            if (a == 32 && separator < 3) // Generation d'un pixel décodé
            {
                a = 0;
                if (separator == 0) // Write CRC
                {
                    fprintf(decfile, "CRC : %d\n", binaryToDecimal_bis(bitSequence, 32));
                }
                else if (separator == 1) // Write File name size
                {
                    fnsize = binaryToDecimal_bis(bitSequence, 32);
                    fprintf(decfile, "Taille du nom de Fichier (en caracteres) : %d\n", fnsize); // Each \n is 2 caracters    
                }
                else if (separator == 2) // Write Data Size
                {
                    dsize = binaryToDecimal_bis(bitSequence, 32);
                    fprintf(decfile, "Taille de données (en caracteres) : %d\n", dsize);
                }
                separator++;
            }

            if (a == 8 && separator == 3)
            {
                a = 0;
                fprintf(decfile, "Nom du fichier: %c", binaryToDecimal(bitSequence, 8));
                separator++;
            }


            // Decoding the Data and the file name
            if (a == 8 && separator > 3 && separator < (3 + fnsize))
            {
                a = 0;
                // Write the file name
                fprintf(decfile, "%c", binaryToDecimal(bitSequence, 8));
                separator++;
            }

            else if (a == 8 && separator == (3 + fnsize))
            {
                a = 0;
                fprintf(decfile, "\n%c", binaryToDecimal(bitSequence, 8));
                separator ++;
            }
            
            else if (a == 8 && separator > (3 + fnsize) && separator < (3 + fnsize + dsize))
            {
                a = 0;
                separator ++;
                fprintf(decfile, "%c", binaryToDecimal(bitSequence, 8));
            }

            else if (separator == (3 + fnsize + dsize)) // End of the Work
                goto exitLoops;
        }
    }

    exitLoops:


    fclose(decfile);
}