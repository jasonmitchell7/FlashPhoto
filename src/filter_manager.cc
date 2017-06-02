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
#include "include/filter_manager.h"
#include <iostream>
#include "include/filter_kernel.h"
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
FilterManager::FilterManager(void) :
    channel_color_red_(0.0),
    channel_color_green_(0.0),
    channel_color_blue_(0.0),
    saturation_amount_(0.0),
    threshold_amount_(0.0),
    blur_amount_(0.0),
    sharpen_amount_(0.0),
    motion_blur_amount_(0.0),
    motion_blur_direction_(UICtrl::UI_DIR_E_W),
    quantize_bins_(0) {}

/*******************************************************************************
 * Member Functions
 ******************************************************************************/
void FilterManager::ApplyChannel(void) {
  std::cout << "Apply has been clicked for Channels with red = "
            << channel_color_red_
            << ", green = " << channel_color_green_
            << ", blue = " << channel_color_blue_ << std::endl;
  ApplyNonConvolutionFilter(&FilterManager::Channel);
}

void FilterManager::ApplySaturate(void) {
  std::cout << "Apply has been clicked for Saturate with amount = "
            << saturation_amount_ << std::endl;
  ApplyNonConvolutionFilter(&FilterManager::Saturate);
}

void FilterManager::ApplyBlur(void) {
  std::cout << "Apply has been clicked for Blur with amount = "
            << blur_amount_ << std::endl;
  kernel_.Init(blur_amount_, FilterKernel::BLUR);
  ApplyConvolutionFilter(0.);
}

void FilterManager::ApplySharpen(void) {
  std::cout << "Apply has been clicked for Sharpen with amount = "
            << sharpen_amount_ << std::endl;
  kernel_.Init(sharpen_amount_, FilterKernel::SHARPEN);
  ApplyConvolutionFilter(0.);
}

void FilterManager::ApplyMotionBlur(void) {
  std::cout << "Apply has been clicked for Sharpen with amount = "
            << motion_blur_amount_
            << " and direction " << motion_blur_direction_ << std::endl;

  switch (motion_blur_direction_) {
    case UICtrl::UI_DIR_N_S:
      kernel_.Init(motion_blur_amount_, FilterKernel::BLUR_N_S);
      break;
    case UICtrl::UI_DIR_E_W:
      kernel_.Init(motion_blur_amount_, FilterKernel::BLUR_E_W);
      break;
    case UICtrl::UI_DIR_NE_SW:
      kernel_.Init(motion_blur_amount_, FilterKernel::BLUR_NE_SW);
      break;
    default:
      kernel_.Init(motion_blur_amount_, FilterKernel::BLUR_NW_SE);
      break;
  }

  ApplyConvolutionFilter(0.);
}

void FilterManager::ApplyEdgeDetect(void) {
  std::cout << "Apply has been clicked for Edge Detect" << std::endl;
  kernel_.Init(1.5, FilterKernel::EDGE_DETECT);
  ApplyConvolutionFilter(0.);
}

void FilterManager::ApplyQuantize(void) {
  std::cout << "Apply has been clicked for Quantize with bins = "
            << quantize_bins_ << std::endl;
  if (quantize_bins_ > 1) {
    ApplyNonConvolutionFilter(&FilterManager::Quantize);
  }
}

void FilterManager::ApplyThreshold(void) {
  std::cout << "Apply Threshold has been clicked with amount ="
            << threshold_amount_ << std::endl;
  ApplyNonConvolutionFilter(&FilterManager::Threshold);
}

void FilterManager::ApplySpecial(void) {
  std::cout << "Apply has been clicked for Special" << std::endl;
  kernel_.Init(1.5, FilterKernel::EMBOSS);
  ApplyConvolutionFilter(.5);
}

