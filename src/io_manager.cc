/*******************************************************************************
 * Name            : io_manager.cc
 * Project         : FlashPhoto
 * Module          : io_manager
 * Description     : Implementation of IOManager class
 * Copyright       : 2016 CSCI3081W TAs. All rights reserved.
 * Creation Date   : Wed Sep 21 20:47:05 2016
 * Original Author : jharwell, Erik Husby, J. Jason Mitchell, William Meusing
 *
 ******************************************************************************/

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "include/io_manager.h"
#include <setjmp.h>
#include <iostream>
#include "include/ui_ctrl.h"
#include "include/state_manager.h"
#include "include/filter_manager.h"
#include "../ext/libpng-1.6.16/png.h"
#include "../ext/jpeg-9a/jpeglib.h"
#include "../ext/jpeg-9a/jerror.h"
#include "include/tool.h"
#include "include/stamper.h"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
namespace image_tools {

/*******************************************************************************
 * Constructors/Destructor
 ******************************************************************************/
IOManager::IOManager(void) :
    file_browser_(nullptr),
    load_canvas_btn_(nullptr),
    load_stamp_btn_(nullptr),
    save_canvas_btn_(nullptr),
    current_file_label_(nullptr),
    file_name_box_(nullptr),
    save_file_label_(nullptr),
    file_name_() {}

/*******************************************************************************
 * Member Functions
 ******************************************************************************/
void IOManager::InitGlui(const GLUI *const glui,
                         void (*s_gluicallback)(int)) {
  new GLUI_Column(const_cast<GLUI*>(glui), true);
  GLUI_Panel *image_panel = new GLUI_Panel(const_cast<GLUI*>(glui),
                                           "Image I/O");
  file_browser_ = new GLUI_FileBrowser(image_panel,
                                      "Choose Image",
                                      false,
                                      UICtrl::UI_FILE_BROWSER,
                                      s_gluicallback);

  file_browser_->set_h(400);

  file_name_box_ = new GLUI_EditText(image_panel ,
                                    "Image:",
                                    file_name_,
                                    UICtrl::UI_FILE_NAME,
                                    s_gluicallback);
  file_name_box_->set_w(200);

  new GLUI_Separator(image_panel);

  current_file_label_ = new GLUI_StaticText(image_panel,
                                           "Will load image: none");
  load_canvas_btn_ = new GLUI_Button(image_panel,
                                    "Load Canvas",
                                    UICtrl::UI_LOAD_CANVAS_BUTTON,
                                    s_gluicallback);
  load_stamp_btn_ = new GLUI_Button(image_panel,
                                   "Load Stamp",
                                   UICtrl::UI_LOAD_STAMP_BUTTON,
                                   s_gluicallback);

  new GLUI_Separator(image_panel);

  save_file_label_ = new GLUI_StaticText(image_panel,
                                        "Will save image: none");

  save_canvas_btn_ = new GLUI_Button(image_panel,
                                    "Save Canvas",
                                    UICtrl::UI_SAVE_CANVAS_BUTTON,
                                    s_gluicallback);

  load_canvas_toggle(false);
  load_stamp_toggle(false);
  save_canvas_toggle(false);
}


bool IOManager::is_valid_image_file(const std::string & name) {
  FILE *f;
  bool is_valid = false;
  if (is_valid_image_file_name(name)) {
    if ((f = fopen(name.c_str(), "r"))) {
      is_valid = true;
      fclose(f);
    }
  }
  return is_valid;
}

void IOManager::set_image_file(const std::string & file_name) {
  // If a directory was selected
  // instead of a file, use the
  // latest file typed or selected.
  std::string image_file = file_name;
  if (!is_valid_image_file_name(image_file)) {
    image_file = file_name_;
  }

  // TOGGLE SAVE FEATURE
  // If no file is selected or typed,
  // don't allow file to be saved. If
  // there is a file name, then allow
  // file to be saved to that name.

  if (!is_valid_image_file_name(image_file)) {
    save_file_label_->set_text("Will save image: none");
    save_canvas_toggle(false);
  } else {
    save_file_label_->set_text((std::string("Will save image: ") +
                                image_file).c_str());
    save_canvas_toggle(true);
  }

  // TOGGLE LOAD FEATURE

  // If the file specified cannot be opened,
  // then disable stamp and canvas loading.
  if (is_valid_image_file(image_file)) {
    load_stamp_toggle(true);
    load_canvas_toggle(true);

    current_file_label_->set_text((std::string("Will load: ") +
                                   image_file).c_str());
    file_name_box_->set_text(image_file);
  } else {
    load_stamp_toggle(false);
    load_canvas_toggle(false);
    current_file_label_->set_text("Will load: none");
  }
}

bool IOManager::LoadImageToCanvas(void) {
  std::cout << "Load Canvas has been clicked for file " <<
      file_name_ << std::endl;
  ValidatedPixelBuffer loaded_image = LoadImageDataFromFile(true);

  if (loaded_pixel_buffer_) {
    delete loaded_pixel_buffer_;
  }

  if (loaded_image.valid_image) {
    loaded_pixel_buffer_ = loaded_image.pixel_buffer;
  } else {
    std::cout << "Image was not valid." << std::endl;
  }

  return loaded_image.valid_image;
}

void IOManager::LoadImageToStamp(Tool* stamper) {
  std::cout << "Load Stamp has been clicked for file " <<
      file_name_ << std::endl;
  ValidatedPixelBuffer loaded_image = LoadImageDataFromFile(false);

  if (loaded_image.valid_image) {
    reinterpret_cast<Stamper *>(stamper)->
      set_buffer_stamp_mask(loaded_image.pixel_buffer);
  } else {
    std::cout << "Image was not valid." << std::endl;
  }
}

void IOManager::SaveCanvasToFile(PixelBuffer* pixel_buffer) {
  std::cout << "Save Canvas been clicked for file " <<
      file_name_ << std::endl;

  if (has_suffix(file_name_ , ".png")) {
    SaveCanvasToPNGFile(pixel_buffer);
  } else if (has_suffix(file_name_, ".jpg") ||
             has_suffix(file_name_, ".jpeg")) {
    SaveCanvasToJPEGFile(pixel_buffer);
  } else {
    std::cout << "Could not determine image type for save operation." <<
              std::endl;
  }
}

PixelBuffer* IOManager::GetLoadedPixelBuffer(void) {
  return loaded_pixel_buffer_;
}

IOManager::ValidatedPixelBuffer IOManager::LoadImageDataFromFile(
    bool composite_color_values) {
  ValidatedPixelBuffer loaded_image;
  if (has_suffix(file_name_ , ".png")) {
    loaded_image = LoadImageDataFromPNGFile(composite_color_values);
  } else if (has_suffix(file_name_, ".jpg") ||
             has_suffix(file_name_, ".jpeg")) {
    loaded_image = LoadImageDataFromJPEGFile();
  } else {
    std::cout << "Could not determine image type for load operation." <<
              std::endl;
  }

  return loaded_image;
}

IOManager::ValidatedPixelBuffer IOManager::LoadImageDataFromJPEGFile(void) {
  std::cout << "Load JPEG." << std::endl;
  ValidatedPixelBuffer loaded_image;

  unsigned char *jpeg_image = nullptr;

  struct jpeg_decompress_struct info;
  struct jpeg_error_mgr jpeg_error;
  JSAMPROW image_row[1];

  info.err = jpeg_std_error(&jpeg_error);

  FILE *fp = fopen(file_name_.c_str(), "rb");

  if (!fp) {
    std::cout << "ERROR: Could not open file to load JPEG." << std::endl;
    return loaded_image;
  }

  jpeg_create_decompress(&info);
  jpeg_stdio_src(&info, fp);

  (void) jpeg_read_header(&info, TRUE);
  (void) jpeg_start_decompress(&info);

  jpeg_image = (unsigned char*)malloc(info.output_width
                                      * info.output_height
                                      * info.num_components);
  image_row[0] = (unsigned char *)malloc(info.output_width*info.num_components);

  int jpeg_index = 0;

  while (info.output_scanline < info.output_height) {
    (void) jpeg_read_scanlines(&info, image_row, 1);
    for (int i = 0; i < info.image_width * info.num_components; i++)
      jpeg_image[jpeg_index++] = image_row[0][i];
  }

  (void) jpeg_finish_decompress(&info);

  loaded_image.valid_image = true;
  loaded_image.pixel_buffer = new PixelBuffer(
    info.image_width,
    info.image_height,
    ColorData(1, 1, static_cast<float>(0.95)));

  for (int x = 0; x < info.image_width; x++) {
    for (int y = 0; y < info.image_height; y++) {
      float red = static_cast<float>(
        jpeg_image[(y*info.image_width*3)+(x*3)+0]/255.);
      float green = static_cast<float>(
        jpeg_image[(y*info.image_width*3)+(x*3)+1]/255.);
      float blue = static_cast<float>(
        jpeg_image[(y*info.image_width*3)+(x*3)+2]/255.);
      loaded_image.pixel_buffer->set_pixel(x, y, ColorData(red, green, blue));
    }
  }

  jpeg_destroy_decompress(&info);
  fclose(fp);

  return loaded_image;
}

IOManager::ValidatedPixelBuffer IOManager::LoadImageDataFromPNGFile(
    bool composite_color_values) {
  ValidatedPixelBuffer loaded_image;
  int width, height;
  png_byte color_type;
  png_byte bit_depth;
  png_bytep* image_rows;

  FILE *fp = fopen(file_name_.c_str(), "rb");

  if (!fp) {
    std::cout << "ERROR: Could not open file to load PNG." << std::endl;
    return loaded_image;
  }

  png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING,
                                           NULL, NULL, NULL);
  if (!png) {
    std::cout << "ERROR: Could not create read struct for PNG." << std::endl;
    fclose(fp);
    return loaded_image;
  }

