/*******************************************************************************
 * Name            : brush.cc
 * Project         : BrushWork
 * Module          : Tools
 * Description     : Source file for the Bruh class, used by the FlashPhoto
 *                   application as a drawing Tool.
 * Copyright       : 2016 CSCI3081W Group C03. All rights reserved.
 * Creation Date   : 10/16/16
 * Original Author : Erik Husby, J. Jason Mitchell, William Meusing
 *
 ******************************************************************************/

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "include/brush.h"
#include <cmath>
#include <iostream>
#include "include/color_data.h"
#include "include/pixel_buffer.h"
#include "include/toolbelt.h"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
namespace image_tools {

/*******************************************************************************
 * Member Functions
 ******************************************************************************/
void Brush::ApplyClick(int mouse_x, int mouse_y) {
  PixelBuffer* display_buffer = my_toolbelt_->get_pixel_buffer();

  ColorData active_color = my_toolbelt_->get_active_color();
  ColorData background_color = display_buffer->background_color();
  ColorData canvas_pixel_color = ColorData();
  ColorData new_pixel_color = ColorData();

  int mask_size_half = mask_size_ / 2;
  int canvas_y = 0;
  int canvas_x = 0;
  double intensity = 1.;
  for (int mask_y = 0; mask_y < mask_size_; mask_y++) {
    for (int mask_x = 0; mask_x < mask_size_; mask_x++) {
      /*
       * Defining the mouse click coordinates to be at the center
       * of the brush's drawing application on the canvas,
       * we must determine the correlation between pixels in
       * the brush mask and pixels on the canvas.
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
        intensity = mask_[mask_y][mask_x];

        /*
         * In the case that we are applying the highlighter to the
         * canvas, intensity should be scaled by the luminance of
         * the current pixel color on the canvas.
         */
        if (uses_luminance_) {
          intensity = intensity * canvas_pixel_color.luminance();
        }

        if (intensity > 0.) {
          new_pixel_color = active_color * intensity +
            canvas_pixel_color * (1. - intensity);
        } else {
          /*
           * Case for the eraser type of brush, where the background
           * color of the pixel buffer display becomes the active color.
           */
          intensity = fabs(intensity);
          new_pixel_color = background_color * intensity +
            canvas_pixel_color * (1. - intensity);
        }
        display_buffer->
          set_valid_pixel(canvas_x, canvas_y, new_pixel_color);
      }
    }
  }
}

void Brush::ApplyDragged(int x1, int y1, int x2, int y2) {
  int x_change = x2 - x1;
  int y_change = y2 - y1;

  int x_change_abs = abs(x_change);
  int y_change_abs = abs(y_change);

  if (x_change_abs > y_change_abs) {
    double line_slope = static_cast<double>(y_change) / x_change;
    for (int x_shift = 1; x_shift <= x_change_abs; x_shift++)
      ApplyClick(x1 + x_shift,
                 static_cast<int>(y1 + x_shift * line_slope));
  } else {
    double line_slope = static_cast<double>(x_change) / y_change;
    for (int y_shift = 1; y_shift <= y_change_abs; y_shift++)
      ApplyClick(static_cast<int>(x1 + y_shift * line_slope),
                 y1 + y_shift);
  }
}


// Mask calculation methods //

void Brush::set_mask_rectangle(int width,
                               int height,
                               double intensity) {
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

  // Create the new mask.
  double** new_mask = nullptr;
  // Create a dynamic array of pointers to double.
  new_mask = new double*[new_mask_size];
  for (int rowNum = 0; rowNum < new_mask_size; rowNum++) {
    new_mask[rowNum] = new double[new_mask_size];
    /*
     * Each i-th pointer is now pointing to a dynamic array
     * of double values.
     */
  }

  // Assign input intensity value uniformly over nonzero section of mask.
  for (int rowNum = 0; rowNum < new_mask_size; rowNum++) {
    if (rowNum >= row_start && rowNum <= row_end) {
      for (int colNum = 0; colNum < new_mask_size; colNum++) {
        if (colNum >= col_start && colNum <= col_end) {
          new_mask[rowNum][colNum] = intensity;
        } else {
          new_mask[rowNum][colNum] = 0.;
        }
      }
    } else {
      for (int colNum = 0; colNum < new_mask_size; colNum++) {
        new_mask[rowNum][colNum] = 0.;
      }
    }
  }

  mask_ = new_mask;
  mask_size_ = new_mask_size;
}

void Brush::set_mask_circle(int diameter,
                            double center_intensity,
                            double outer_intensity) {
  int new_mask_diameter = diameter;

  /*
   * Mask diameter must be an odd integer so that it may have a center
   * for drawing on the canvas with the click of a mouse.
   * If the input diameter is even, bump up that size by one.
   */
  if (new_mask_diameter % 2 == 0) {
    new_mask_diameter++;
  }

  // Find the center pixel of the mask.
  int center_index = diameter / 2;

  // Calculate the falloff rate per pixel from the center, if necessary.
  double falloff_rate = 0.;
  if (center_index > 0) {
    falloff_rate = (outer_intensity - center_intensity) / center_index;
  }

  // Create the new mask.
  double** new_mask = nullptr;
  // Create a dynamic array of pointers to double.
  new_mask = new double*[new_mask_diameter];
  for (int rowNum = 0; rowNum < new_mask_diameter; rowNum++) {
    new_mask[rowNum] = new double[new_mask_diameter];
    /*
     * Each i-th pointer is now pointing to a dynamic array
     * of double values.
     */
  }

  /*
   * Assign nonzero intensity values to pixels within the circle of the mask,
   * calculating their values based off an intensity falloff rate and the
   * distance of each pixel from the center of the circle.
   */
  for (int rowNum = 0; rowNum < new_mask_diameter; rowNum++) {
    for (int colNum = 0; colNum < new_mask_diameter; colNum++) {
      double distance_to_center = sqrt(
              pow(rowNum - center_index, 2) +
              pow(colNum - center_index, 2));

      double intensity = center_intensity;
      if (distance_to_center > center_index) {
        intensity = 0.;
      } else {
        intensity = intensity + falloff_rate * distance_to_center;
        if ((falloff_rate < 0. && intensity < outer_intensity) ||
            (falloff_rate > 0. && intensity > outer_intensity)) {
          intensity = outer_intensity;
        }
      }

      new_mask[rowNum][colNum] = intensity;
    }
  }

  mask_ = new_mask;
  mask_size_ = new_mask_diameter;
}

}  /* namespace image_tools */
