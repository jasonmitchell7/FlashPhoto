/*******************************************************************************
 * Name            : toolbelt.h
 * Project         : FlashPhoto
 * Module          : Tools
 * Description     : Header file for the Toolbelt class, used by FlashPhoto.
 * Copyright       : 2016 CSCI3081W Group C03. All rights reserved.
 * Creation Date   : 10/16/16
 * Original Author : Erik Husby, J. Jason Mitchell, William Meusing
 *
 ******************************************************************************/

#ifndef PROJECT_ITERATION2_SRC_INCLUDE_TOOLBELT_H_
#define PROJECT_ITERATION2_SRC_INCLUDE_TOOLBELT_H_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <vector>
#include "./pixel_buffer.h"
#include "./color_data.h"
#include "./tool.h"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
namespace image_tools {

/*******************************************************************************
 * Class Definitions
 ******************************************************************************/
/**
 * @brief The purpose of this class is to store information about the tools
 * accessed by changing the currently selected tool on the application UI,
 * as well as the active color for drawing.
 **/
class ToolBelt {
 public:
    ToolBelt(
            PixelBuffer* display_buffer,
            int active_tool,
            ColorData active_color);
    virtual ~ToolBelt(void) { tools_.clear(); }
  
    /**
     * @brief Sets the active tool of the toolbelt
     * 
     * @param[in] index The index of the new active tool
     */
    inline void set_active_tool(int index) { active_tool_ = index; }
  
    /**
     * @brief Sets the color to be used by the toolbelt's tools
     * 
     * @param[in] new_active_color The new active color
     */
    inline void set_active_color(ColorData new_active_color) {
        active_color_ = new_active_color; }

    inline Tool* get_active_tool() { return tools_[active_tool_]; }
    inline ColorData get_active_color() { return active_color_; }
    inline PixelBuffer* get_pixel_buffer() { return my_pixel_buffer_; }
    inline PixelBuffer* set_pixel_buffer(PixelBuffer* new_pixel_buffer) {
      my_pixel_buffer_ = new_pixel_buffer;
    }
    inline Tool* get_buffer_stamper() { return tools_[5]; }

 private:
    /**
     * @brief Adds a new tool to the toolbelt.
     *
     */
    inline void add_tool(Tool* new_tool) { tools_.push_back(new_tool); }

    // A pointer to pixel data for the drawing canvas.
    PixelBuffer* my_pixel_buffer_;

    // Pointers to instances of every tool selectable on the BrushWorkApp UI.
    std::vector<Tool *> tools_;

    // Index of the currently selected tool from UI in tools_ vector.
    int active_tool_;

    // A copy of the active (tool) color as set on the BrushWorkApp UI.
    ColorData active_color_;
};
}  // namespace image_tools

#endif  // PROJECT_ITERATION2_SRC_INCLUDE_TOOLBELT_H_
