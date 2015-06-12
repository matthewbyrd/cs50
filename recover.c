/**
 * recover.c
 *
 * Computer Science 50
 * Problem Set 4
 *
 * Recovers JPEGs from a forensic image.
 *
 * Usage: ./recover
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

typedef uint8_t BYTE;

/** returns whether n contiguous items of two lists of ints are identical */
bool compare(BYTE first[], BYTE second[], size_t n);


int main(int argc, char* argv[])
{
    // blocksize for each read pass
    BYTE* block = malloc(sizeof(BYTE) * 512);
        
    // patterns we're looking for
    BYTE pattern1[] = {0xff, 0xd8, 0xff, 0xe0};
    BYTE pattern2[] = {0xff, 0xd8, 0xff, 0xe1};

    // open the recovery source
    FILE* file = fopen("card.raw", "r");
    if (file == NULL)
    {
        printf("Error when reading card.raw\n");
        return 1;
    }
    
    // number of files recovered
    int recovered = 0;
    // recovered filename
    char recoveredName[8];
    FILE* newfile;
    
    // read the blocks
    while (fread(block, 512, 1, file) == 1)
    {   
        bool reading = false;
        
        // look for jpg file match
        if (compare(block, pattern1, 4) || compare(block, pattern2, 4))
        {
            if (reading == true) 
            {
                // close the previous file
                fclose(newfile);
            }
            // get a file name
            sprintf(recoveredName, "%03d.jpg", recovered++);
            //store entire block in a file.
            newfile = fopen(recoveredName, "w");
            reading = true;
            
            // copy block into newfile
            fwrite(block, 512, 1, newfile);
        }
        else if (reading)
        {
            // copy block into newfile
            fwrite(block, 512, 1, newfile);
        }
    } 
    
    free(block);
    fclose(newfile);
    fclose(file);
    return 0;
}


/** returns whether n contiguous items of two lists of ints are identical */
bool compare(BYTE first[], BYTE second[], size_t n)
{
    int i;
    for (i = 0; i < n; i++)
    {
        if (first[i] != second[i])
        {
            return false;
        }
    }
    return true;
}

