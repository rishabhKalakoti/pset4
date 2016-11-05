/**
 * recover.c
 *
 * Computer Science 50
 * Problem Set 4
 *
 * Recovers JPEGs from a forensic image.
 */

#include <stdio.h>
#include <string.h>

int main(int argc, char* argv[])
{
    // open card.raw
    FILE* infile = fopen("card.raw", "r");
    
    // detect the first jpeg
    char jpeg0[4] = {0xff, 0xd8, 0xff, 0xe0};
    char jpeg1[4] = {0xff, 0xd8, 0xff, 0xe1};
    char firstchk[4];
    fread(firstchk, 1, 4, infile);
    while((memcmp(firstchk, jpeg0, 4) || 
        memcmp(firstchk, jpeg1, 4)) && firstchk[3]!=EOF)
    {
        fseek(infile, -3, SEEK_CUR);
        fread(firstchk, 1, 4, infile);
    }
    fseek(infile, -1, SEEK_CUR);
    
    
    // open new jpeg
    char buffer[512];
    
    fread(buffer, 1, 512, infile);
    
    int filecount = 0;
    int flag = 0;
    FILE* outfile;
    do
    {
        if((buffer[0] == jpeg0[0] && buffer[1] == jpeg0[1] && buffer[2] == jpeg0[2] && buffer[3] == jpeg0[3]) || 
        (buffer[0] == jpeg1[0] && buffer[1] == jpeg1[1] && buffer[2] == jpeg1[2] && buffer[3] == jpeg1[3]))
        {
            if (flag == 0)
                flag = 1;
            else
                fclose(outfile);
            
            char fname[8];
            sprintf(fname, "%03d.jpg", filecount);
            outfile = fopen(fname, "w");
            filecount++;
        }
        
        fwrite(buffer, 1, 512, outfile);
    // write 512 bytes until a new jpeg is detected
    }while(fread(buffer, 1, 512, infile) == 512);
    // find end of file
    fclose(outfile);
    fclose(infile);
    
    return 0;
}