  png_infop info = png_create_info_struct(png);
  if (!info) {
    std::cout << "ERROR: Could not create info struct for PNG." << std::endl;
    fclose(fp);
    png = nullptr;
    return loaded_image;
  }

  if (setjmp(png_jmpbuf(png)))  {
    std::cout << "ERROR: Could not initialize read buffer for PNG."
              << std::endl;
    fclose(fp);
    png = nullptr;
    info = nullptr;
    return loaded_image;
  }
  png_init_io(png, fp);

  png_read_info(png, info);

  width      = png_get_image_width(png, info);
  height     = png_get_image_height(png, info);
  color_type = png_get_color_type(png, info);
  bit_depth  = png_get_bit_depth(png, info);

  if (bit_depth == 16)
    png_set_strip_16(png);

  if (color_type == PNG_COLOR_TYPE_PALETTE)
    png_set_palette_to_rgb(png);

  if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
    png_set_expand_gray_1_2_4_to_8(png);

  if (color_type == PNG_COLOR_TYPE_GRAY ||
    color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
      png_set_gray_to_rgb(png);
  }

  if (png_get_valid(png, info, PNG_INFO_tRNS))
    png_set_tRNS_to_alpha(png);

  // Fill in the alpha values if the color type does not contain an alpha value.
  if (color_type == PNG_COLOR_TYPE_RGB || color_type == PNG_COLOR_TYPE_GRAY ||
    color_type == PNG_COLOR_TYPE_PALETTE) {
    png_set_filler(png, 0xFF, PNG_FILLER_AFTER);
  }

