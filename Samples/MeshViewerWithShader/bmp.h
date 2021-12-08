#ifndef BMP_H
#define BMP_H

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cstdlib>

#define UNPACK(k, b) *((uint##b##_t*)k)

struct bmp_head
{
    char type[2];
    char length[4];
    char blank0[4];
    char offset[4];

    char infoSize[4];
    char width[4];
    char height[4];
    char unknown0[2];
    char bits[2];
    char compressType[4];
    char *data;
};

bmp_head readBmp(const char* path)
{
    FILE* f = fopen(path, "rb");
    bmp_head head;
    if (!f)
    {
        printf("BMP file read failed.\n");
        exit(-1);
    }
    fread(&head, sizeof(bmp_head), 1, f);
    printf("%s\n", path);
    printf("TYPE: %c%c\n", head.type[0], head.type[1]);
    printf("LENGTH: %d\n", UNPACK(head.length, 32));
    printf("OFFSET: %d\n", UNPACK(head.offset, 32));
    printf("INFO SIZE: %d\n", UNPACK(head.infoSize, 32));
    printf("SIZE: %dx%d\n", UNPACK(head.width, 32), UNPACK(head.height, 32));
    printf("BIT: %x\n", UNPACK(head.bits, 16));
    printf("COMPRESS TYPE: %x\n", UNPACK(head.compressType, 32));

    head.data = new char[UNPACK(head.length, 32)];
    fseek(f, 0, SEEK_SET);

    fread(head.data, sizeof(char), UNPACK(head.length, 32), f);
    return head;
}


#endif