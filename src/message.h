#ifndef __MESSAGE_H__
#define __MESSAGE_H__

typedef struct{
    unsigned char *crc;
    unsigned char *size_of_filename;
    unsigned char *size_of_data;
    char *filename;
    char *data;
}message;


message* CreateFromFile(char *filename);

unsigned int ComputeCRC(message* m);

// message* Reallocate(message *m);

// void FreeMessage(message *m);

void HeDoesTheJob_Txt(bwimage_t* host_image, message* secret_message);
void LsbTxt(unsigned char* pixel, message* secret_message, int index);
void DecimalToBinary(unsigned int crc, unsigned char src_bin[]);
unsigned char binaryToDecimal(char bitSequence[], int size);
void ReverseEngineerTheJob_Txt(bwimage_t* coded);



#endif//__MESSAGE_H__