#include "lib.h"

FILE* openBMPFile(const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        perror("Cannot open file");
        exit(1);
    }
    return file;
}


void readBMPHeaders(FILE* file, BMPFileHeader* fileHeader, BMPInfoHeader* infoHeader) {
    size_t fileHeaderRead = fread(fileHeader, sizeof(BMPFileHeader), 1, file);
    size_t infoHeaderRead = fread(infoHeader, sizeof(BMPInfoHeader), 1, file);

    if (fileHeaderRead != 1 || infoHeaderRead != 1 || fileHeader->bfType != 0x4D42) {
        fprintf(stderr, "Error: BMP 파일이 아니거나 올바르지 않은 파일 형식임\n");
        fclose(file);
        exit(1);
    }
}


int*** processPixels(FILE* file, int width, int height) {
    int rowSize = (width * 3 + 3) & ~3;
    uint8_t* data = malloc(rowSize);
    int*** pixels = malloc(height * sizeof(int**));

    if (pixels == NULL || data == NULL) {
        perror("Memory allocation error");
        fclose(file);
        exit(1);
    }

    for (int i = 0; i < height; i++) {
        pixels[i] = malloc(width * sizeof(int*));
        if (pixels[i] == NULL) {
            perror("Memory allocation error");
            fclose(file);
            exit(1);
        }
        for (int j = 0; j < width; j++) {
            pixels[i][j] = malloc(3 * sizeof(int));
            if (pixels[i][j] == NULL) {
                perror("Memory allocation error");
                fclose(file);
                exit(1);
            }
        }
    }

    for (int i = 0; i < height; i++) {
        fread(data, rowSize, 1, file);
        for (int j = 0; j < width; j++) {
            uint8_t red = data[j * 3 + 2];
            uint8_t blue = data[j * 3];
            uint8_t green = data[j * 3 + 1];
            pixels[i][j][0] = (int)red;
            pixels[i][j][1] = (int)green;
            pixels[i][j][2] = (int)blue;
        }
    }

    free(data);
    return pixels;
}

void clearTerminal() {
    printf("\x1b[2J\x1b[H");
}

#ifdef _WIN32
// Windows에 대한 코드
void playMP3(char* filename) {
    char mp3Command[100];
    snprintf(mp3Command, sizeof(mp3Command), "start /min wmplayer \"%s.mp3\"", filename);
    if (system(mp3Command) != 0) {
        perror("Failed to play MP3");
        exit(1);
    }
}

#elif defined(__linux__)
// Linux에 대한 코드
void playMP3(char* filename) {
    char mp3Command[100];
    snprintf(mp3Command, sizeof(mp3Command), "mpg123 \"%s.mp3\" &", filename);
    if (system(mp3Command) != 0) {
        perror("Failed to play MP3");
        exit(1);
    }
}

#elif defined(__APPLE__)
// macOS에 대한 코드
void playMP3(char* filename) {
    char mp3Command[100];
    snprintf(mp3Command, sizeof(mp3Command), "afplay \"%s.mp3\" &", filename);
    if (system(mp3Command) != 0) {
        perror("Failed to play MP3");
        exit(1);
    }
}

#else
// 다른 운영 체제에 대한 처리 (예: 에러 출력)
void playMP3(char* filename) {
    printf("Unsupported OS\n");
}

#endif

int countFramesInFolder(const char* folderPath) {
    DIR* dir = opendir(folderPath);
    if (dir == NULL) {
        perror("opendir");
        exit(1);
    }

    int numFrames = 0;
    struct dirent* entry;

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) {
            numFrames++;
        }
    }

    closedir(dir);
    return numFrames;
}

void renderFrame(struct winsize w, BMPInfoHeader infoHeader, int*** pixels) {
    float aspect_ratio_x = (float)w.ws_col / infoHeader.biWidth;
    float aspect_ratio_y = (float)w.ws_row / infoHeader.biHeight;
    float aspect_ratio = fminf(aspect_ratio_x, aspect_ratio_y);

    int scaled_width = infoHeader.biWidth * aspect_ratio;
    int scaled_height = infoHeader.biHeight * aspect_ratio;

    int left_padding = (w.ws_col - scaled_width) / 2;

    for (int y = scaled_height - 1; y >= 0; y--) {
        for (int x = 0; x < left_padding; x++) {
            printf(BLACK_BG " ");
        }

        for (int x = 0; x < scaled_width; x++) {
            int original_x = (int)(x / aspect_ratio);
            int original_y = (int)(y / aspect_ratio);
            int red = pixels[original_y][original_x][0];
            int green = pixels[original_y][original_x][1];
            int blue = pixels[original_y][original_x][2];

            printf("\x1b[48;2;%d;%d;%dm ", red, green, blue);
        }

        for (int x = left_padding + scaled_width; x < w.ws_col; x++) {
            printf(BLACK_BG " ");
        }
        printf(RESET_COLOR "\n");
    }

    for (int y = scaled_height; y < w.ws_row; y++) {
        for (int x = 0; x < w.ws_col; x++) {
            printf(BLACK_BG " ");
        }
        printf(RESET_COLOR "\n");
    }
}

#ifdef _WIN32
void handleCtrlC(int signum) {
    if (signum == SIGINT) {
        system("taskkill /IM wmplayer.exe /F");
        exit(0);
    }
}

#elif defined(__APPLE__)
void handleCtrlC(int signum) {
    if (signum == SIGINT) {
        system("killall afplay");
        exit(0);
    }
}

#elif defined(__linux__)
void handleCtrlC(int signum) {
    if (signum == SIGINT) {
        system("killall mpg123");
        exit(0);
    }
}

#else
void handleCtrlC(int signum) {
    if (signum == SIGINT) {
        exit(0);
    }
}

#endif

void curserToTopLeftTop() {
    printf("\033[0;0H");
}

void curserVisible(int visible) {
    if (visible) printf("\e[?25h");
    else printf("\e[?25l");
}

void loadFrames(int numFrames, int*** frameData, BMPFileHeader* fileHeader, BMPInfoHeader* infoHeader, char VIDEO_NAME[50]) {
    char filename[50];
    for (int i = 1; i <= numFrames; i++) {
        printf("Loading frame %d...\n", i);
        curserToTopLeftTop();
        sprintf(filename, "%s/frame_%d.bmp", VIDEO_NAME, i);
        FILE* file = openBMPFile(filename);
        if (file == NULL) {
            perror("Error opening file");
            exit(EXIT_FAILURE);
        }
        readBMPHeaders(file, fileHeader, infoHeader);
        frameData[i] = processPixels(file, infoHeader->biWidth, infoHeader->biHeight);
        fclose(file);
    }
}

void playFrames(int numFrames, int*** frameData, BMPInfoHeader infoHeader, char VIDEO_NAME[50], int TARGET_FPS) {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    playMP3(VIDEO_NAME);
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    int frame = 1;
    while (1) {
        clock_gettime(CLOCK_MONOTONIC, &end);
        double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1000000000.0;
        int target_frame = (int)(elapsed * TARGET_FPS) + 1;
        if (target_frame > numFrames) {
            break;
        }
        if (target_frame > frame) {
            curserToTopLeftTop();
            renderFrame(w, infoHeader, frameData[target_frame]);
            frame = target_frame;
        }
    }
}

void freeFrames(int numFrames, int*** frameData) {
    for (int i = 1; i <= numFrames; i++) free(frameData[i]);
    free(frameData);
}