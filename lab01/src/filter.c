#include "pngparser.h"
#include <stdio.h>
#include <string.h>

/* This filter iterates over the image and calculates the average value of the
 * color channels for every pixel This value is then written to all the channels
 * to get the grayscale representation of the image
 */
void filter_grayscale(struct image *img, void *weight_arr) {
  struct pixel(*image_data)[img->size_x] =
      (struct pixel(*)[img->size_x])img->px;
  double *weights = (double *)weight_arr;
  /* BUG!
   * This bug isn't graded.
   *
   * FIX: Initialize both variables to 0.
   */
  for (unsigned short i = 0; i < img->size_y; i++) {
    for (unsigned short j = 0; j < img->size_x; j++) {
      double luminosity = 0;

      luminosity += weights[0] * image_data[i][j].red;
      luminosity += weights[1] * image_data[i][j].green;
      luminosity += weights[2] * image_data[i][j].blue;

      image_data[i][j].red = (uint8_t)luminosity;
      image_data[i][j].green = (uint8_t)luminosity;
      image_data[i][j].blue = (uint8_t)luminosity;
    }
  }
}

/* This filter blurs an image. The larger the radius, the more noticeable the
 * blur.
 *
 * For every pixel we define a square of side 2*radius+1 centered around it.
 * The new value of the pixel is the average value of all pixels in the square.
 *
 * Pixels of the square which fall outside the image do not count towards the
 * average. They are ignored (e.g. 5x5 box will turn into a 3x3 box in the
 * corner).
 *
 */
void filter_blur(struct image *img, void *r) {
  struct pixel(*image_data)[img->size_x] =
      (struct pixel(*)[img->size_x])img->px;
  int radius = *((int *)r);

  struct pixel(*new_data)[img->size_x] =
      malloc(sizeof(struct pixel) * img->size_x * img->size_y);

  if (!new_data) {
    return;
  }

  /* We iterate over all pixels */
  for (long i = 0; i < img->size_y; i++) {
    for (long j = 0; j < img->size_x; j++) {

      unsigned red = 0, green = 0, blue = 0, alpha = 0;

      /* We iterate over all pixels in the square */
      int num_pixels = 0;
      for (long y_offset = -radius; y_offset <= radius; y_offset++) {
        for (long x_offset = -radius; x_offset <= radius; x_offset++) {

          /* BUG!
           * This bug isn't graded.
           *
           * FIX: Limit reads only to valid memory
           */
          if (i + y_offset >= 0 && i + y_offset < img->size_y &&
              j + x_offset >= 0 && j + x_offset < img->size_x) {
            struct pixel current = image_data[i + y_offset][j + x_offset];

            red += current.red;
            blue += current.blue;
            green += current.green;
            alpha += current.alpha;
            num_pixels += 1;
          }
        }
      }

      /* Calculate the average */
      if (num_pixels != 0) {
        red /= num_pixels;
        green /= num_pixels;
        blue /= num_pixels;
        alpha /= num_pixels;
      }

      /* Assign new values */
      new_data[i][j].red = red;
      new_data[i][j].green = green;
      new_data[i][j].blue = blue;
      new_data[i][j].alpha = alpha;
    }
  }

  free(img->px);
  img->px = (struct pixel *)new_data;
  return;
}

/* We allocate and return a pixel */
struct pixel *get_pixel() {
  struct pixel *px = (struct pixel *)malloc(sizeof(struct pixel));
  return px;
}

/* This filter just negates every color in the image */
void filter_negative(struct image *img, void *noarg) {
  struct pixel(*image_data)[img->size_x] =
      (struct pixel(*)[img->size_x])img->px;

  /* Iterate over all the pixels */
  for (long i = 0; i < img->size_y; i++) {
    for (long j = 0; j < img->size_x; j++) {

      struct pixel current = image_data[i][j];
      struct pixel *neg = get_pixel();

      /* The negative is just the maximum minus the current value */
      neg->red = 255 - current.red;
      neg->green = 255 - current.green;
      neg->blue = 255 - current.blue;
      neg->alpha = current.alpha;

      /* Write it back */
      image_data[i][j] = *neg;
      free(neg);
    }
  }
}

void filter_transparency(struct image *img, void *transparency) {
  struct pixel(*image_data)[img->size_x] =
      (struct pixel(*)[img->size_x])img->px;
  uint8_t local_alpha = *((uint8_t *)transparency);

  /* Iterate over all pixels */
  for (long i = 0; i < img->size_y; i++) {
    for (long j = 0; j < img->size_x; j++) {

      image_data[i][j].alpha = local_alpha;
    }
  }
}

/* The filter structure comprises the filter function, its arguments and the
 * image we want to process */
struct filter {
  void (*filter)(struct image *img, void *arg);
  void *arg;
  struct image *img;
};

void execute_filter(struct filter *fil) { fil->filter(fil->img, fil->arg); }

int main(int argc, char *argv[]) {
  struct filter fil;
  char arg[256];
  char input[256];
  char output[256];
  char command[256];
  int radius;
  uint8_t alpha;
  struct image *img = NULL;
  double weights[] = {0.2125, 0.7154, 0.0721};

  /* Some filters take no arguments, while others have 1 */
  if (argc != 4 && argc != 5) {
    goto error_usage;
  }

  fil.filter = NULL;
  fil.arg = NULL;

  /* Copy arguments for easier reference */
  strncpy(input, argv[1], 256);
  strncpy(output, argv[2], 256);
  strncpy(command, argv[3], 256);

  /* If the filter takes an argument, copy it */
  if (argv[4]) {
    strncpy(arg, argv[4], 256);
  }

  /* Error when loading a png image */
  if (load_png(input, &img)) {
    exit(1);
  }

  /* Set up the filter structure */
  fil.img = img;

  /* Decode the filter */
  if (!strcmp(command, "grayscale")) {
    fil.filter = filter_grayscale;
    fil.arg = weights;
  } else if (!strcmp(command, "negative")) {
    fil.arg = NULL;
    fil.filter = filter_negative;
  } else if (!strcmp(command, "blur")) {
    /* Bad filter radius will just be interpretted as 0 - no change to the image
     */
    radius = atoi(arg);
    if (radius < 0) {
      radius = 0;
    }
    fil.filter = filter_blur;
    fil.arg = &radius;
  } else if (!strcmp(command, "alpha")) {

    char *end_ptr;
    alpha = strtol(arg, &end_ptr, 16);

    if (*end_ptr) {
      goto error_usage;
    }

    if (alpha > 255) {
      alpha = 255;
    }

    fil.filter = filter_transparency;
    fil.arg = &alpha;
  }

  /* Invalid filter check */
  if (fil.filter) {
    execute_filter(&fil);
  } else {
    goto error_filter;
  }

  if (store_png(output, img, NULL, 0)) {
    goto error_filter;
  }
  free(img->px);
  free(img);
  return 0;

error_filter:
  free(img->px);
  free(img);
error_usage:
  printf("Usage: %s input_image output_image filter_name [filter_arg]\n",
         argv[0]);
  printf("Filters:\n");
  printf("grayscale\n");
  printf("negative\n");
  printf("blur radius_arg\n");
  printf("alpha hex_alpha\n");
  return 1;
}