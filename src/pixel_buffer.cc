/*******************************************************************************
 * Name            : pixel_buffer.cc
 * Project         : BrushWork
 * Module          : utils
 * Description     : Implementation of PixelBuffer class
 * Copyright       : 2016 CSCI3081W TAs. All rights reserved.
 * Creation Date   : 2/15/15
 * Original Author : Seth Johnson, Erik Husby, J. Jason Mitchell, William Meusing
 *
 ******************************************************************************/

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "./include/pixel_buffer.h"
#include <iostream>
#include <cstring>
#include "./include/color_data.h"


/*******************************************************************************
 * Namespace Definitions
 ******************************************************************************/
using std::cerr;
using std::endl;
namespace image_tools {

/*******************************************************************************
 * Constructors/Destructors
 ******************************************************************************/
  PixelBuffer::PixelBuffer(int w,
                           int h,
                           ColorData background_color)
      : width_(w),
        height_(h),
        pixels_(new ColorData[w*h]),
        background_color_(new ColorData(background_color)) {
    FillPixelBufferWithColor(background_color);
  }

  PixelBuffer::~PixelBuffer(void) {
    delete [] pixels_;
    delete background_color_;
  }

/*******************************************************************************
 * Member Functions
 ******************************************************************************/

  bool PixelBuffer::is_valid_pixel(int x, int y) const {
    bool is_valid_pixel = true;
    if ((x < 0) || (x >= width_) || (y < 0) || (y >= height_)) {
      is_valid_pixel = false;
    }
    return is_valid_pixel;
  }

  ColorData PixelBuffer::get_valid_pixel(int x, int y) const {
    ColorData pixel_data;
    int index = x + width_*(height_-(y+1));
    pixel_data = pixels_[index];
    return pixel_data;
  }

  void PixelBuffer::set_valid_pixel(int x, int y, const ColorData& new_pixel) {
    int index = x + width_*(height_-(y+1));
    pixels_[index] = new_pixel;
  }

  ColorData PixelBuffer::get_pixel(int x, int y) const {
    ColorData pixel_data;

    if ((x < 0) || (x >= width_) || (y < 0) || (y >= height_)) {
      cerr << "getPixel: x,y out of range: " << x << " " << y << endl;
    } else {
      int index = x + width_*(height_-(y+1));
      pixel_data = pixels_[index];
    }
    return pixel_data;
  }

  void PixelBuffer::set_pixel(int x, int y, const ColorData& new_pixel) {
    if ((x < 0) || (x >= width_) || (y < 0) || (y >= height_)) {
      cerr << "setPixel: x,y out of range: " << x << " " << y << endl;
    } else {
      int index = x + width_*(height_-(y+1));
      pixels_[index] = new_pixel;
    }
  }

  void PixelBuffer::FillPixelBufferWithColor(ColorData color) {
    std::fill(pixels_, pixels_+width_*height_, color);
  }

  ColorData* PixelBuffer::GetAllPixels(void) {
    ColorData* pixels_copy = new ColorData[width_*height_];
    for (int i = 0; i < width_*height_; i++) {
      pixels_copy[i] = pixels_[i];
    }
    return pixels_copy;
  }

  void PixelBuffer::SetAllPixels(ColorData* pixels_copy) {
    for (int i = 0; i < width_*height_; i++) {
      pixels_[i] = pixels_copy[i];
    }
  }

  PixelBuffer* PixelBuffer::Copy(void) {
    PixelBuffer* pixel_buffer_copy = new PixelBuffer(
            width_, height_, *background_color_);
    pixel_buffer_copy->SetAllPixels(GetAllPixels());
    return pixel_buffer_copy;
  }

}  /* namespace image_tools */
