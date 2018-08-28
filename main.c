#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

#define BUFSIZE (4096 * 512)

const char PICTURE_TYPE_TEXT[22][64] ={
        "Other",
        "32x32 pixels 'file icon' (PNG only)",
        "Other file icon",
        "Cover (front)",
        "Cover (back)",
        "Leaflet page",
        "Media (e.g. label side of CD)",
        "Lead artist/lead performer/soloist",
        "Artist/performer",
        "Conductor",
        "Band/Orchestra",
        "Composer",
        "Lyricist/text writer",
        "Recording Location",
        "During recording",
        "During performance",
        "Movie/video screen capture",
        "A bright coloured fish",
        "Illustration",
        "Band/artist logotype",
        "Publisher/Studio logotype"
        "Not defined"
};

int main(int argc, char **argv)
{
    char *program_name = argv[0];
    if(argc != 2){
        printf("usase: %s [flac file]\n", program_name);
        exit(EXIT_FAILURE);
    }

    FILE *fp = fopen(argv[1], "rb");
    if(fp == NULL){
        printf("error: can't open file %s", argv[1]);
        exit(EXIT_FAILURE);
    }

    ssize_t ret_in;
    unsigned char buffer[BUFSIZE];
    ret_in = fread(buffer, sizeof(unsigned char), BUFSIZE, fp);

    if(ret_in < 0){
        printf("error: can't read\n");
        exit(EXIT_FAILURE);
    }

    // <32>: "fLaC"
    char flac_header[5];
    for(int j=0; j<4; j++)
        flac_header[j] = buffer[j];
    flac_header[4] = '\0';
    // printf("%s\n", flac_header);
    
    if ( strcmp(flac_header, "fLaC") != 0){
        printf("error: no flac file.\n");
        exit(EXIT_FAILURE);
    }

    // METADATA BLOCK
    int read_pos = 4;
    int block_type, pic_type;
    int length;
    while(true){
        length = 0;
        length += (int)(buffer[read_pos+1]<<16);
        length += (int)(buffer[read_pos+2]<<8);
        length += (int)(buffer[read_pos+3]<<0);
        
        block_type = (int)(buffer[read_pos]&~(1<<7));
        
        switch(block_type){
        case 0: // STREAMINFO
            printf ("STREAMINFO: %dbyte\n", length);
            break;
        case 1: // PADDING
            printf ("PADDING: %dbyte\n", length);
            break;
        case 2: // APPLICATION
            printf ("APPLICATION: %dbyte\n", length);
            break;
        case 3: // SEEKTABLE
            printf ("SEEKTABLE: %dbyte\n", length);
            break;
        case 4: // VORBIS_COMMENT
            printf ("VORBIS_COMMENT: %dbyte\n", length);
            break;
        case 5: // CUESHEET
            printf ("CUESHEET: %dbyte\n", length);
            break;
        case 6: // PICTURE
            pic_type = 0;
            pic_type += (int)(buffer[read_pos+4]<<24);
            pic_type += (int)(buffer[read_pos+5]<<16);
            pic_type += (int)(buffer[read_pos+6]<<8);
            pic_type += (int)(buffer[read_pos+7]<<0);
            if (pic_type < 0 || 20 < pic_type)
                pic_type = 21;
            printf ("PICTURE %s: %dbyte\n", PICTURE_TYPE_TEXT[pic_type], length);
            break;
        default :
            printf("NO DEFINED: %dbyte\n", length);
            break;
        }
        
        // printf("POS: %6d\tBLOCK_TYPE: 0x%02x\tBLOCK_LENGTH %6d\n", read_pos, (buffer[read_pos]&~(1<<7)), length);
        
        if (length == 0){
            printf("error\n");
            break;
        }
        if (read_pos+length > BUFSIZE ){
            printf("buffer over\n");
            break;
        }
        if(buffer[read_pos] & (1<<7))
            break;


        read_pos += (length + 4);
        
    }



    fclose(fp);

    exit(EXIT_SUCCESS);
}
