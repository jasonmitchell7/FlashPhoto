/*******************************************************************************
 * Name            : filter_kernel.h
 * Project         : BrushWork
 * Module          : utils
 * Description     : Header file for the FilterKernel class.
 * Copyright       : 2016 CSCI3081W Group C03. All rights reserved.
 * Creation Date   : 11/19/2106
 * Original Author : Erik Husby, J. Jason Mitchell, William Meusing
 *
 ******************************************************************************/

#ifndef PROJECT_ITERATION2_SRC_INCLUDE_FILTER_KERNEL_H_
#define PROJECT_ITERATION2_SRC_INCLUDE_FILTER_KERNEL_H_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <math.h>
#include "GL/glui.h"
#include "./pixel_buffer.h"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/


namespace image_tools {

/*******************************************************************************
 * Class Definitions
 ******************************************************************************/
/**
 * @brief Manages the creation and application of convolution filters
 */

class FilterKernel {
 public:
  FilterKernel() { kernel_ = nullptr;
                   kernel_function_ = &Blur; }

  ~FilterKernel() { if (kernel_ != nullptr) delete kernel_; }
  
  enum ConvolutionFilter {
    BLUR,
    BLUR_N_S,
    BLUR_E_W,
    BLUR_NE_SW,
    BLUR_NW_SE,
    SHARPEN,
    EDGE_DETECT,
    EMBOSS
  };


  /**
   * @brief Applies the convolution filter at a pixel
   *
   * @param buffer_copy A copy of the pixel buffer, is not modified here since it would break application over the entire image
   * @param buffer_x The column coordinate of the pixel in the pixel buffer that our filter will look around
   * @param buffer_y The row coordinate y of the pixel in the pixel buffer that our filter will look around
   * @param bias The offset for the color returned by the filter application
   *
   * @return The color produced by the convolution filter to be put at buffer_x, buffer_y
   */
  ColorData Apply(
    PixelBuffer *buffer_copy, int buffer_x, int buffer_y, float bias);
  
  /**
   * @brief Initializes our kernel with a certain filter function and a radius
   *
   * @param filter_amount The radius of our filter kernel
   * @param filter_type Specifies the filter function to be used
   *
   * @return The color produced by the convolution filter to be put at buffer_x, buffer_y
   */
  void Init(const double filter_amount, ConvolutionFilter filter_type);

 private:
  /**
   * @brief Get the value of the blur kernel at the column coordinate x, and row coordinate y
   * @param x The column coordinate x
   * @param y The row coordinate y
   * @param kernel_size The length/width of the kernel
   *
   * @return The value to be put into the kernel at x,y
   */
  static int Blur(int x, int y, int kernel_size);
  
  /**
   * @brief Get the value of the directional (North/South) blur kernel at the column coordinate x, and row coordinate y
   *
   * @param x The column coordinate x
   * @param y The row coordinate y
   * @param kernel_size The length/width of the kernel
   *
   * @return The value to be put into the kernel at x,y
   */
  static inline int Blur_N_S(int x, int y, int kernel_size) {
    return (x == kernel_size / 2) ? 1 : 0;
  }
  
  /**
   * @brief Get the value of the directional (East/West) blur kernel at the column coordinate x, and row coordinate y
   *
   * @param x The column coordinate x
   * @param y The row coordinate y
   * @param kernel_size The length/width of the kernel
   *
   * @return The value to be put into the kernel at x,y
   */
  static inline int Blur_E_W(int x, int y, int kernel_size) {
    return (y == kernel_size / 2) ? 1 : 0;
  }
  
  /**
   * @brief Get the value of the directional (Northeast/Southwest) blur kernel at the column coordinate x, and row coordinate y
   *
   * @param x The column coordinate x
   * @param y The row coordinate y
   * @param kernel_size The length/width of the kernel
   *
   * @return The value to be put into the kernel at x,y
   */
  static inline int Blur_NE_SW(int x, int y, int kernel_size) {
    return ((x + y) == kernel_size - 1) ? 1 : 0;
  }
  
  /**
   * @brief Get the value of the directional (Northwest/Southeast) blur kernel at the column coordinate x, and row coordinate y
   *
   * @param x The column coordinate x
   * @param y The row coordinate y
   * @param kernel_size The length/width of the kernel
   *
   * @return The value to be put into the kernel at x,y
   */
  static inline int Blur_NW_SE(int x, int y, int kernel_size) {
    return (x == y) ? 1 : 0;
  }
  
  /**
   * @brief Get the value of the sharpen kernel at the column coordinate x, and row coordinate y
   *
   * @param x The column coordinate x
   * @param y The row coordinate y
   * @param kernel_size The length/width of the kernel
   *
   * @return The value to be put into the kernel at x,y
   */
  static int Sharpen(int x, int y, int kernel_size);
  
  /**
   * @brief Get the value of the edge detection kernel at the column coordinate x, and row coordinate y
   *
   * @param x The column coordinate x
   * @param y The row coordinate y
   * @param kernel_size The length/width of the kernel
   *
   * @return The value to be put into the kernel at x,y
   */
  static int EdgeDetect(int x, int y, int kernel_size);
  
  /**
   * @brief Get the value of the emboss kernel at the column coordinate x, and row coordinate y
   *
   * @param x The column coordinate x
   * @param y The row coordinate y
   * @param kernel_size The length/width of the kernel
   *
   * @return The value to be put into the kernel at x,y
   */
  static int Emboss(int x, int y, int kernel_size);

  int kernel_size_ = 0;
  float* kernel_ = nullptr;
  int (*kernel_function_)(int x, int y, int kernel_size);
};

} /* namespace image_tools */
#endif  /* PROJECT_ITERATION2_SRC_INCLUDE_FILTER_KERNEL_H_ */
