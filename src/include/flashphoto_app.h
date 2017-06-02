/*******************************************************************************
 * Name            : flashphoto_app.h
 * Project         : FlashPhoto
 * Module          : App
 * Description     : Header file for FlashPhotoApp class
 * Copyright       : 2016 CSCI3081W TAs. All rights reserved.
 * Creation Date   : 2/15/15
 * Original Author : Seth Johnson, Erik Husby, J. Jason Mitchell, William Meusing
 *
 ******************************************************************************/

#ifndef PROJECT_ITERATION2_SRC_INCLUDE_FLASHPHOTO_APP_H_
#define PROJECT_ITERATION2_SRC_INCLUDE_FLASHPHOTO_APP_H_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <string>
#include "./base_gfx_app.h"
#include "./color_data.h"
#include "./pixel_buffer.h"
#include "./filter_manager.h"
#include "./io_manager.h"
#include "./ui_ctrl.h"
#include "./state_manager.h"
#include "./toolbelt.h"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
namespace image_tools {

/*******************************************************************************
 * Class Definitions
 ******************************************************************************/
class FlashPhotoApp : public BaseGfxApp {
 public:
  FlashPhotoApp(int width, int height);
  virtual ~FlashPhotoApp(void);

  void MouseDragged(int x, int y);
  void MouseMoved(int x, int y);
  void LeftMouseDown(int x, int y);
  void LeftMouseUp(int x, int y);
  void Display(void);
  void GluiControl(int control_id);

  /**
   * @brief Initialize the FlashPhotoApp
   *
   * @param[in] argc Unused--required by BaseGfxApp
   * @param[in] argv Unused--required by BaseGfxApp
   * @param[in] x Unused--required by BaseGfxApp
   * @param[in] y Unused--required by BaseGfxApp
   * @param[in] background_color The initial canvas color
   */
  void Init(
      int argc,
      char *argv[],
      int x,
      int y,
      ColorData background_color);

  void set_pixel_buffer(
    PixelBuffer *new_pixel_buffer, bool reset_state_manager);

 private:
  /**
   * @brief Update the colors displayed on the GLUI control panel after updating
   * their values in FlashPhoto
   *
   */
  void update_colors(void);

  /**
   * @brief Initialize the GLUI interface
   *
   */
  void InitGlui(void);

  /**
   * @brief Initialize OpenGL
   *
   */
  void InitGraphics(void);

  /**
   * @brief Initialize the buffers for the main window
   */
  void InitializeBuffers(ColorData initial_color,
                         int width, int height);

  /* Copy/move assignment/construction disallowed */
  FlashPhotoApp(const FlashPhotoApp &rhs) = delete;
  FlashPhotoApp& operator=(const FlashPhotoApp &rhs) = delete;

  /**
   * @brief Manager for all filter operations
   */
  FilterManager filter_manager_;

  /**
   * @brief Manager for all I/O operations
   */
  IOManager io_manager_;

  /**
   * @brief Manager for redo/undo stack
   */
  StateManager state_manager_;

  /**
   * @brief A collection of GLUI spinners for RGB control elements.
   */
  struct {
    GLUI_Spinner *spinner_red;
    GLUI_Spinner *spinner_green;
    GLUI_Spinner *spinner_blue;
  } glui_ctrl_hooks_;

  /** Pointer to pixel data for the screen */
  PixelBuffer *display_buffer_;

  // These are used to store the selections from the GLUI user interface
  int cur_tool_;  /**< Currently selected tool from UI */

  float cur_color_red_;
  float cur_color_green_;
  float cur_color_blue_;

  /**
 * @brief Initialize the tool container.
 */
  void InitializeToolBelt(PixelBuffer *display_buffer,
                          int active_tool,
                          ColorData active_color);

  /** Pointer to container for tool objects and active tool/color UI info */
  ToolBelt *toolbelt_;

  /** Drawing methods and member variables. */
  void DrawPixel(int x, int y);
  void InterpolateToPoint(int destination_x, int destination_y);
  struct PixelLocation {
    int x;
    int y;
  } last_draw_location_;
};

}  /* namespace image_tools */

#endif  // PROJECT_ITERATION2_SRC_INCLUDE_FLASHPHOTO_APP_H_
