/*******************************************************************************
 * Name            : io_manager.h
 * Project         : FlashPhoto
 * Module          : io_manager
 * Description     : Header for IoManager class
 * Copyright       : 2016 CSCI3081W TAs. All rights reserved.
 * Creation Date   : Wed Sep 21 20:40:20 2016
 * Original Author : jharwell, Erik Husby, J. Jason Mitchell, William Meusing
 *
 ******************************************************************************/

#ifndef PROJECT_ITERATION2_SRC_INCLUDE_IO_MANAGER_H_
#define PROJECT_ITERATION2_SRC_INCLUDE_IO_MANAGER_H_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <string>
#include "GL/glui.h"
#include "./ui_ctrl.h"
#include "./filter_kernel.h"
#include "./filter_manager.h"
#include "./pixel_buffer.h"
#include "./color_data.h"
#include "include/tool.h"
#include "include/stamper.h"


/*******************************************************************************
 * Namespaces
 ******************************************************************************/
namespace image_tools {

/*******************************************************************************
 * Class Definitions
 ******************************************************************************/
/**
 * @brief The manager for I/O operations in FlashPhoto. This includes
 * initializing GLUI control elements related to image loading/saving, as well
 * as handling the load/save operations themselves. The load operation includes
 * validating the image type and filesystem permissions before the load will
 * proceed. It also handles loading an image from a file to the stamp.
 */
class IOManager {
 public:
  IOManager();
  ~IOManager() {}

  /**
   * @brief Initialize GLUI control elements for IO management
   *
   * @param[in] glui GLUI handle
   * @param[in] s_gluicallback Callback to install
   */
  void InitGlui(const GLUI *const glui,
                void (*s_gluicallback)(int));

  /**
   * @brief Set the image file. If the file is valid, enable loading/saving
   *
   * @param[in] filepath Pathname of the file
   */
  void set_image_file(const std::string & filepath);

  /**
   * @brief Get the current image file name
   *
   * @return The current image file name
   */
  const std::string& file_name(void) { return file_name_;}

  /**
   * @brief Get a handle on the GLUI file browser
   *
   * @return The handle
   */
  GLUI_FileBrowser* file_browser(void) { return file_browser_;}

  /**
   * @brief Load the selected image file to the canvas
   *
   */
  bool LoadImageToCanvas(void);

  /**
   * @brief Load the selected image file to the stamp
   *
   */
  void LoadImageToStamp(Tool* stamper);

  /**
   * @brief Save the current state of the canvas to a file
   *
   */
  void SaveCanvasToFile(PixelBuffer* pixel_buffer);

  PixelBuffer* GetLoadedPixelBuffer(void);

 private:
  /* Copy/move assignment/construction disallowed */
  IOManager(const IOManager &rhs) = delete;
  IOManager& operator=(const IOManager &rhs) = delete;

  void save_canvas_toggle(bool enabled) {
    UICtrl::button_toggle(save_canvas_btn_, enabled);
  }

  void load_stamp_toggle(bool enabled) {
    UICtrl::button_toggle(load_stamp_btn_, enabled);
  }

  void load_canvas_toggle(bool enabled) {
    UICtrl::button_toggle(load_canvas_btn_, enabled);
  }
  /**
   * @brief Determine if a file name contains a given suffix
   *
   * @param[in] str The name of the file
   * @param[in] suffix The suffix to be checked
   *
   * @return TRUE if yes, FALSE otherwise
   */
  bool has_suffix(const std::string & str, const std::string & suffix) {
    return str.find(suffix, str.length()-suffix.length()) != std::string::npos;
  }

  /**
   * @brief Determine if a file has a valid name for an image file
   *
   * @param[in] name The name of the file
   * 
   * @return TRUE if the file has a valid name for an image, FALSE otherwise
   */
  bool is_valid_image_file_name(const std::string & name) {
    return (has_suffix(name, ".png") || has_suffix(name, ".jpg")
           || has_suffix(name, ".jpeg"));
  }

  struct ValidatedPixelBuffer {
    bool valid_image = false;
    PixelBuffer* pixel_buffer = nullptr;
  };

  PixelBuffer* loaded_pixel_buffer_ = nullptr;

  ValidatedPixelBuffer LoadImageDataFromFile(bool composite_color_values);
  ValidatedPixelBuffer LoadImageDataFromJPEGFile(void);
  ValidatedPixelBuffer LoadImageDataFromPNGFile(bool composite_color_values);

  void SaveCanvasToJPEGFile(PixelBuffer* pixel_buffer);
  void SaveCanvasToPNGFile(PixelBuffer* pixel_buffer);

  /**
   * @brief Determine if the name of a file corresponds to an image
   *
   * The file must exist, have a proper suffix, and be openable
   *(i.e. the user
   * has permission to open it).
   *
   * @param[in] name The name of the file to be checked
   * 
   * @return TRUE if yes, FALSE otherwise
   */
  bool is_valid_image_file(const std::string &name);

  /* data members */
  GLUI_FileBrowser *file_browser_;
  GLUI_Button *load_canvas_btn_;
  GLUI_Button *load_stamp_btn_;
  GLUI_Button *save_canvas_btn_;
  GLUI_StaticText *current_file_label_;
  GLUI_EditText *file_name_box_;
  GLUI_StaticText *save_file_label_;
  std::string file_name_;
};

}  /* namespace image_tools */

#endif  /* PROJECT_ITERATION2_SRC_INCLUDE_IO_MANAGER_H_ */
