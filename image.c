/**
 * image.c - PPM圖像縮小程序
 * 功能：將PPM圖像縮小到原來的一半大小(2x縮小)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    unsigned char r, g, b;
} Pixel;

typedef struct {
    char format[3];  // PPM格式 (P3 或 P6)
    int width;       // 寬度
    int height;      // 高度
    int maxval;      // 最大顏色值
    Pixel *data;     // 像素數據
} PPMImage;

/**
 * 讀取PPM圖像文件
 *
 * @param filename 輸入文件名
 * @return 讀取的PPM圖像，如失敗返回NULL
 */
PPMImage* readPPM(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        fprintf(stderr, "Cannot open file: %s\n", filename);
        return NULL;
    }

    PPMImage *img = (PPMImage*)malloc(sizeof(PPMImage));
    if (!img) {
        fclose(file);
        fprintf(stderr, "Memory allocate failed\n");
        return NULL;
    }

    // 讀取PPM格式
    if (fscanf(file, "%2s", img->format) != 1) {
        fprintf(stderr, "Read failed\n");
        fclose(file);
        free(img);
        return NULL;
    }

    // 檢查格式
    if (strcmp(img->format, "P3") != 0 && strcmp(img->format, "P6") != 0) {
        fprintf(stderr, "PPM format not support: %s\n", img->format);
        fclose(file);
        free(img);
        return NULL;
    }

    // 跳過註釋行
    int c = getc(file);
    while (c == '#') {
        while (getc(file) != '\n');
        c = getc(file);
    }
    ungetc(c, file);

    // 讀取寬度、高度和最大顏色值
    if (fscanf(file, "%d %d %d", &img->width, &img->height, &img->maxval) != 3) {
        fprintf(stderr, "Read image failed\n");
        fclose(file);
        free(img);
        return NULL;
    }

    // 跳過一個空白字符
    getc(file);

    // 分配像素數據內存
    img->data = (Pixel*)malloc(img->width * img->height * sizeof(Pixel));
    if (!img->data) {
        fprintf(stderr, "Memory allocate failed\n");
        fclose(file);
        free(img);
        return NULL;
    }

    // 讀取像素數據
    if (strcmp(img->format, "P3") == 0) {
        // ASCII格式
        for (int i = 0; i < img->width * img->height; i++) {
            int r, g, b;
            if (fscanf(file, "%d %d %d", &r, &g, &b) != 3) {
                fprintf(stderr, "Read pixel failed\n");
                fclose(file);
                free(img->data);
                free(img);
                return NULL;
            }
            img->data[i].r = (unsigned char)r;
            img->data[i].g = (unsigned char)g;
            img->data[i].b = (unsigned char)b;
        }
    } else {
        // 二進制格式 (P6)
        if (fread(img->data, sizeof(Pixel), img->width * img->height, file) != img->width * img->height) {
            fprintf(stderr, "Read pixel failed\n");
            fclose(file);
            free(img->data);
            free(img);
            return NULL;
        }
    }

    fclose(file);
    return img;
}

/**
 * 保存PPM圖像到文件
 *
 * @param img PPM圖像
 * @param filename 輸出文件名
 * @return 成功返回1，失敗返回0
 */
int writePPM(PPMImage *img, const char *filename) {
    FILE *file = fopen(filename, "wb");
    if (!file) {
        fprintf(stderr, "Cannot create file: %s\n", filename);
        return 0;
    }

    // 寫入PPM頭部
    fprintf(file, "%s\n%d %d\n%d\n", img->format, img->width, img->height, img->maxval);

    // 寫入像素數據
    if (strcmp(img->format, "P3") == 0) {
        // ASCII格式
        for (int i = 0; i < img->width * img->height; i++) {
            fprintf(file, "%d %d %d ", img->data[i].r, img->data[i].g, img->data[i].b);
            if ((i + 1) % img->width == 0)
                fprintf(file, "\n");
        }
    } else {
        // 二進制格式 (P6)
        fwrite(img->data, sizeof(Pixel), img->width * img->height, file);
    }

    fclose(file);
    return 1;
}

/**
 * 釋放PPM圖像內存
 *
 * @param img 要釋放的PPM圖像
 */
void freePPM(PPMImage *img) {
    if (img) {
        if (img->data) {
            free(img->data);
        }
        free(img);
    }
}

/**
 * 將圖像縮小到原來的一半大小 (2x縮小)
 * 方法：將每個2x2像素塊平均為一個像素
 *
 * @param input 輸入PPM圖像
 * @return 縮小後的PPM圖像，如失敗返回NULL
 */
PPMImage* dwonsizeImage2X(PPMImage *input) {
    if (!input || !input->data) {
        fprintf(stderr, "無效的輸入圖像\n");
        return NULL;
    }

    // 計算新的寬度和高度（向下取整）
    int newWidth = input->width / 2;
    int newHeight = input->height / 2;

    // 創建縮小後的圖像
    PPMImage *output = (PPMImage*)malloc(sizeof(PPMImage));
    if (!output) {
        fprintf(stderr, "Allocate memory for scaled image failed\n");
        return NULL;
    }

    // 複製格式和最大顏色值
    strcpy(output->format, input->format);
    output->width = newWidth;
    output->height = newHeight;
    output->maxval = input->maxval;

    // 分配新圖像的像素數據內存
    output->data = (Pixel*)malloc(newWidth * newHeight * sizeof(Pixel));
    if (!output->data) {
        fprintf(stderr, "Allocate memory for scaled image data failed\n");
        free(output);
        return NULL;
    }

    // 將每個2x2像素塊平均為一個像素
    for (int y = 0; y < newHeight; y++) {
        for (int x = 0; x < newWidth; x++) {
            int r = 0, g = 0, b = 0;
            int count = 0;

            // 計算2x2塊中所有像素的和
            for (int dy = 0; dy < 2; dy++) {
                for (int dx = 0; dx < 2; dx++) {
                    int origX = x * 2 + dx;
                    int origY = y * 2 + dy;

                    // 確保坐標在原始圖像範圍內
                    if (origX < input->width && origY < input->height) {
                        Pixel p = input->data[origY * input->width + origX];
                        r += p.r;
                        g += p.g;
                        b += p.b;
                        count++;
                    }
                }
            }

            // 計算平均值
            if (count > 0) {
                output->data[y * newWidth + x].r = (unsigned char)(r / count);
                output->data[y * newWidth + x].g = (unsigned char)(g / count);
                output->data[y * newWidth + x].b = (unsigned char)(b / count);
            }
        }
    }

    return output;
}

/**
 * 主函數示例：讀取PPM圖像，縮小它，然後保存結果
 */
int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Uasage: %s <input_file> <output_file>\n", argv[0]);
        return 1;
    }

    // 讀取輸入圖像
    PPMImage *input = readPPM(argv[1]);
    if (!input) {
        return 1;
    }

    printf("Original image: %dx%d\n", input->width, input->height);

    // 縮小圖像
    PPMImage *output = dwonsizeImage2X(input);
    if (!output) {
        freePPM(input);
        return 1;
    }

    printf("Scaled image: %dx%d\n", output->width, output->height);

    // 保存結果
    if (!writePPM(output, argv[2])) {
        freePPM(input);
        freePPM(output);
        return 1;
    }

    printf("Images scaled and saved to: %s\n", argv[2]);

    // 釋放內存
    freePPM(input);
    freePPM(output);

    return 0;
}
