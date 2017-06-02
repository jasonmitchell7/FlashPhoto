/*******************************************************************************
 * Name            : blur_tool.h
 * Project         : BrushWork
 * Module          : Tools
 * Description     : Header file for the BlurTool class, a type of Brush used by
 *                   the FlashPhoto application as a drawing Tool.
 * Copyright       : 2016 CSCI3081W Group C03. All rights reserved.
 * Creation Date   : 11/17/16
 * Original Author : Erik Husby, J. Jason Mitchell, William Meusing
 *
 ******************************************************************************/

#ifndef PROJECT_ITERATION2_SRC_INCLUDE_BLUR_TOOL_H_
#define PROJECT_ITERATION2_SRC_INCLUDE_BLUR_TOOL_H_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <math.h>
#include "./brush.h"
#include "./filter_kernel.h"

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
class BlurTool : public Brush {
 public:
    BlurTool(ToolBelt* my_toolbelt, int diameter) :
            Brush(my_toolbelt, diameter,
                  static_cast<double>(diameter / 2), .5, false) {
      filter_kernel_array_ = new FilterKernel*[diameter + 1];
      set_blur_mask();
    }

 private:
    /**
     * @brief Uses the mask_ data member set via the call to the constructor
     * for parent class Brush to determine the blur kernel sizes that will be
     * applied to each pixel on the canvas.
     */
    void set_blur_mask();

    /**
     * @brief Unlike other types of brushes, the blur tool references a
     * mask of blur_kernel_ indices and uses the corresponding blur kernel
     * to blur the color of each canvas pixel by a spatially-determined amount
     * when the tool is applied to the canvas.
     *
     * @param[in] mouse_x The horizontal position of the mouse pointer
     * @param[in] mouse_y The vertical position of the mouse pointer
     */
    void ApplyClick(int mouse_x, int mouse_y);

    /**
     * @brief Grid of blur kernel sizes that act as indices into the
     * blur_kernel_ vector. This determines the strength of blurring effect
     * applied to each pixel during the mask's application to the canvas.
     */
    int** blur_mask_ = nullptr;

    /**
     * @brief An array of blur kernels of different sizes that are created
     * when the blur tool is instantiated, each stored at the index number that
     * is equal to the kernel size.
     */
    FilterKernel** filter_kernel_array_ = nullptr;
};
}  // namespace image_tools

#endif  // PROJECT_ITERATION2_SRC_INCLUDE_BLUR_TOOL_H_