  png_read_update_info(png, info);

  image_rows = reinterpret_cast<png_bytep*>(malloc(sizeof(png_bytep) * height));
  for (int y = 0; y < height; y++) {
    image_rows[y] = reinterpret_cast<png_byte*>(
      malloc(png_get_rowbytes(png, info)));
  }

  png_read_image(png, image_rows);

  fclose(fp);
  png = nullptr;
  info = nullptr;

  ColorData background_color = ColorData(1, 1, static_cast<float>(0.95));
  loaded_image.valid_image = true;
  loaded_image.pixel_buffer = new PixelBuffer(width, height, background_color);

  for (int y = 0; y < height; y++) {
    png_bytep row = image_rows[y];
    for (int x = 0; x < width; x++) {
      // Get the RGBA values from the PNG.
      float red = static_cast<float>(image_rows[y][(x * 4) + 0 ] / 255.);
      float green = static_cast<float>(image_rows[y][(x * 4) + 1] / 255.);
      float blue = static_cast<float>(image_rows[y][(x * 4) + 2] / 255.);
      float alpha = static_cast<float>(image_rows[y][(x * 4) + 3] / 255.);

      if (composite_color_values) {
        // Composite the values with the background.
        red = (red * alpha) + (background_color.clamped_color().red()
                               * (1 - alpha));
        green = (green * alpha) + (background_color.clamped_color().red()
                                   * (1 - alpha));
        blue = (blue * alpha) + (background_color.clamped_color().blue()
                                 * (1 - alpha));
        alpha = alpha + (background_color.clamped_color().alpha()
                         * (1 - alpha));
      }

      // Apply new color to the pixel buffer.
      loaded_image.pixel_buffer->set_pixel(
        x, y, ColorData(red, green, blue, alpha));
    }
  }

  for (int y = 0; y < height; y++) {
    free(image_rows[y]);
  }
  free(image_rows);

  std::cout << "Loaded PNG." << std::endl;
  return loaded_image;
}

