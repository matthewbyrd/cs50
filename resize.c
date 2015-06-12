/**
 * resize.c
 *
 * Computer Science 50
 * Problem Set 4
 *
 * Resizes a BMP by a factor provided on the cli.
 * 
 * Usage: ./resize resize_factor infile outfile
 */
       
#include <stdio.h>
#include <stdlib.h>

// custom library written by Harvard CS50
#include "bmp.h"

int main(int argc, char* argv[])
{
    // ensure proper usage
    if (argc != 4)
    {
        printf("Usage: ./resize resize_factor infile outfile\n");
        return 1;
    }

    // get the resize factor
    int resize = atoi(argv[1]);
    
    if (resize < 1 || resize > 100)
    {
        printf("Resize factor is too large. Enter an integer in range [1,100]\n");
        return 2;
    }

    // remember filenames
    char* infile = argv[2];
    char* outfile = argv[3];

    // open input file 
    FILE* inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        printf("Could not open %s.\n", infile);
        return 3;
    }

    // open output file
    FILE* outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 4;
    }

    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 || 
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 5;
    }
    
    // the headers for the new file
    BITMAPFILEHEADER new_bf = bf;
    BITMAPINFOHEADER new_bi = bi;
    
    // CHANGE HEADER INFORMATION
    new_bi.biWidth *= resize;
    new_bi.biHeight *= resize;
    
    // determine padding of old and new files
    int padding =  (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    int new_padding = (4 - (new_bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
 
    // determine new sizeimage
    new_bi.biSizeImage = (new_bi.biWidth * sizeof(RGBTRIPLE) + new_padding) * abs(new_bi.biHeight);
    
    // calculate new file size
    new_bf.bfSize = new_bf.bfSize - bi.biSizeImage + new_bi.biSizeImage;

    // write outfile's BITMAPFILEHEADER
    fwrite(&new_bf, sizeof(BITMAPFILEHEADER), 1, outptr);

    // write outfile's BITMAPINFOHEADER
    fwrite(&new_bi, sizeof(BITMAPINFOHEADER), 1, outptr);

    // iterate over infile's scanlines
    for (int i = 0, biHeight = abs(bi.biHeight); i < biHeight; i++)
    {
        // store the line in an array to be written to the new file resize times
        RGBTRIPLE* line = malloc(sizeof(RGBTRIPLE) * new_bi.biWidth);
        
        // make sure malloc worked
        if (line == NULL)
        {
            printf("Could not allocate memory (c: 97 returned NULL)\n");
            return 6;
        }
        
        // iterate over pixels in scanline, storing them resize times in our line
        for (int j = 0, l = 0; j < bi.biWidth; j++)
        {
            // temporary storage
            RGBTRIPLE triple;

            // read RGB triple from infile
            fread(&triple, sizeof(RGBTRIPLE), 1, inptr);
            
            // copy that RGB triple into our line resize times
            for (int n = 0; n < resize; n++)
            {
                line[l++] = triple;
            }
        }
        
        // write the line resize times, with padding
        for (int m = 0; m < resize; m++)
        {
            // write the line of pixels
            fwrite(&line[0], (sizeof(RGBTRIPLE) * new_bi.biWidth), 1, outptr);
            // add padding, if any   
            for (int k = 0; k < new_padding; k++)
            {
                fputc(0x00, outptr);
            }
        }
        
        // skip over padding in old file, if any
        fseek(inptr, padding, SEEK_CUR);
        
        // free the space used for our line
        free(line);
    }

    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);
    
    return 0;
}