void FilterManager::InitGlui(const GLUI *const glui,
                             void (*s_gluicallback)(int)) {
  new GLUI_Column(const_cast<GLUI*>(glui), true);
  GLUI_Panel *filter_panel = new GLUI_Panel(const_cast<GLUI*>(glui), "Filters");
  {
    GLUI_Panel *blur_panel = new GLUI_Panel(filter_panel, "Blur");
    {
      GLUI_Spinner * blur_amount = new GLUI_Spinner(blur_panel, "Amount:",
                                                    &blur_amount_);
      blur_amount->set_int_limits(0, 20);
      blur_amount->set_int_val(5);

      new GLUI_Button(blur_panel, "Apply",
                      UICtrl::UI_APPLY_BLUR, s_gluicallback);
    }

    GLUI_Panel *motion_blur_panel = new GLUI_Panel(filter_panel, "MotionBlur");
    {
      GLUI_Spinner*motion_blur_amount = new GLUI_Spinner(motion_blur_panel,
                                                         "Amount:",
                                                         &motion_blur_amount_);
      motion_blur_amount->set_int_limits(0, 100);
      motion_blur_amount->set_int_val(5);

      motion_blur_direction_ = UICtrl::UI_DIR_E_W;
      GLUI_RadioGroup *dir_blur = new GLUI_RadioGroup(
          motion_blur_panel,
          reinterpret_cast<int*>(&motion_blur_direction_));
      new GLUI_RadioButton(dir_blur, "North/South");
      new GLUI_RadioButton(dir_blur, "East/West");
      new GLUI_RadioButton(dir_blur, "NorthEast/SouthWest");
      new GLUI_RadioButton(dir_blur, "NorthWest/SouthEast");

      new GLUI_Button(motion_blur_panel, "Apply",
                      UICtrl::UI_APPLY_MOTION_BLUR, s_gluicallback);
    }
    GLUI_Panel *sharpen_panel = new GLUI_Panel(filter_panel, "Sharpen");
    {
      GLUI_Spinner * sharp_amount = new GLUI_Spinner(sharpen_panel,
                                                     "Amount:",
                                                     &sharpen_amount_);
      sharp_amount->set_int_limits(0, 100);
      sharp_amount->set_int_val(5);

      new GLUI_Button(sharpen_panel, "Apply",
                      UICtrl::UI_APPLY_SHARP, s_gluicallback);
    }
    GLUI_Panel *edge_det_panel = new GLUI_Panel(filter_panel, "Edge Detect");

    {
      new GLUI_Button(edge_det_panel, "Apply",
                      UICtrl::UI_APPLY_EDGE, s_gluicallback);
    }
    GLUI_Panel *thres_panel = new GLUI_Panel(filter_panel, "Threshold");
    {
      GLUI_Spinner *threshold_amount = new GLUI_Spinner(thres_panel,
                                                        "Level:",
                                                        &threshold_amount_);
      threshold_amount->set_float_limits(0, 1);
      threshold_amount->set_float_val(0.5);

      new GLUI_Button(thres_panel, "Apply",
                      UICtrl::UI_APPLY_THRESHOLD, s_gluicallback);
    }

    new GLUI_Column(filter_panel, true);

    GLUI_Panel *satur_panel = new GLUI_Panel(filter_panel, "Saturation");
    {
      GLUI_Spinner * saturation_amount = new GLUI_Spinner(satur_panel,
                                                          "Amount:",
                                                          &saturation_amount_);
      saturation_amount->set_float_limits(-10, 10);
      saturation_amount->set_float_val(1);

      new GLUI_Button(satur_panel, "Apply",
                      UICtrl::UI_APPLY_SATURATE,
                      s_gluicallback);
    }

    GLUI_Panel *channel_panel = new GLUI_Panel(filter_panel, "Channels");
    {
      GLUI_Spinner *channel_red = new GLUI_Spinner(channel_panel, "Red:",
                                                   &channel_color_red_);
      GLUI_Spinner *channel_green = new GLUI_Spinner(channel_panel,
                                                     "Green:",
                                                     &channel_color_green_);
      GLUI_Spinner *channel_blue = new GLUI_Spinner(channel_panel,
                                                    "Blue:",
                                                    &channel_color_blue_);

      channel_red->set_float_limits(0, 10);
      channel_red->set_float_val(1);
      channel_green->set_float_limits(0, 10);
      channel_green->set_float_val(1);
      channel_blue->set_float_limits(0, 10);
      channel_blue->set_float_val(1);

      new GLUI_Button(channel_panel, "Apply",
                      UICtrl::UI_APPLY_CHANNEL,
                      s_gluicallback);
    }

    GLUI_Panel *quant_panel = new GLUI_Panel(filter_panel, "Quantize");
    {
      GLUI_Spinner * quantize_bins = new GLUI_Spinner(quant_panel, "Bins:",
                                                      &quantize_bins_);
      quantize_bins->set_int_limits(2, 256);
      quantize_bins->set_int_val(8);
      quantize_bins->set_speed(0.1);

      new GLUI_Button(quant_panel, "Apply",
                      UICtrl::UI_APPLY_QUANTIZE,
                      s_gluicallback);
    }

    // YOUR SPECIAL FILTER PANEL
    GLUI_Panel *specialFilterPanel = new GLUI_Panel(filter_panel,
                                                    "Special Filter");
    {
      new GLUI_Button(specialFilterPanel,
                      "Apply",
                      UICtrl::UI_APPLY_SPECIAL_FILTER,
                      s_gluicallback);
    }
  }
} /* FilterManager::InitGlui() */

