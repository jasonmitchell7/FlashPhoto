/*******************************************************************************
 * Name            : brush.h
 * Project         : FlashPhoto
 * Module          : Tools
 * Description     : Header file for the Brush class, used by the FlashPhoto
 *                   application as a drawing Tool.
 * Copyright       : 2016 CSCI3081W Group C03. All rights reserved.
 * Creation Date   : 10/16/16
 * Original Author : Erik Husby, J. Jason Mitchell, William Meusing
 *
 ******************************************************************************/

#ifndef PROJECT_ITERATION2_SRC_INCLUDE_BRUSH_H_
#define PROJECT_ITERATION2_SRC_INCLUDE_BRUSH_H_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <cmath>
#include <iostream>
#include "./color_data.h"
#include "./pixel_buffer.h"
#include "./toolbelt.h"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
namespace image_tools {

/*******************************************************************************
 * Class Definitions
 ******************************************************************************/
/**
 * @brief This is a base class for all brushes that are selectable
 * on the tool UI for the application.
 */
class Brush: public Tool {
 public:
    /**
     * @brief Default constructor, to be called
     * when inhertited classes are initialized.
     */
    Brush() {}

    /**
     * @brief Constructor for brushes with rectangular masks.
     *
     * @param[in] my_toolbelt Pointer to the toolbelt that uses the brush
     * @param[in] width The width of the brush's mask
     * @param[in] height The height of the brush's mask
     * @param[in] intensity The intensity of the brush, determines how opaque it is when applied to the canvas
     * @param[in] uses_luminance Boolean flag which indicates whether the brush uses the luminance of the pixels
     * it alters
     */
    Brush(ToolBelt* my_toolbelt,
          int width, int height, double intensity,
          bool uses_luminance) {
      my_toolbelt_ = my_toolbelt;
      set_mask_rectangle(width, height, intensity);
      uses_luminance_ = uses_luminance;
    }

    /**
     * @brief Constructor for brushes with circular masks.
     *
     * @param[in] diameter The diameter of the brush's mask
     * @param[in] intensity_center The central intensity of the brush's mask
     * @param[in] intensity_outer The outer intensity of the brush's mask
     * @param[in] uses_luminance Boolean flag which indicates whether the brush uses the luminance of the pixels
     * it alters
     */
    Brush(ToolBelt* my_toolbelt,
          int diameter, double intensity_center, double intensity_outer,
          bool uses_luminance) {
      my_toolbelt_ = my_toolbelt;
      set_mask_circle(diameter, intensity_center, intensity_outer);
      uses_luminance_ = uses_luminance;
    }
  
    /**
     * @brief Destructor for brushes, deletes the mask
     */
    virtual ~Brush(void) {
      if (mask_) {
        for (int mask_row = 0; mask_row < mask_size_; mask_row++) {
          delete mask_[mask_row];
        }
      }
    }

    /**
     * @brief Draws the brush's mask on the canvas, centered at the mouse
     * coordinates (x, y).
     *
     * @param[in] x Horizontal position the brush will be applied at
     * @param[in] y Vertical position the brush will be applied at
     */
    virtual void ApplyClick(int x, int y);

    /**
     * @brief Draws the brush's mask on the canvas at every pixel along a linear
     * interpolation path from mouse coordinates (x1, y1) to (x2, y2).
     * NOTE: This function is deprecated, since a more complex interpolation
     * method has been implemented for MouseDragged events in BrushWorkApp.
     *
     */
    virtual void ApplyDragged(int x1, int y1,
                              int x2, int y2);

 protected:
    /**
     * @brief Sets the mask of the brush to a rectangular shape of intensity values
     * for blending the active (tool) color set with color on the canvas.
     *
     * @param[in] width The width of the mask
     * @param[in] height The height of the mask
     * @param[in] intensity The intensity of the mask, determines the opacity of the brush
     */
    void set_mask_rectangle(int width,
                            int height,
                            double intensity);

    /**
     * @brief Sets the mask of the brush to a circular shape of intensity values
     * for blending the active (tool) color set with color on the canvas.
     * Is able to set a gradient of intensity values that fall off from a central
     * intensity value to an intensity on the circle's outer edge.
     *
     * @param[in] diameter The diameter of the mask
     * @param[in] intensity_center The central intensity of the mask
     * @param[in] intensity_outer The outer intensity of the mask
     */
    void set_mask_circle(int diameter,
                         double intensity_center,
                         double intensity_outer);

    /**
     * Grid of color intensity values that are applied to the canvas when drawing
     * with the brush, using the active color as set from the application UI.
     */
    double** mask_ = nullptr;

    /**
     * Size of one side of the square mask. This is required in order to both
     * center the mask on the mouse click on the canvas and to iterate through
     * the mask while it is drawn on the canvas.
     */
    int mask_size_ = 0;

    /**
     * Whether or not an instance of a brush takes the luminance of the current
     * canvas pixel color into account when applying itself to the canvas.
     */
    bool uses_luminance_ = false;
};
}  // namespace image_tools

#endif  // PROJECT_ITERATION2_SRC_INCLUDE_BRUSH_H_
