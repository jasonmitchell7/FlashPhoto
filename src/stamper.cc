/*******************************************************************************
 * Name            : stamper.cc
 * Project         : BrushWork
 * Module          : Tools
 * Description     : Source file for the Stamper class, a type of Brush used by
 *                   the FlashPhoto application as a drawing Tool.
 * Copyright       : 2016 CSCI3081W Group C03. All rights reserved.
 * Creation Date   : 10/16/16
 * Original Author : Erik Husby, J. Jason Mitchell, William Meusing
 *
 ******************************************************************************/

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "include/stamper.h"
#include <math.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
namespace image_tools {

/*******************************************************************************
 * Member Functions
 ******************************************************************************/
void Stamper::ApplyClick(int mouse_x, int mouse_y) {
  if (!stamp_mask_) {
    std::cout << "Stamp image has not been set." << std::endl;
    return;
  }

  PixelBuffer* display_buffer = my_toolbelt_->get_pixel_buffer();

  ColorData canvas_pixel_color = ColorData();
  ColorData new_pixel_color = ColorData();
  ColorData mask_color = ColorData();

  int mask_size_half = mask_size_ / 2;
  int canvas_y = 0;
  int canvas_x = 0;
  for (int mask_y = 0; mask_y < mask_size_; mask_y++) {
    for (int mask_x = 0; mask_x < mask_size_; mask_x++) {
      /*
       * Defining the mouse click coordinates to be at the center
       * of the stamper's drawing application on the canvas,
       * we must determine the correlation between pixels in
       * the stamp mask and pixels on the canvas.
       */
      canvas_x = mouse_x + (mask_x - mask_size_half);
      canvas_y = mouse_y + (mask_y - mask_size_half);

      /*
       * Make sure that we don't run off the edge of the canvas's
       * pixel buffer display.
       */
      if (display_buffer->is_valid_pixel(canvas_x, canvas_y)) {
        canvas_pixel_color = display_buffer->
          get_valid_pixel(canvas_x, canvas_y);
        mask_color = stamp_mask_[mask_y][mask_x];

        /*
         * Blend the color stored in this particular pixel of the
         * stamp mask with the color on the canvas, if the color
         * of the stamp mask pixel is not transparent.
         */
        if (mask_color.alpha() > 0.) {
          new_pixel_color = mask_color * stamp_intensity_ +
            canvas_pixel_color * (1. - stamp_intensity_);
          display_buffer->
            set_valid_pixel(
            canvas_x, canvas_y, new_pixel_color);
        }
      }
    }
  }
}

void Stamper::set_buffer_stamp_mask(PixelBuffer *pixel_buffer) {
  clear_stamp_mask();

  int mask_size = std::max(pixel_buffer->height(), pixel_buffer->width());
  mask_size = mask_size + (mask_size % 2);

  int padding_left = ceil((mask_size - pixel_buffer->width()) / 2);
  int padding_top = ceil((mask_size - pixel_buffer->height()) / 2);

  ColorData** new_mask = new ColorData*[mask_size];

  // Initialize the new mask.
  for (int y = 0; y < mask_size; y++) {
      new_mask[y] = new ColorData[mask_size];
  }

  for (int y = 0; y < mask_size; y++) {
    for (int x = 0; x < mask_size; x++) {
      if (y < padding_top ||
        x < padding_left ||
        y >= padding_top + pixel_buffer->height() ||
        x >= padding_left + pixel_buffer->width()) {
        // Set the padding to be transparent.
          new_mask[y][x] = ColorData(0, 0, 0, 0);
      } else {
        new_mask[y][x] = pixel_buffer->get_pixel(
          x - padding_left, y - padding_top).clamped_color();
      }
    }
  }

  stamp_mask_ = new_mask;
  mask_size_ = mask_size;
}

void Stamper::set_ppm_stamp_mask(int width, int height, double alpha,
                                 std::string image_name) {
  clear_stamp_mask();

  /*
   * The filename of the base image ASCII PPM file that we will use to
   * determine the color values of the stamp mask.
   */
  std::string base_fname = "src/stamps/" + image_name + ".ppm";
  /*
   * Base image maximum color value specified on the line after the
   * image dimensions in the base image PPM files.
   */
  int base_colormax = 255;

  int mask_width = width;
  int mask_height = height;

  /*
   * Mask width and height must be odd integers so that it may have a center
   * for drawing on the canvas with the click of a mouse.
   * If either the input width or height are even, bump up that size by one.
   */
  if (mask_width % 2 == 0) {
    mask_width++;
  }
  if (mask_height % 2 == 0) {
    mask_height++;
  }

  /*
   * The mask will be padded with zeros to make it square,
   * which should make it easier to apply the mask to the canvas.
   * The following computations determine the region of the mask that will
   * be filled by a uniform intensity value; if the mask width and height are
   * not equal, pad the smaller dimension with stripes of zeros on its sides.
   */
  int new_mask_size = std::max(mask_width, mask_height);
  int pad_width = (new_mask_size - mask_width) / 2;
  int pad_height = (new_mask_size - mask_height) / 2;

  int row_start = pad_height;
  int row_end = new_mask_size - 1 - pad_height;
  int col_start = pad_width;
  int col_end = new_mask_size - 1 - pad_width;

  // Create the new stamp mask.
  ColorData** new_stamp_mask = nullptr;
  // Create a dynamic array of pointers to ColorData.
  new_stamp_mask = new ColorData*[new_mask_size];
  for (int rowNum = 0; rowNum < new_mask_size; ++rowNum) {
    new_stamp_mask[rowNum] = new ColorData[new_mask_size];
    /*
     * Each i-th pointer is now pointing to a dynamic array
     * of ColorData values.
     */
  }


  // Read in the base image PPM file as a PPM pixel array //

  /*
   * Initialize the base image width and height variables.
   * If found in the base image PPM file, they will be updated.
   */
  int base_width = 0;
  int base_height = 0;

  std::string leftover = "";
  bool found_colormax = false;

  std::string line = "";
  std::string prev_line = "";

  /*
   * Create a filestream for reading the base image PPM file, and open it.
   * The is_open call will return false if there is a problem.
   */
  std::ifstream base_file(base_fname.c_str());
  if (base_file.is_open()) {
    // Use std::getline to parse the text file one line at a time.
    while (std::getline(base_file, line)) {
      /*
       * At this point, "line" contains the full text of the parsed line.
       * Use a stringstream to parse out the space-delimited contents of the line.
       */
      std::stringstream ss(line);

      // Using the stream operator, get the first token of the line.
      int first = 0;
      leftover = "";
      ss >> first >> leftover;

      /*
       * Once the line with the maximum color specification of "base_colormax" is found,
       * the remainder of the PPM file describes pixel colors of the base image.
       */
      if (first == base_colormax) {
        found_colormax = true;
        break;
      } else {
        prev_line = line;
      }
    }
  } else {
    std::cerr << "**StamperError: Could not open base image ASCII PPM file '" <<
              base_fname << "' for Stamper special tool." << std::endl;
    std::cerr << "Using default red & black stripes mask for all Stampers." <<
              std::endl;
    std::cerr << "Please make sure to run './bin/FlashPhoto' from the "
              " iteration2/ directory for full Stamper capabilities" <<
              std::endl;

    /*
     * Assign default checkerboard color
     * uniformly over non-transparent section of mask.
     */
    for (int rowNum = 0; rowNum < new_mask_size; rowNum++) {
      if (rowNum >= row_start && rowNum <= row_end) {
        for (int colNum = 0; colNum < new_mask_size; colNum++) {
          if (colNum >= col_start && colNum <= col_end) {
            if ((rowNum + colNum) % 20 < 10) {
              new_stamp_mask[rowNum][colNum] = ColorData(
                0, 0, 0, 1);
            } else {
              new_stamp_mask[rowNum][colNum] = ColorData(
                1, 0, 0, 1);
            }
          } else {
            new_stamp_mask[rowNum][colNum] = ColorData(
              0, 0, 0, 0);
          }
        }
      } else {
        for (int colNum = 0; colNum < new_mask_size; colNum++) {
          new_stamp_mask[rowNum][colNum] = ColorData(0, 0, 0, 0);
        }
      }
    }

    stamp_mask_ = new_stamp_mask;
    mask_size_ = new_mask_size;
    return;
  }

  /*
   * Validate maximum color and image size specifications
   * found in the base image PPM file.
   */
  if (!found_colormax) {
    std::cerr << "**StamperError: Base image PPM file does not have a"
      " maximum color value of " << base_colormax << std::endl;
    base_file.close();  // Close the base image file.
    return;
  } else {
    /*
     * The line preceding the maximum color specification specifies
     * the width and height of the base image. Record these specs.
     */
    std::stringstream ss(prev_line);
    ss >> base_width >> base_height >> leftover;
  }
  if (base_width < 1 || base_height < 1 || leftover != "") {
    std::cerr << "**StamperError: Base image PPM file does not contain"
      " valid image specifications." << std::endl;
    base_file.close();  // Close the base image file.
    return;
  }

  /*
   * Create a 2D array to store the three color integers (r, g, b)
   * that describe the color of every pixel in the base image.
   */
  float base_array[base_height][base_width][3];
  int rowNum = 0;
  int colNum = 0;
  int r = 0;
  int g = 0;
  int b = 0;
  float base_colormax_f = base_colormax;

  if (base_file.is_open()) {
    // Use std::getline to parse the text file one line at a time.
    std::string line = "";
    while (std::getline(base_file, line)) {
      /*
       * At this point, "line" contains the (r, g, b) color integers
       * of a single pixel. Add this pixel to the base image PPM array.
       */
      std::stringstream ss(line);
      ss >> r >> g >> b;

      base_array[rowNum][colNum][0] = r / base_colormax_f;
      base_array[rowNum][colNum][1] = g / base_colormax_f;
      base_array[rowNum][colNum][2] = b / base_colormax_f;

      colNum++;
      if (colNum >= base_width) {
        colNum = 0;
        rowNum++;
      }
    }  // end parse line
    base_file.close();  // Close the base image file.
  } else {
    std::cerr << "**StamperError: Could not open base image file " <<
              base_fname << std::endl; return;
  }


  // Assign colors to ColorData stamper mask //

  /*
   * Set pixel colors for the output image
   * using the following resizing algorithm.
   */
  double scale_height = mask_height / static_cast<double>(base_height);
  double scale_width  = mask_width  / static_cast<double>(base_width);
  int base_rowNum = 0;
  int base_colNum = 0;

  for (int rowNum=0; rowNum < new_mask_size; rowNum++) {
    if (rowNum >= row_start && rowNum <= row_end) {
      base_rowNum = (rowNum - row_start) / scale_height;
      /*
       * If mask & base image heights are not multiples of one other,
       * we could run off the bottom of the the base PPM array.
       */
      if (base_rowNum >= base_height) {
        base_rowNum = base_height - 1;
      }
      for (int colNum = 0; colNum < new_mask_size; colNum++) {
        if (colNum >= col_start && colNum <= col_end) {
            base_colNum = (colNum - col_start) / scale_width;
            /*
             * If mask & base image widths are not multiples of one other,
             * we could run off the right side of the the base PPM array.
             */
            if (base_colNum >= base_width) {
                base_colNum = base_width - 1;
            }

            /*
             * The pixel from the base image PPM array at index
             * [base_rowNum][base_colNum] was selected to be used
             * in the stamper mask at index [rowNum][colNum].
             * Copy this pixel's (r, g, b) color values into the
             * stamper mask.
             */
            new_stamp_mask[rowNum][colNum] = ColorData(
              base_array[base_rowNum][base_colNum][0],
              base_array[base_rowNum][base_colNum][1],
              base_array[base_rowNum][base_colNum][2],
              alpha);
        } else {
          new_stamp_mask[rowNum][colNum] = ColorData(0, 0, 0, 0);
        }
      }
    } else {
      for (int colNum = 0; colNum < new_mask_size; colNum++) {
        new_stamp_mask[rowNum][colNum] = ColorData(0, 0, 0, 0);
      }
    }
  }

  stamp_mask_ = new_stamp_mask;
  mask_size_ = new_mask_size;
}

void Stamper::clear_stamp_mask(void) {
  if (stamp_mask_) {
    for (int y = 0; y < mask_size_; y++) {
      if (stamp_mask_[y]) {
        delete stamp_mask_[y];
      }
    }

    delete stamp_mask_;
  }
}

}  /* namespace image_tools */
