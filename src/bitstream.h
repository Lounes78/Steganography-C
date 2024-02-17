#ifndef __BITSTREAM_H__
#define __BITSTREAM_H__

typedef struct{
    unsigned int size; // Taille en octets
    unsigned int capacity;
    unsigned int position; // Position en bits
    unsigned char *data; // Les donnees
}bitstream;

bitstream* CreateEmptyBitstream();//fait
bitstream* CreateBitstreamOnData(char *data, unsigned int size);//fait

void ResetPosition(bitstream *b);//fait
void WriteBits(bitstream *b, char o, unsigned int nbbits);//fait
char ReadBits(bitstream *b, unsigned int nbbits); //remplacé par prepsecimage
void FreeBitstream(); //fait

// Preprocessing of the secret data
void PrepSecImg(bwimage_t* secret_image, bitstream* of_secret, unsigned int seed); //fait

// Coding it
void Lsb(unsigned char* pixel, bitstream* of_secret); //fait
void HeDoesTheJob(bwimage_t* host_image, bitstream* of_secret); //fait

// Randomisation
void HeDoesTheJob_Rand(bwimage_t* host_image, bitstream* of_secret, int seed); //fait



// Decoding it 
unsigned char binaryToDecimal(char bitSequence[], int size); //fait
void DecimalToBinary(unsigned int crc, unsigned char src_bin[]); //fait

void ReverseEngineerTheJob(bwimage_t* coded, bwimage_t* decoded_image); //fait
void ReverseEngineerTheJob_Rand(bwimage_t* coded, bwimage_t* decoded_image, unsigned int seed);


// Sufficient Size 
int CanIHandleIt(bwimage_t* host_image, bwimage_t* secret_image); //fait


#endif//__BITSTREAM_H__
