/*******************************************************************************
 * Name            : flashphoto_app.cc
 * Project         : FlashPhoto
 * Module          : App
 * Description     : Implementation of FlashPhoto
 * Copyright       : 2016 CSCI3081W TAs. All rights reserved.
 * Creation Date   : 2/15/15
 * Original Author : Seth Johnson, Erik Husby, J. Jason Mitchell, William Meusing
 *
 ******************************************************************************/

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "include/flashphoto_app.h"
#include <cmath>
#include <iostream>
#include "include/color_data.h"
#include "include/pixel_buffer.h"
#include "include/ui_ctrl.h"
#include "include/state_manager.h"
#include "include/filter_manager.h"
#include "include/io_manager.h"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
namespace image_tools {

/*******************************************************************************
 * Constructors/Destructor
 ******************************************************************************/
FlashPhotoApp::FlashPhotoApp(int width, int height) : BaseGfxApp(width, height),
                                                      filter_manager_(),
                                                      io_manager_(),
                                                      state_manager_(),
                                                      glui_ctrl_hooks_(),
                                                      display_buffer_(nullptr),
                                                      cur_tool_(0),
                                                      cur_color_red_(0.0),
                                                      cur_color_green_(0.0),
                                                      cur_color_blue_(0.0),
                                                      toolbelt_(nullptr) {}

/*******************************************************************************
 * Member Functions
 ******************************************************************************/
void FlashPhotoApp::Init(
    int argc,
    char* argv[],
    int x,
    int y,
    ColorData background_color) {
  BaseGfxApp::Init(argc, argv,
                   x, y,
                   GLUT_RGB|GLUT_DOUBLE|GLUT_DEPTH,
                   true,
                   width()+51,
                   50);

  // Set the name of the window
  set_caption("FlashPhoto");

  // Initialize Interface
  InitializeBuffers(background_color, width(), height());

  InitGlui();
  InitGraphics();

  InitializeToolBelt(display_buffer_,
                     cur_tool_,
                     ColorData(cur_color_red_,
                               cur_color_green_,
                               cur_color_blue_));

  state_manager_.RegisterNewCanvasState(display_buffer_->GetAllPixels());
}

void FlashPhotoApp::Display(void) {
  DrawPixels(0, 0, width(), height(), display_buffer_->data());
}

FlashPhotoApp::~FlashPhotoApp(void) {
  if (display_buffer_) {
    delete display_buffer_;
    delete toolbelt_;
  }
}

void FlashPhotoApp::set_pixel_buffer(
  PixelBuffer *new_pixel_buffer, bool reset_canvas_state) {
  display_buffer_ = new_pixel_buffer;
  toolbelt_->set_pixel_buffer(display_buffer_);
  filter_manager_.set_pixel_buffer(display_buffer_);

  BaseGfxApp::SetWindowDimensions(
    display_buffer_->width(), display_buffer_->height());

  if (reset_canvas_state) {
    state_manager_.ClearUndoAndRedoStacks();
  }

  state_manager_.RegisterNewCanvasState(display_buffer_->GetAllPixels());
}

void FlashPhotoApp::DrawPixel(int x, int y) {
  toolbelt_->get_active_tool()->ApplyClick(x, y);
  last_draw_location_ = {x, y};
}

// This function is based on Bresenham's line algorithm //
void FlashPhotoApp::InterpolateToPoint(int destination_x, int destination_y) {
  if (last_draw_location_.x >= 0 && last_draw_location_.y >= 0) {
    int delta_x = abs(destination_x - last_draw_location_.x);
    int delta_y = abs(destination_y - last_draw_location_.y);

    int direction_x = last_draw_location_.x > destination_x ? -1 : 1;
    int direction_y = last_draw_location_.y > destination_y ? -1 : 1;

    double error = -1.0;

    if (delta_x == 0 && delta_y != 0) {
      double delta_error = delta_x / delta_y;

      int x = last_draw_location_.x;

      for (int y = last_draw_location_.y;
           y != destination_y;
           y = y + direction_y) {
        if (y == destination_y && x == destination_x) {
          break;
        }

        DrawPixel(x, y);

        error = error + delta_error;

        while (error >= 0.0 &&
               !(y == destination_y && x == destination_x)) {
          x = x + direction_x;
          error = error - 1.0;

          DrawPixel(x, y);
        }
      }

      return;
    } else if (delta_x != 0) {
      double delta_error = delta_y / delta_x;

      int y = last_draw_location_.y;

      for (int x = last_draw_location_.x;
           x != destination_x;
           x = x + direction_x) {
        if (y == destination_y && x == destination_x) {
          break;
        }

        DrawPixel(x, y);

        error = error + delta_error;

        while (error >= 0.0 &&
               !(y == destination_y && x == destination_x)) {
          y = y + direction_y;
          error = error - 1.0;

          DrawPixel(x, y);
        }
      }

      return;
    }
  }

  DrawPixel(destination_x, destination_y);
}

void FlashPhotoApp::MouseDragged(int x, int y) {
  InterpolateToPoint(x, y);
}

void FlashPhotoApp::MouseMoved(int x, int y) {}

void FlashPhotoApp::LeftMouseDown(int x, int y) {
  DrawPixel(x, y);
}

void FlashPhotoApp::LeftMouseUp(int x, int y) {
  state_manager_.RegisterNewCanvasState(display_buffer_->GetAllPixels());
}

void FlashPhotoApp::InitializeBuffers(ColorData background_color,
                                      int width, int height) {
  display_buffer_ = new PixelBuffer(width, height, background_color);
  filter_manager_.set_pixel_buffer(display_buffer_);
}

void FlashPhotoApp::InitGlui(void) {
  // Select first tool (this activates the first radio button in glui)
  cur_tool_ = 0;

  GLUI_Panel *toolPanel = new GLUI_Panel(glui(), "Tool Type");
  {
    GLUI_RadioGroup *radio = new GLUI_RadioGroup(toolPanel, &cur_tool_,
                                                 UICtrl::UI_TOOLTYPE,
                                                 s_gluicallback);
    // Create interface buttons for different tools:
    new GLUI_RadioButton(radio, "Pen");
    new GLUI_RadioButton(radio, "Eraser");
    new GLUI_RadioButton(radio, "Spray Can");
    new GLUI_RadioButton(radio, "Calligraphy Pen");
    new GLUI_RadioButton(radio, "Highlighter");
    new GLUI_RadioButton(radio, "Stamp");
    new GLUI_RadioButton(radio, "Blur");
    new GLUI_RadioButton(radio, "Stamper");
  }

  GLUI_Panel *color_panel = new GLUI_Panel(glui(), "Tool Color");
  {
    cur_color_red_ = 0;
    glui_ctrl_hooks_.spinner_red  = new GLUI_Spinner(color_panel, "Red:",
                                                     &cur_color_red_,
                                                     UICtrl::UI_COLOR_R,
                                                     s_gluicallback);
    glui_ctrl_hooks_.spinner_red->set_float_limits(0, 1.0);

    cur_color_green_ = 0;
    glui_ctrl_hooks_.spinner_green = new GLUI_Spinner(color_panel, "Green:",
                                                      &cur_color_green_,
                                                      UICtrl::UI_COLOR_G,
                                                      s_gluicallback);
    glui_ctrl_hooks_.spinner_green->set_float_limits(0, 1.0);

    cur_color_blue_ = 0;
    glui_ctrl_hooks_.spinner_blue  = new GLUI_Spinner(color_panel, "Blue:",
                                                      &cur_color_blue_,
                                                      UICtrl::UI_COLOR_B,
                                                      s_gluicallback);
    glui_ctrl_hooks_.spinner_blue->set_float_limits(0, 1.0);

    new GLUI_Button(color_panel, "Red", UICtrl::UI_PRESET_RED,
                    s_gluicallback);
    new GLUI_Button(color_panel, "Orange", UICtrl::UI_PRESET_ORANGE,
                    s_gluicallback);
    new GLUI_Button(color_panel, "Yellow", UICtrl::UI_PRESET_YELLOW,
                    s_gluicallback);
    new GLUI_Button(color_panel, "Green", UICtrl::UI_PRESET_GREEN,
                    s_gluicallback);
    new GLUI_Button(color_panel, "Blue", UICtrl::UI_PRESET_BLUE,
                    s_gluicallback);
    new GLUI_Button(color_panel, "Purple", UICtrl::UI_PRESET_PURPLE,
                    s_gluicallback);
    new GLUI_Button(color_panel, "White", UICtrl::UI_PRESET_WHITE,
                    s_gluicallback);
    new GLUI_Button(color_panel, "Black", UICtrl::UI_PRESET_BLACK,
                    s_gluicallback);
  }

  /* Initialize state management (undo, redo, quit) */
  state_manager_.InitGlui(glui(), s_gluicallback);

  new GLUI_Button(const_cast<GLUI*>(glui()),
                "Quit", UICtrl::UI_QUIT,
                static_cast<GLUI_Update_CB>(exit));

  /* Initialize Filtering */
  filter_manager_.InitGlui(glui(), s_gluicallback);

  /* Initialize image I/O */
  io_manager_.InitGlui(glui(), s_gluicallback);
  return;
}

void FlashPhotoApp::GluiControl(int control_id) {
  switch (control_id) {
    case UICtrl::UI_PRESET_RED:
      cur_color_red_ = 1;
      cur_color_green_ = 0;
      cur_color_blue_ = 0;
      update_colors();
      break;
    case UICtrl::UI_PRESET_ORANGE:
      cur_color_red_ = 1;
      cur_color_green_ = 0.5;
      cur_color_blue_ = 0;
      update_colors();
      break;
    case UICtrl::UI_PRESET_YELLOW:
      cur_color_red_ = 1;
      cur_color_green_ = 1;
      cur_color_blue_ = 0;
      update_colors();
      break;
    case UICtrl::UI_PRESET_GREEN:
      cur_color_red_ = 0;
      cur_color_green_ = 1;
      cur_color_blue_ = 0;
      update_colors();
      break;
    case UICtrl::UI_PRESET_BLUE:
      cur_color_red_ = 0;
      cur_color_green_ = 0;
      cur_color_blue_ = 1;
      update_colors();
      break;
    case UICtrl::UI_PRESET_PURPLE:
      cur_color_red_ = 0.5;
      cur_color_green_ = 0;
      cur_color_blue_ = 1;
      update_colors();
      break;
    case UICtrl::UI_PRESET_WHITE:
      cur_color_red_ = 1;
      cur_color_green_ = 1;
      cur_color_blue_ = 1;
      update_colors();
      break;
    case UICtrl::UI_PRESET_BLACK:
      cur_color_red_ = 0;
      cur_color_green_ = 0;
      cur_color_blue_ = 0;
      update_colors();
      break;
    case UICtrl::UI_APPLY_BLUR:
      filter_manager_.ApplyBlur();
      state_manager_.RegisterNewCanvasState(display_buffer_->GetAllPixels());
      break;
    case UICtrl::UI_APPLY_SHARP:
      filter_manager_.ApplySharpen();
      state_manager_.RegisterNewCanvasState(display_buffer_->GetAllPixels());
      break;
    case UICtrl::UI_APPLY_MOTION_BLUR:
      filter_manager_.ApplyMotionBlur();
      state_manager_.RegisterNewCanvasState(display_buffer_->GetAllPixels());
      break;
    case UICtrl::UI_APPLY_EDGE:
      filter_manager_.ApplyEdgeDetect();
      state_manager_.RegisterNewCanvasState(display_buffer_->GetAllPixels());
      break;
    case UICtrl::UI_APPLY_THRESHOLD:
      filter_manager_.ApplyThreshold();
      state_manager_.RegisterNewCanvasState(display_buffer_->GetAllPixels());
      break;
    case UICtrl::UI_APPLY_DITHER:
      filter_manager_.ApplyThreshold();
      state_manager_.RegisterNewCanvasState(display_buffer_->GetAllPixels());
      break;
    case UICtrl::UI_APPLY_SATURATE:
      filter_manager_.ApplySaturate();
      state_manager_.RegisterNewCanvasState(display_buffer_->GetAllPixels());
      break;
    case UICtrl::UI_APPLY_CHANNEL:
      filter_manager_.ApplyChannel();
      state_manager_.RegisterNewCanvasState(display_buffer_->GetAllPixels());
      break;
    case UICtrl::UI_APPLY_QUANTIZE:
      filter_manager_.ApplyQuantize();
      state_manager_.RegisterNewCanvasState(display_buffer_->GetAllPixels());
      break;
    case UICtrl::UI_APPLY_SPECIAL_FILTER:
      filter_manager_.ApplySpecial();
      state_manager_.RegisterNewCanvasState(display_buffer_->GetAllPixels());
      break;
    case UICtrl::UI_FILE_BROWSER:
      io_manager_.set_image_file(io_manager_.file_browser()->get_file());
      break;
    case UICtrl::UI_LOAD_CANVAS_BUTTON:
      if (io_manager_.LoadImageToCanvas()) {
        set_pixel_buffer(io_manager_.GetLoadedPixelBuffer(), true);
      }
      break;
    case UICtrl::UI_LOAD_STAMP_BUTTON:
      io_manager_.LoadImageToStamp(toolbelt_->get_buffer_stamper());
      break;
    case UICtrl::UI_SAVE_CANVAS_BUTTON:
      io_manager_.SaveCanvasToFile(display_buffer_);
      // Reload the current directory:
      io_manager_.file_browser()->fbreaddir(".");
      break;
    case UICtrl::UI_FILE_NAME:
      io_manager_.set_image_file(io_manager_.file_name());
      break;
    case UICtrl::UI_UNDO:
      display_buffer_->SetAllPixels(state_manager_.UndoOperation());
      break;
    case UICtrl::UI_REDO:
      display_buffer_->SetAllPixels(state_manager_.RedoOperation());
      break;
    default:
      break;
  }

  // Forces canvas to update changes made in this function
  glui()->post_update_main_gfx();

  // Update toolbelt
  toolbelt_->set_active_color(
      ColorData(cur_color_red_, cur_color_green_, cur_color_blue_));
  toolbelt_->set_active_tool(cur_tool_);
}

/*******************************************************************************
 * Member Functions For Managing GLUI Interface
 ******************************************************************************/
void FlashPhotoApp::update_colors(void) {
  glui_ctrl_hooks_.spinner_blue->set_float_val(cur_color_blue_);
  glui_ctrl_hooks_.spinner_green->set_float_val(cur_color_green_);
  glui_ctrl_hooks_.spinner_red->set_float_val(cur_color_red_);
}

void FlashPhotoApp::InitGraphics(void) {
  // Initialize OpenGL for 2D graphics as used in the BrushWork app
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluOrtho2D(0, width(), 0, height());
  glViewport(0, 0, width(), height());
}

void FlashPhotoApp::InitializeToolBelt(
    PixelBuffer *display_buffer,
    int active_tool,
    ColorData active_color) {
  toolbelt_ = new ToolBelt(display_buffer, active_tool, active_color);
}

}  /* namespace image_tools */
