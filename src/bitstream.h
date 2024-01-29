#ifndef __BITSTREAM_H__
#define __BITSTREAM_H__

typedef struct{
    unsigned int size; // Taille en octets
    unsigned int capacity;
    unsigned int position; // Position en bits
    unsigned char *data; // Les donnees
}bitstream;

bitstream* CreateEmptyBitstream();
bitstream* CreateBitstreamOnData(char *data, unsigned int size);

void ResetPosition(bitstream *b);
void WriteBits(bitstream *b, char o, unsigned int nbbits);
char ReadBits(bitstream *b, unsigned int nbbits);
void FreeBitstream();

// Preprocessing of the secret data
void PrepSecImg(bwimage_t* secret_image, bitstream* of_secret);

// Coding it
void lsb(unsigned char* pixel, bitstream* of_secret);
void HeDoesTheJob(bwimage_t* host_image, bitstream* of_secret);


// Decoding it
unsigned char binaryToDecimal(char bitSequence[8]);
void ReverseEngineerTheJob(bwimage_t* coded, bwimage_t* decoded_image);


// Sufficient Size 
int CanIHandleIt(bwimage_t* host_image, bwimage_t* secret_image);

#endif//__BITSTREAM_H__
