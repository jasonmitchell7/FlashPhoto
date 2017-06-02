/*******************************************************************************
 * Name            : filter_manager.h
 * Project         : FlashPhoto
 * Module          : filter_manager
 * Description     : Header for FilterManager class
 * Copyright       : 2016 CSCI3081W TAs. All rights reserved.
 * Creation Date   : Wed Sep 21 18:40:20 2016
 * Original Author : jharwell, Erik Husby, J. Jason Mitchell, William Meusing
 *
 ******************************************************************************/

#ifndef PROJECT_ITERATION2_SRC_INCLUDE_FILTER_MANAGER_H_
#define PROJECT_ITERATION2_SRC_INCLUDE_FILTER_MANAGER_H_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "GL/glui.h"
#include "./filter_kernel.h"
#include "./pixel_buffer.h"
#include "./ui_ctrl.h"
#include "./io_manager.h"
#include "./state_manager.h"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
namespace image_tools {

/*******************************************************************************
 * Class Definitions
 ******************************************************************************/
/**
 * @brief Manager for all aspects of filters in FlashPhoto, including
 * initialization of GLUI control elements for filters, filter creation,
 * application, deletion.
 */

class FilterManager {
 public:
  FilterManager();
  ~FilterManager() {}

  /**
   * @brief Sets the pixel buffer to be used by the filter manager to the supplied pixel buffer
   *
   * @param[in] pixel_buffer The pixel buffer to be used by the filter manager
   */
  inline void set_pixel_buffer(PixelBuffer *const pixel_buffer) {
    pixel_buffer_ = pixel_buffer;
  }

  /**
   * @brief Apply a blur filter to the buffer, blurring sharply defined edges
   */
  void ApplyBlur(void);

  /**
   * @brief Apply a sharpening filter to the buffer, sharpening blurry/undefined
   * edges
   */
  void ApplySharpen(void);

  /**
   * @brief Apply a motion blurring filter to the buffer
   */
  void ApplyMotionBlur(void);

  /**
   * @brief Apply an edge detection filter to the buffer
   */
  void ApplyEdgeDetect(void);

  /**
   * @brief Apply a threshold detection filter to the buffer
   */
  void ApplyThreshold(void);

  /**
   * @brief Apply the channel filter to the buffer
   */
  void ApplyChannel(void);

  /**
   * @brief Apply the channel filter to the buffer
   */
  void ApplySaturate(void);
  
  /**
   * @brief Apply the quantization filter to the buffer
   */
  void ApplyQuantize(void);

  /**
   * @brief Apply a special filter to the buffer
   *
   */
  void ApplySpecial(void);

  /**
   * @brief Initialize the elements of the GLUI interface required by the
   * FilterManager
   *
   * @param[in] glui GLUI handle
   * @param[in] s_gluicallback Callback to install
   */
  void InitGlui(const GLUI *const glui,
                void (*s_gluicallback)(int));

 private:
  /**
   * @brief Apply a convolution filter (i.e. a filter that requires a kernel) to the canvas
   *
   * @param[in] bias The bias, which specifies an offset for the color produced on the canvas
   */
  void ApplyConvolutionFilter(float bias);
  
  /**
   * @brief Apply a non-convolution filter to the canvas
   *
   * @param[in] non_convolution_function The non-convolution function to be used
   */
  void ApplyNonConvolutionFilter(
          ColorData (FilterManager::* non_convolution_function)(int x, int y));
  /**
   * @brief Change the red, blue, green values of a color to 0 or 1 depending on if it's greater than, or less than the threshold value
   *
   * @param[in] x The column index of the pixel in the pixel buffer
   * @param[in] y The row index of the pixel in the pixel buffer
   * 
   * @return The color at x,y with the threshold function applied
   */
  ColorData Threshold(int x, int y);
  
  /**
   * @brief Scale the red, blue, green values of a color according to the saturate value
   *
   * @param[in] x The column index of the pixel in the pixel buffer
   * @param[in] y The row index of the pixel in the pixel buffer
   * 
   * @return The color at x,y scaled by the saturate value
   */
  ColorData Saturate(int x, int y);
  
  /**
   * @brief Scale the red, blue, green channels according to the different channel factors
   *
   * @param[in] x The column index of the pixel in the pixel buffer
   * @param[in] y The row index of the pixel in the pixel buffer
   * 
   * @return The color at x,y with its r, g, b values scaled independently
   */
  ColorData Channel(int x, int y);
  
  /**
   * @brief Constrain all of the colors in the image to a set number of colors
   *
   * @param[in] x The column index of the pixel in the pixel buffer
   * @param[in] y The row index of the pixel in the pixel buffer
   * 
   * @return The binned color at x, y
   */
  ColorData Quantize(int x, int y);

  float channel_color_red_;
  float channel_color_green_;
  float channel_color_blue_;
  float saturation_amount_;
  float threshold_amount_;
  float blur_amount_;
  float sharpen_amount_;
  float motion_blur_amount_;
  enum UICtrl::MotionBlurDirection motion_blur_direction_;
  int quantize_bins_;

  PixelBuffer* pixel_buffer_;
  FilterKernel kernel_;
};

}  /* namespace image_tools */
#endif  /* PROJECT_ITERATION2_SRC_INCLUDE_FILTER_MANAGER_H_ */