void FilterManager::ApplyConvolutionFilter(float bias) {
  int image_width = pixel_buffer_->width();
  int image_height = pixel_buffer_->height();

  PixelBuffer* buffer_copy = pixel_buffer_->Copy();

  for (int buffer_y = 0; buffer_y < image_height; buffer_y++) {
      for (int buffer_x = 0; buffer_x < image_width; buffer_x++) {
          pixel_buffer_->set_valid_pixel(
            buffer_x, buffer_y, kernel_.Apply(
              buffer_copy, buffer_x, buffer_y, bias));
    }
  }

  delete buffer_copy;
}

void FilterManager::ApplyNonConvolutionFilter(
        ColorData (FilterManager::* non_convolution_function)(int x, int y)
) {
  int image_width = pixel_buffer_->width();
  int image_height = pixel_buffer_->height();

  for (int buffer_y = 0; buffer_y < image_height; buffer_y++) {
    for (int buffer_x = 0; buffer_x < image_width; buffer_x++) {
      pixel_buffer_->set_valid_pixel(
        buffer_x, buffer_y, (this->*non_convolution_function)(
          buffer_x, buffer_y));
    }
  }
}

ColorData FilterManager::Threshold(int x, int y) {
  ColorData current_color = pixel_buffer_->get_pixel(x, y);
  float r, g, b, a;
  r = (current_color.red() >= threshold_amount_) ? 1. : 0.;
  g = (current_color.green() >= threshold_amount_) ? 1. : 0.;
  b = (current_color.blue() >= threshold_amount_) ? 1. : 0.;
  a = current_color.alpha();

  return ColorData(r, g, b, a);
}

ColorData FilterManager::Saturate(int x, int y) {
  ColorData current_color = pixel_buffer_->get_pixel(x, y);
  float r, g, b, a;
  if (saturation_amount_ < 0.) {
    r = 1 - current_color.red();
    g = 1 - current_color.green();
    b = 1 - current_color.blue();
    a = current_color.alpha();
  } else {
    r = current_color.red();
    g = current_color.green();
    b = current_color.blue();
    a = current_color.alpha();
  }

  float saturation_mag = fabs(saturation_amount_);
  float gray_value = 0.2989 * r + 0.5870 * g + 0.1140 * b;
  ColorData gray_color = ColorData(gray_value, gray_value, gray_value);
  ColorData gray_contribution = gray_color * (1 - saturation_mag);

  ColorData color_contribution = ColorData(r, g, b) * saturation_mag;
  ColorData new_color = (
    color_contribution + gray_contribution).clamped_color();

  return new_color;
}

ColorData FilterManager::Channel(int x, int y) {
  ColorData current_color = pixel_buffer_->get_pixel(x, y);
  float r, g, b, a;
  r = current_color.red();
  g = current_color.green();
  b = current_color.blue();
  a = current_color.alpha();

  return (ColorData(r * channel_color_red_,
                    g * channel_color_green_,
                    b * channel_color_blue_,
                    a)).clamped_color();
}

ColorData FilterManager::Quantize(int x, int y) {
  ColorData current_color = pixel_buffer_->get_pixel(x, y);
  float r, g, b, a;
  r = current_color.red();
  g = current_color.green();
  b = current_color.blue();
  a = current_color.alpha();

  int one_less_bin = quantize_bins_ - 1;

  int r_bin = rint(r * one_less_bin);
  int g_bin = rint(g * one_less_bin);
  int b_bin = rint(b * one_less_bin);

  float quantize_factor = 1. / one_less_bin;

  float new_r = r_bin * quantize_factor;
  float new_g = g_bin * quantize_factor;
  float new_b = b_bin * quantize_factor;

  return ColorData(new_r, new_g, new_b, a);
}

}  /* namespace image_tools */
