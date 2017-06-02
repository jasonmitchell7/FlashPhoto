/*******************************************************************************
 * Name            : tool.h
 * Project         : FlashPhoto
 * Module          : Tools
 * Description     : Header file for the Tool class, used by the FlashPhoto
 *                   application to interact with elements on the display.
 * Copyright       : 2016 CSCI3081W Group C03. All rights reserved.
 * Creation Date   : 10/16/16
 * Original Author : Erik Husby, J. Jason Mitchell, William Meusing
 *
 ******************************************************************************/

#ifndef PROJECT_ITERATION2_SRC_INCLUDE_TOOL_H_
#define PROJECT_ITERATION2_SRC_INCLUDE_TOOL_H_

/*******************************************************************************
 * Includes
 ******************************************************************************/

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
namespace image_tools {

/*******************************************************************************
 * Class Definitions
 ******************************************************************************/
class ToolBelt;  // Forward declaration of ToolBelt for cross references.
/**
 * @brief This is a base class for all tools that are selectable on the tool UI
 * for the application.
 **/
class Tool {
 public:
    virtual ~Tool(void) {}

    /**
     * @brief Draws the tool's mask on the canvas at the position where the user
     * clicked the mouse.
     */
    virtual void ApplyClick(int x, int y) = 0;

    /**
     * @brief Draws the tool's mask on the canvas at every pixel sitting on a
     * line connects the first mouse position to the second mouse position.
     * NOTE: This function is deprecated, since a more complex interpolation
     * method has been implemented for MouseDragged events in application.
     */
    virtual void ApplyDragged(int x1, int y1, int x2, int y2) = 0;

 protected:
    /**
     * A pointer to a container of instances of every selectable tool on the
     * tool UI for the running application that created this tool.
     */
    ToolBelt* my_toolbelt_ = nullptr;
};
}  // namespace image_tools

#endif  // PROJECT_ITERATION2_SRC_INCLUDE_TOOL_H_
