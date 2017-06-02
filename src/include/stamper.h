/*******************************************************************************
 * Name            : stamper.h
 * Project         : FlashPhoto
 * Module          : Tools
 * Description     : Header file for the Stamper class, a type of Brush used by
 *                   the FlashPhoto application as a drawing Tool.
 * Copyright       : 2016 CSCI3081W Group C03. All rights reserved.
 * Creation Date   : 10/19/16
 * Original Author : Erik Husby, J. Jason Mitchell, William Meusing
 *
 ******************************************************************************/

#ifndef PROJECT_ITERATION2_SRC_INCLUDE_STAMPER_H_
#define PROJECT_ITERATION2_SRC_INCLUDE_STAMPER_H_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "./brush.h"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
namespace image_tools {

/*******************************************************************************
 * Class Definitions
 ******************************************************************************/
/**
 * @brief This is the class for the stamper type of brush, which may be applied
 * to the pixel buffer display.
 */
class Stamper : public Brush {
 public:
    /**
     * @brief Constructor for the Stamp tool.
     */
    explicit Stamper(ToolBelt* my_toolbelt) {
      my_toolbelt_ = my_toolbelt;
    }

    /**
     * @brief Constructor for Stamper special tool.
     *
     * @param[in] width The width of the stamper brush
     * @param[in] height The height of the stamper brush
     * @param[in] intensity The intensity of the stamper brush
     * @param[in] alpha The value of the alpha (transparency) channel
     * @param[in] image_name The name of the image used by the stamper brush
     */
    Stamper(ToolBelt* my_toolbelt,
            int width, int height,
            double intensity, double alpha,
            std::string image_name) {
      my_toolbelt_ = my_toolbelt;
      set_ppm_stamp_mask(width, height, alpha, image_name);
      stamp_intensity_ = intensity;
    }

    void set_buffer_stamp_mask(PixelBuffer* pixel_buffer);

    virtual ~Stamper(void) {
      if (stamp_mask_) {
        for (int mask_row = 0; mask_row < mask_size_; mask_row++) {
          delete stamp_mask_[mask_row];
        }
      }
    }

 private:
    void set_ppm_stamp_mask(int width, int height, double alpha,
                            std::string image_name);

    void clear_stamp_mask(void);

    /**
     * @brief Unlike other types of brushes, the blur tool references a
     * mask of blur kernel  when it is applied to the canvas.
     */
    void ApplyClick(int mouse_x, int mouse_y);

    /**
     * @brief Grid of absolute ColorData that are applied with one overall
     * intensity value to the canvas when drawing with the stamper tool.
     */
    ColorData** stamp_mask_ = nullptr;
    double stamp_intensity_ = 1.;
};
}  // namespace image_tools

#endif  // PROJECT_ITERATION2_SRC_INCLUDE_STAMPER_H_