void IOManager::SaveCanvasToPNGFile(PixelBuffer* pixel_buffer) {
  int input_components = 4;
  png_bytep* image_rows = reinterpret_cast<png_bytep*>(
    malloc(sizeof(png_bytep) * pixel_buffer->height()));

  for (int y = 0; y < pixel_buffer->height(); y++) {
    image_rows[y] = reinterpret_cast<png_byte*>(
      malloc(pixel_buffer->width() * input_components * sizeof(png_byte)));
  }

  for (int y = 0; y < pixel_buffer->height(); y++) {
    for (int x = 0; x < pixel_buffer->width(); x++) {
      ColorData color_data = pixel_buffer->get_pixel(x, y);
      color_data = color_data.clamped_color();

      int pixel_index = (x * input_components);
      image_rows[y][pixel_index] = (png_byte)(color_data.red() * 255.);
      image_rows[y][pixel_index + 1] = (png_byte)(color_data.green() * 255.);
      image_rows[y][pixel_index + 2] = (png_byte)(color_data.blue() * 255.);
      image_rows[y][pixel_index + 3] = (png_byte)(color_data.alpha() * 255.);
    }
  }

  FILE *fp = fopen(file_name_.c_str(), "wb");

  if (!fp) {
    std::cout << "ERROR: Could not open file to write PNG." << std::endl;
    return;
  }

  png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING,
                                            NULL, NULL, NULL);
  if (!png) {
    std::cout << "ERROR: Could not create write struct for PNG." << std::endl;
    return;
  }

  png_infop info = png_create_info_struct(png);
  if (!info) {
    std::cout << "ERROR: Could not create info struct to write PNG."
              << std::endl;
    return;
  }

  if (setjmp(png_jmpbuf(png))) {
    std::cout << "ERROR: Could not initialize write buffer for PNG."
              << std::endl;
    return;
  }

  png_init_io(png, fp);

  png_set_IHDR(png,
               info,
               pixel_buffer->width(),
               pixel_buffer->height(),
               8,
               PNG_COLOR_TYPE_RGBA,
               PNG_INTERLACE_NONE,
               PNG_COMPRESSION_TYPE_DEFAULT,
               PNG_FILTER_TYPE_DEFAULT);

  png_write_info(png, info);

  png_write_image(png, image_rows);
  png_write_end(png, nullptr);

  fclose(fp);

  png_destroy_write_struct(&png, &info);

  for (int y = 0; y < pixel_buffer->height(); y++) {
    free(image_rows[y]);
  }
  free(image_rows);

  std::cout << "Saved PNG." << std::endl;
}

void IOManager::SaveCanvasToJPEGFile(PixelBuffer* pixel_buffer) {
  // We have 3 image components (RGB), JPEGs do not support transparency/alpha.
  int input_components = 3;

  unsigned char *jpeg_image = nullptr;

  struct jpeg_compress_struct info;
  struct jpeg_error_mgr jpeg_error;
  JSAMPROW image_rows[1];

  info.err = jpeg_std_error(&jpeg_error);
  jpeg_create_compress(&info);

  FILE *fp = fopen(file_name_.c_str(), "wb");

  if (!fp) {
    std::cout << "ERROR: Could not open file to write JPEG." << std::endl;
    return;
  }

  jpeg_stdio_dest(&info, fp);

  info.image_height = pixel_buffer->height();
  info.image_width = pixel_buffer->width();
  info.input_components = input_components;
  info.in_color_space = JCS_RGB;

  jpeg_set_defaults(&info);

  jpeg_start_compress(&info, TRUE);

  jpeg_image = (unsigned char *)malloc(
    info.image_width * info.image_height * input_components);

  for (int y = 0; y < info.image_height; y++) {
    for (int x = 0; x < info.image_width; x++) {
      ColorData color_data = pixel_buffer->get_pixel(x, y);
      color_data = color_data.clamped_color();

      int pixel_index = (y * info.image_width * input_components)
                        + (x * input_components);
      jpeg_image[pixel_index] = (color_data.red() * 255.);
      jpeg_image[pixel_index + 1] = (color_data.green() * 255.);
      jpeg_image[pixel_index + 2] = (color_data.blue() * 255.);
    }
  }

  while (info.next_scanline < info.image_height) {
    image_rows[0] = &jpeg_image[
      info.next_scanline * info.image_width * input_components];
    (void) jpeg_write_scanlines(&info, image_rows, 1);
  }

  jpeg_finish_compress(&info);
  fclose(fp);
  jpeg_destroy_compress(&info);

  std::cout << "Saved JPEG." << std::endl;
}

}  /* namespace image_tools */
