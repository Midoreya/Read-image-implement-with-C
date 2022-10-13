//---------------------//
#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
//---------------------//
#include <jpeglib.h>
//---------------------//

typedef struct image {
  int w;
  int h;
  int c;
  float *data;
} image;

float *read_jpeg(char *input) {
  long i, ret;

  struct stat file_info;
  long jpg_size;
  unsigned char *jpg_buffer;

  struct jpeg_decompress_struct cinfo;
  struct jpeg_error_mgr jerr;

  long bmp_size;
  unsigned char *bmp_buffer;
  int row_stride, width, height, pixel_size;

  ret = stat(input, &file_info);

  jpg_size = file_info.st_size;
  jpg_buffer = (unsigned char *)malloc(jpg_size + 100);

  int fd = open(input, O_RDONLY);
  i = 0;
  while (i < jpg_size) {
    ret = read(fd, jpg_buffer + i, jpg_size - i);
    i += ret;
  }
  close(fd);

  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_decompress(&cinfo);

  jpeg_mem_src(&cinfo, jpg_buffer, jpg_size);

  ret = jpeg_read_header(&cinfo, TRUE);
  assert(ret == 1);

  jpeg_start_decompress(&cinfo);

  width = cinfo.output_width;
  height = cinfo.output_height;
  pixel_size = cinfo.output_components;

  bmp_size = width * height * pixel_size;
  bmp_buffer = (unsigned char *)malloc(bmp_size);
  row_stride = width * pixel_size;

  while (cinfo.output_scanline < cinfo.output_height) {
    unsigned char *buffer_array[1];
    buffer_array[0] = bmp_buffer + (cinfo.output_scanline) * row_stride;

    jpeg_read_scanlines(&cinfo, buffer_array, 1);
  }

  float *img_buffer;
  img_buffer = (float *)malloc(bmp_size * sizeof(float));

  for (i = 0; i < bmp_size; i++) {
    img_buffer[i] = bmp_buffer[i];
  }

  jpeg_finish_decompress(&cinfo);
  jpeg_destroy_decompress(&cinfo);

  free(jpg_buffer);
  free(bmp_buffer);

  return img_buffer;
}

image read_image(char *input) {
  long i, ret;

  struct stat file_info;
  long jpg_size;
  unsigned char *jpg_buffer;

  struct jpeg_decompress_struct cinfo;
  struct jpeg_error_mgr jerr;

  long bmp_size;
  unsigned char *bmp_buffer;
  int row_stride, width, height, pixel_size;

  ret = stat(input, &file_info);

  jpg_size = file_info.st_size;
  jpg_buffer = (unsigned char *)malloc(jpg_size + 100);

  int fd = open(input, O_RDONLY);
  i = 0;
  while (i < jpg_size) {
    ret = read(fd, jpg_buffer + i, jpg_size - i);
    i += ret;
  }
  close(fd);

  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_decompress(&cinfo);

  jpeg_mem_src(&cinfo, jpg_buffer, jpg_size);

  ret = jpeg_read_header(&cinfo, TRUE);
  assert(ret == 1);

  jpeg_start_decompress(&cinfo);

  width = cinfo.output_width;
  height = cinfo.output_height;
  pixel_size = cinfo.output_components;

  bmp_size = width * height * pixel_size;
  bmp_buffer = (unsigned char *)malloc(bmp_size);
  row_stride = width * pixel_size;

  while (cinfo.output_scanline < cinfo.output_height) {
    unsigned char *buffer_array[1];
    buffer_array[0] = bmp_buffer + (cinfo.output_scanline) * row_stride;

    jpeg_read_scanlines(&cinfo, buffer_array, 1);
  }

  image img;
  img.data = (float *)malloc(bmp_size * sizeof(float));

  for (i = 0; i < bmp_size; i++) {
    img.data[i] = bmp_buffer[i];
  }

  img.c = pixel_size;
  img.w = width;
  img.h = height;

  jpeg_finish_decompress(&cinfo);
  jpeg_destroy_decompress(&cinfo);

  free(jpg_buffer);
  free(bmp_buffer);

  return img;
}

/*-------------------------------------------
                  Main Functions
-------------------------------------------*/

// gcc read_jpeg.c -ljpeg

int main(int argc, char **argv) {
  char *filepath = NULL;
  filepath = (char *)malloc(1024 * sizeof(char));
  filepath = argv[1];

  float *img_buffer;
  img_buffer = read_jpeg(filepath);

  image img;
  img = read_image(filepath);

  int i = 0;
  int size = img.c * img.h * img.w;
  int stride = img.c * img.w;

  for (i = 0; i < size; i++) {
    printf("%4.0f\t", img_buffer[i]);
    if (i % stride == 0 && i != 0) printf("\n");
  }
  printf("\n");

  for (i = 0; i < size; i++) {
    printf("%4.0f\t", img.data[i]);
    if (i % stride == 0 && i != 0) printf("\n");
  }

  return 0;
}
