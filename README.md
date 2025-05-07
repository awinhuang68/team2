# Convert a Color Image to Grayscale
- Objective: Read a PPM image and write a new grayscale version.
- Method: Use the luminance formula: gray = 0.3*R + 0.59*G + 0.11*B.
- Output: New PPM file in grayscale.
- Hint: Replace R, G, B of each pixel with the grayscale value

# Compile code
```c
gcc -o resize_image image.c
```

# Run code
```sehll
./resize_image input.ppm output.ppm
```

# Test pull request by write permission
