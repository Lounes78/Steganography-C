#ifndef __MESSAGE_H__
#define __MESSAGE_H__

typedef struct{
    unsigned char *crc;
    unsigned char *size_of_filename;
    unsigned char *size_of_data;
    char *filename;
    char *data;
}message;


message* CreateFromFile(char *filename); //fait

unsigned int ComputeCRC(message* m); //fait

// message* Reallocate(message *m);

// void FreeMessage(message *m);

void HeDoesTheJob_Txt(bwimage_t* host_image, message* secret_message); //fait
void LsbTxt(unsigned char* pixel, message* secret_message, int index); //fait
void DecimalToBinary(unsigned int crc, unsigned char src_bin[]); //fait
unsigned char binaryToDecimal(char bitSequence[], int size); //fait
void ReverseEngineerTheJob_Txt(bwimage_t* coded); //fait



#endif//__MESSAGE_H__