#ifndef LIB_H
#define LIB_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <dirent.h>
#include <sys/ioctl.h>
#include <math.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define BLACK_BG "\x1b[48;2;0;0;0m"
#define RESET_COLOR "\x1b[0m"

extern int** currentFrame;
extern int** nextFrame;

#pragma pack(1)

typedef struct {
    uint16_t bfType;
    uint32_t bfSize;
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t bfOffBits;
} BMPFileHeader;

typedef struct {
    uint32_t biSize;
    int32_t biWidth;
    int32_t biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    int32_t biXPelsPerMeter;
    int32_t biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
} BMPInfoHeader;

FILE* openBMPFile(const char* filename);
void readBMPHeaders(FILE* file, BMPFileHeader* fileHeader, BMPInfoHeader* infoHeader);
int*** processPixels(FILE* file, int width, int height);
void clearTerminal();
void playMP3(char* filename);
int countFramesInFolder(const char* folderPath);
void renderFrame(struct winsize w, BMPInfoHeader infoHeader, int*** pixels);
void handleCtrlC(int signum);
void curserToTopLeftTop();
void curserVisible(int visible);
void loadFrames(int numFrames, int*** frameData, BMPFileHeader* fileHeader, BMPInfoHeader* infoHeader, char videoName[50]);
void playFrames(int numFrames, int*** frameData, BMPInfoHeader infoHeader, char VIDEO_NAME[50], int TARGET_FPS);
void freeFrames(int numFrames, int*** frameData);

#endif // LIB_H
