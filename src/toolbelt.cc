/*******************************************************************************
 * Name            : toolbelt.cc
 * Project         : BrushWork
 * Module          : Tools
 * Description     : Source file for the ToolBelt class, used by FlashPhoto.
 * Copyright       : 2016 CSCI3081W Group C03. All rights reserved.
 * Creation Date   : 10/16/16
 * Original Author : Erik Husby, J. Jason Mitchell, William Meusing
 *
 ******************************************************************************/

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <vector>
#include <string>
#include "include/pixel_buffer.h"
#include "include/color_data.h"
#include "include/tool.h"
#include "include/blur_tool.h"
#include "include/stamper.h"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
namespace image_tools {

/*******************************************************************************
 * Constructors/Destructors
 ******************************************************************************/
ToolBelt::ToolBelt(
  PixelBuffer* display_buffer,
  int active_tool,
  ColorData active_color) {
    my_pixel_buffer_ = display_buffer;
    active_tool_ = active_tool;
    active_color_ = active_color;

    // Initialize the toolbelt by adding all of the currently available tools.
    add_tool(reinterpret_cast<Tool *>(
               new Brush(this, 3, 1., 1., false)));     // Pen
    add_tool(reinterpret_cast<Tool *>(
               new Brush(this, 21, -1., -1., false)));  // Eraser
    add_tool(reinterpret_cast<Tool *>(
               new Brush(this, 41, .2, 0., false)));    // Spray Can
    add_tool(reinterpret_cast<Tool *>(
               new Brush(this, 5, 15, 1., false)));     // Calligraphy Pen
    add_tool(reinterpret_cast<Tool *>(
               new Brush(this, 5, 15, .4, true)));      // Highlighter
    add_tool(reinterpret_cast<Tool *>(
               new Stamper(this)));                     // Stamp
    add_tool(reinterpret_cast<Tool *>(
               new BlurTool(this, 41)));                // Blur
    add_tool(reinterpret_cast<Tool *>(                  // Stamper
               new Stamper(this, 200, 150, .5, 1., "pink-roses")));
}

}  /* namespace image_tools */
