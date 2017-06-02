/*******************************************************************************
 * Name            : filter_manager.cc
 * Project         : FlashPhoto
 * Module          : filter_manager
 * Description     : Implementation of FilterManager class
 * Copyright       : 2016 CSCI3081W TAs. All rights reserved.
 * Creation Date   : Wed Sep 21 18:47:02 2016
 * Original Author : jharwell, Erik Husby, J. Jason Mitchell, William Meusing
 *
 ******************************************************************************/

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "include/filter_kernel.h"
#include <iostream>
#include "include/pixel_buffer.h"
#include "include/ui_ctrl.h"
#include "include/state_manager.h"
#include "include/io_manager.h"
#include "include/color_data.h"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
namespace image_tools {

/*******************************************************************************
 * Constructors/Destructor
 ******************************************************************************/


/*******************************************************************************
 * Member Functions
 ******************************************************************************/
ColorData FilterKernel::Apply(PixelBuffer *buffer_copy,
                              int buffer_x, int buffer_y, float bias) {
  ColorData color_accumulator = ColorData(0., 0., 0., 1.);

  /*
   * To prevent darkening around the edges, we need to verify the value of
   * the kernel's factor.
   */
  int factor_accumulator = 0;
  int offset = kernel_size_ / 2;
  int set_x, set_y;
  double factor;

  for (int kernel_y = 0; kernel_y < kernel_size_; kernel_y++) {
    for (int kernel_x = 0; kernel_x < kernel_size_; kernel_x++) {
      set_x = buffer_x + kernel_x - offset;
      set_y = buffer_y + kernel_y - offset;
      if (buffer_copy->is_valid_pixel(set_x, set_y)) {
        factor_accumulator += kernel_[kernel_y * kernel_size_ + kernel_x];
      }
    }
  }

  if (factor_accumulator <= 0)
    factor = 1.;
  else
    factor = 1. / factor_accumulator;

  /*
   * Now that we have the correct value for the factor
   * by which to scale our colors, we can apply the kernel.
   */
  for (int kernel_y = 0; kernel_y < kernel_size_; kernel_y++) {
    for (int kernel_x = 0; kernel_x < kernel_size_; kernel_x++) {
      set_x = buffer_x + kernel_x - offset;
      set_y = buffer_y + kernel_y - offset;
      if (buffer_copy->is_valid_pixel(set_x, set_y)) {
        color_accumulator = color_accumulator
                            + buffer_copy->get_valid_pixel(set_x, set_y)
                            * kernel_[kernel_y * kernel_size_ + kernel_x];
      }
    }
  }

  ColorData bias_color = ColorData(bias, bias, bias, 0);
  return (color_accumulator * factor + bias_color).clamped_color();
}

void FilterKernel::Init(const double filter_amount,
                        ConvolutionFilter filter_type) {
  int filter_width = static_cast<int>(rint(filter_amount * 2.));
  kernel_size_ = (!(filter_width % 2)) ? filter_width + 1 : filter_width;

  kernel_ = new float[kernel_size_ * kernel_size_];

  switch (filter_type) {
    case BLUR:
      kernel_function_ = &Blur;
      break;
    case BLUR_N_S:
      kernel_function_ = &Blur_N_S;
      break;
    case BLUR_E_W:
      kernel_function_ = &Blur_E_W;
      break;
    case BLUR_NE_SW:
      kernel_function_ = &Blur_NE_SW;
      break;
    case BLUR_NW_SE:
      kernel_function_ = &Blur_NW_SE;
      break;
    case SHARPEN:
      kernel_function_ = &Sharpen;
      break;
    case EDGE_DETECT:
      kernel_function_ = &EdgeDetect;
      break;
    default:
      kernel_function_ = &Emboss;
      break;
  }

  for (int kernel_y = 0; kernel_y < kernel_size_; kernel_y++) {
    for (int kernel_x = 0; kernel_x < kernel_size_; kernel_x++) {
      kernel_[kernel_y * kernel_size_ + kernel_x] = (*kernel_function_)(
        kernel_x, kernel_y, kernel_size_);
    }
  }
}

int FilterKernel::Blur(int x, int y, int kernel_size) {
  int middle_index = kernel_size / 2;
  int adjusted_x = (x <= middle_index) ? x : kernel_size - x - 1;
  int adjusted_y = (y <= middle_index) ? y : kernel_size - y - 1;

  return (adjusted_x >= middle_index - adjusted_y) ? 1 : 0;
}

int FilterKernel::Sharpen(int x, int y, int kernel_size) {
  int middle_index = kernel_size / 2;

  if (x == middle_index || y == middle_index)
    return (x == y) ? kernel_size * 2 - 1 : -1;
  else
    return 0;
}

int FilterKernel::EdgeDetect(int x, int y, int kernel_size) {
  int middle_index = kernel_size / 2;

  if (x != middle_index)
    return -1;
  else
    return (x == y) ? kernel_size * kernel_size - 1 : -1;
}

int FilterKernel::Emboss(int x, int y, int kernel_size) {
  int middle_index = kernel_size / 2;

  if ((x + y) < (kernel_size - 1))
    return -1;
  else if ((x + y) > (kernel_size - 1))
    return 1;
  else
    return 0;
}

}  /* namespace image_tools */
