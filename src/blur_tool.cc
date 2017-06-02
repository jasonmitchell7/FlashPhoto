/*******************************************************************************
 * Name            : blur_tool.cc
 * Project         : BrushWork
 * Module          : Tools
 * Description     : Source file for the BlurTool class, a type of Brush used by
 *                   the FlashPhoto application as a drawing Tool.
 * Copyright       : 2016 CSCI3081W Group C03. All rights reserved.
 * Creation Date   : 11/17/16
 * Original Author : Erik Husby, J. Jason Mitchell, William Meusing
 *
 ******************************************************************************/

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "include/blur_tool.h"
#include <math.h>
#include "include/filter_kernel.h"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
namespace image_tools {

/*******************************************************************************
 * Member Functions
 ******************************************************************************/
void BlurTool::ApplyClick(int mouse_x, int mouse_y) {
  PixelBuffer* display_buffer = my_toolbelt_->get_pixel_buffer();
  PixelBuffer* buffer_copy = display_buffer->Copy();

  ColorData new_pixel_color = ColorData();

  int mask_size_half = mask_size_ / 2;
  int canvas_y = 0;
  int canvas_x = 0;
  for (int mask_y = 0; mask_y < mask_size_; mask_y++) {
    for (int mask_x = 0; mask_x < mask_size_; mask_x++) {
      /*
       * Defining the mouse click coordinates to be at the center
       * of the blur tool's drawing application on the canvas,
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
        /*
         * Determine the blur strength to use at this coordinate
         * in the blur mask and apply a blur kernel of that size
         * to the canvas at only this canvas pixel location.
         */
        int kernel_size = blur_mask_[mask_y][mask_x];

        // If the kernel size is zero, blur is not applied.
        if (kernel_size != 0) {
          new_pixel_color = filter_kernel_array_[kernel_size]->Apply(
            buffer_copy, canvas_x, canvas_y, 0.);

          display_buffer->set_valid_pixel(
            canvas_x, canvas_y, new_pixel_color);
        }
      }
    }
  }
}

void BlurTool::set_blur_mask() {
  // Create the new mask.
  int** new_blur_mask_ = nullptr;
  // Create a dynamic array of pointers to int.
  new_blur_mask_ = new int*[mask_size_];

  for (int rowNum = 0; rowNum < mask_size_; rowNum++) {
    new_blur_mask_[rowNum] = new int[mask_size_];
    /*
     * Each rowNum-th pointer is now pointing to a dynamic array
     * of int values.
     */

    /*
     * Use the intensity values from the mask_ we explicitly set by calling
     * the parent Brush class constructor for circular masks as a means to
     * determine the blur kernel size that will be applied at each pixel
     * location in the blur mask.
     */
    for (int colNum = 0; colNum < mask_size_; colNum++) {
      int kernel_size = static_cast<int>(rint(mask_[rowNum][colNum]));

      // If the kernel size is zero, a new blur kernel is not created.
      if (kernel_size != 0) {
        /*
         * Kernel size must be an odd integer so that it may have a center
         * for application on the canvas. If the input kernel size is even,
         * decrement that size by one.
         */
        if (kernel_size % 2 == 0) {
          kernel_size--;
        }

        /*
         * Determine if a blur kernel for this kernel size has been created.
         * If not, create one and shelve it.
         */
        if (!filter_kernel_array_[kernel_size]) {
          FilterKernel *new_kernel = new FilterKernel();
          new_kernel->Init(kernel_size, FilterKernel::BLUR);

          filter_kernel_array_[kernel_size] = new_kernel;
        }
      }
      new_blur_mask_[rowNum][colNum] = kernel_size;
    }
  }

  blur_mask_ = new_blur_mask_;
}

}  /* namespace image_tools */
