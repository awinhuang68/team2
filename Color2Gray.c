#include <stdio.h>
#include <stdlib.h>
/*
requirement:
Objective: Read a PPM image and write a new grayscale version.
Method: Use the luminance formula: gray = 0.3R + 0.59G + 0.11*B.
Output: New PPM file in grayscale. Hint: Replace R, G, B of each pixel with the grayscale value
*/

// Function to convert a color pixel to grayscale
unsigned char toGray(unsigned char r, unsigned char g, unsigned char b) 
{
    //requirement: Method: Use the luminance formula: gray = 0.3R + 0.59G + 0.11*B.
	return (unsigned char)(0.3 * r + 0.59 * g + 0.11 * b);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <input.ppm> <output.pgm>\n", argv[0]);
        return 1;
    }

    FILE *inputFile = fopen(argv[1], "rb");
    if (!inputFile) {
        perror("Error opening input file");
        return 1;
    }

    char format[3];
    int width, height, maxVal;

    // Read PPM header
    if (fscanf(inputFile, "%2s\n%d %d\n%d\n", format, &width, &height, &maxVal) != 4 || format[0] != 'P' || format[1] != '6') {
        fprintf(stderr, "Invalid PPM file format\n");
        fclose(inputFile);
        return 1;
    }

    FILE *outputFile = fopen(argv[2], "wb");
    if (!outputFile) {
        perror("Error opening output file");
        fclose(inputFile);
        return 1;
    }

    // Write PGM header
    fprintf(outputFile, "P5\n%d %d\n%d\n", width, height, maxVal);

    unsigned char *colorPixel = malloc(3 * sizeof(unsigned char));
    if (!colorPixel) {
        perror("Memory allocation failed");
        fclose(inputFile);
        fclose(outputFile);
        return 1;
    }

    unsigned char grayPixel;
    for (int i = 0; i < width * height; i++) {
        if (fread(colorPixel, 3, 1, inputFile) != 1) {
            fprintf(stderr, "Error reading pixel data\n");
            free(colorPixel);
            fclose(inputFile);
            fclose(outputFile);
            return 1;
        }
        grayPixel = toGray(colorPixel[0], colorPixel[1], colorPixel[2]);
        fwrite(&grayPixel, 1, 1, outputFile);
    }

    free(colorPixel);
    fclose(inputFile);
    fclose(outputFile);

    printf("Conversion to grayscale completed successfully.\n");
    return 0;
}