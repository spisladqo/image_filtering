#define LOADBMP_IMPLEMENTATION
#include "../loadbmp.h"
#include "../common.h"

#define filterWidth 7
#define filterHeight 7

double filter_num_sum(double **array, size_t x_max, size_t y_max)
{
  double total = 0;
  for (size_t x = 0; x < x_max; x++)
    for (size_t y = 0; y < y_max; y++)
      total += array[x][y];
  return total;
}

#define FILTER_SUM filter_num_sum

double filter[filterHeight][filterWidth] =
    {
        0, 0, 0, 1, 0, 0, 0,
        0, 0, 1, 1, 1, 0, 0,
        0, 1, 1, 1, 1, 1, 0,
        1, 1, 1, 1, 1, 1, 1,
        0, 1, 1, 1, 1, 1, 0,
        0, 0, 1, 1, 1, 0, 0,
        0, 0, 0, 1, 0, 0, 0};

double factor = 1.0 / 25.0;
double bias = 0.0;

int main(int argc, char *argv[])
{
  // todo: automatic h and w
  // einstein 640 × 782
  // renoir 384 × 286
  // starry night 2728 × 2160
  unsigned int w = 640, h = 782;
  char *init_filename = "example_images/input/einstein.bmp";
  char *result_filename = "example_images/result/einstein.bmp";
  unsigned char *initial_image, *result_image;
  unsigned int bits_per_pixel = 1;

  initial_image = malloc(w * h * bits_per_pixel);
  result_image = malloc(w * h * bits_per_pixel);
  if (!initial_image || !result_image)
  {
    printf("Failed to allocate memory for initial or result image\n");
    return -1;
  }

  unsigned int err = loadbmp_decode_file(init_filename, &initial_image, &w, &h, bits_per_pixel);
  if (err)
  {
    printf("Failed to load bmp: error %u\n", err);
    free(initial_image);
    free(result_image);
    return err;
  }

  // apply the filter
  for (int x = 0; x < w; x++)
    for (int y = 0; y < h; y++)
    {
      double red = 0.0, green = 0.0, blue = 0.0;

      // multiply every value of the filter with corresponding image pixel
      for (int filterY = 0; filterY < filterHeight; filterY++)
        for (int filterX = 0; filterX < filterWidth; filterX++)
        {
          int imageX = (x - filterWidth / 2 + filterX + w) % w;
          int imageY = (y - filterHeight / 2 + filterY + h) % h;
          red += initial_image[imageY * w + imageX] * filter[filterY][filterX];
          green += initial_image[imageY * w + imageX + 1] * filter[filterY][filterX];
          blue += initial_image[imageY * w + imageX + 2] * filter[filterY][filterX];
        }

      // truncate values smaller than zero and larger than 255
      result_image[y * w + x] = min(max((int)(factor * red + bias), 0), 255);
      result_image[y * w + x + 1] = min(max((int)(factor * green + bias), 0), 255);
      result_image[y * w + x + 2] = min(max((int)(factor * blue + bias), 0), 255);

      // take absolute value and truncate to 255
      // result_image[y * w + x] = min(abs((int)(factor * red + bias)), 255);
      // result_image[y * w + x + 1] = min(abs((int)(factor * green + bias)), 255);
      // result_image[y * w + x + 2] = min(abs((int)(factor * blue + bias)), 255);
    }

  loadbmp_encode_file(result_filename, result_image, w, h, bits_per_pixel);
  free(initial_image);
  free(result_image);
}