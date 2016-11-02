/**
 * copy.c
 *
 * Computer Science 50
 * Problem Set 4
 *
 * Copies a BMP piece by piece, just because.
 */
       
#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"

int main(int argc, char* argv[])
{
    // ensure proper usage
    if (argc != 4)
    {
        printf("Usage: ./resize n infile outfile\n");
        return 1;
    }
    
    if(!atoi(argv[1]))
    {
        printf("argument 1 must be integer");
        return 1;
    }
    
    int n = atoi(argv[1]);
    // remember filenames
    char* infile = argv[2];
    char* outfile = argv[3];

    // open input file 
    FILE* inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        printf("Could not open %s.\n", infile);
        return 2;
    }

    // open output file
    FILE* outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 3;
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
        return 4;
    }
    
    
    // copy header initial values
    int iHeight = bi.biHeight;
    int iWidth = bi.biWidth;
    
    // edit header values
    bi.biHeight = bi.biHeight * n;
    bi.biWidth = bi.biWidth * n;
    
    // determine padding for scanlines
    int padding =  (4 - (iWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    int paddingNew = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    
    
    bi.biSizeImage = (sizeof(RGBTRIPLE) * bi.biWidth + paddingNew) * abs(bi.biHeight);
    bf.bfSize = bi.biSizeImage + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    
    
    // write outfile's BITMAPFILEHEADER
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);

    // write outfile's BITMAPINFOHEADER
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);
    
    /*-----------------------------ZAMYLA'S ALGO----------------------------*/
    // psuedo-code
    /*
    1.    for each row
    2.        for n-1 times
    3.            write pixels, padding to the file
    4.            send infile cursor back
    5.        write pixels, padding to outfile
    6.        skip over infile padding
    */
    
    for (int i = 0, biHeight = abs(iHeight); i < biHeight; i++)                 
    {
        
        RGBTRIPLE triple;
        for (int k = 0; k < n - 1; k++)                                         
        {
            /*--------write pixels, padding to the file------------*/
            for (int j = 0; j < iWidth; j++)                                        
            {
                // read RGB triple from infile
                fread(&triple, sizeof(RGBTRIPLE), 1, inptr);
                // write RGB triple to outfile
                for (int l = 0; l < n; l++)                                     
                {
                    fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
                }
            }
            // write padding to the outfile
            for (int l = 0; l < paddingNew; l++)                                
            {
                fputc(0x00, outptr);
            }
            /*------------send infile cursor back-------------*/
            fseek(inptr, -1 * sizeof(RGBTRIPLE) * iWidth, SEEK_CUR);                 
        }
        /*-----------------------write pixels, padding to outfile--------------*/
        for (int j = 0; j < iWidth; j++)                                            
        {
            // read RGB triple from infile
            fread(&triple, sizeof(RGBTRIPLE), 1, inptr);
            /*--------write pixels, padding to the file------------*/
            // write RGB triple to outfile
            for (int l = 0; l < n; l++)
            {
                fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
            }
        }
        // write padding to the outfile
        for (int l = 0; l < paddingNew; l++)
        {
            fputc(0x00, outptr);
        }
        /*-----------------------skip over infile padding------------------*/
        fseek(inptr, padding, SEEK_CUR);  
        
    }
    
    
    /*-----------------------------MY ALGO----------------------------*/
    /*
    int count = 1;
    for (int i = 0, biHeight = abs(iHeight); i < biHeight;)
    {
        int j;
        // iterate over pixels in scanline
        for (j = 0; j < iWidth; j++)
        {
            // temporary storage
            RGBTRIPLE triple;

            // read RGB triple from infile
            fread(&triple, sizeof(RGBTRIPLE), 1, inptr);
            
            // write RGB triple to outfile
            
            for (int k = 0; k < n; k++)
            {
                fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
            }
        }
        // skip over padding, if any
        // fseek(inptr, padding, SEEK_CUR);
        
        // add back new padding
        for (int l = 0; l < paddingNew; l++)
        {
            fputc(0x00, outptr);
        }
        
        if(!(count % n == 0))   //n==3
        {
            fseek(inptr, -1 * sizeof(RGBTRIPLE) * j, SEEK_CUR);
        }
        else
        {
            fseek(inptr, padding, SEEK_CUR);
            count = 0;
            i++;
        }
        count++;
    }
    */

    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // that's all folks
    return 0;
}