#include "lib.h"

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <Video Folder Path> <Target FPS>\n", argv[0]);
        return 1;
    }

    signal(SIGINT, handleCtrlC);
    clearTerminal();
    curserVisible(0);

    BMPFileHeader fileHeader;
    BMPInfoHeader infoHeader;
    char videoFolderPath[50];

    char* VIDEO_NAME = argv[1];
    int TARGET_FPS = atoi(argv[2]);

    snprintf(videoFolderPath, sizeof(videoFolderPath), "./%s", VIDEO_NAME);
    int numFrames = countFramesInFolder(videoFolderPath);
    int*** frameData = malloc(numFrames * sizeof(int**));
    if (frameData == NULL) {
        perror("Error allocating memory");
        exit(EXIT_FAILURE);
    }

    loadFrames(numFrames, frameData, &fileHeader, &infoHeader, VIDEO_NAME);
    playFrames(numFrames, frameData, infoHeader, VIDEO_NAME, TARGET_FPS);
    freeFrames(numFrames, frameData);

    system("killall afplay");
    curserVisible(1);
    return 0;
}
