### Programming Tutorial: Adding a New Invert Filter to FlashPhoto

1. Open src/lib/libimgtools/src/include/ui_ctrl.h and add a new entry named UI_APPLY_INVERT to the 'Type' enumeration.

2. Open src/lib/libimgtools/src/filter_manager.cc and insert the following definition for the Invert filter panel and the Apply button it will contain after those for the Special filter panel.

    ```
    GLUI_Panel *invert_panel = new GLUI_Panel(filter_panel, "Invert");
    {
      new GLUI_Button(invert_panel, "Apply",
                      UICtrl::UI_APPLY_INVERT,
                      s_gluicallback);
    }
    ```

3. Add a function in FilterManager that will determine the new color of a pixel on the canvas when this filter is applied to it. Use the following definition:

    ```
    ColorData FilterManager::Invert(int x, int y) {
      ColorData current_color = pixel_buffer_->get_pixel(x, y);
      float new_red = 1 - current_color.red();
      float new_green = 1 - current_color.green();
      float new_blue = 1 - current_color.blue();
      float alpha = current_color.alpha();
      return ColorData(new_red, new_green, new_blue, alpha);
    }
    ```

    Make sure to also declare this function in src/lib/libimgtools/src/include/filter_manager.h.
    
4. Add a second function in FilterManager that will apply this non-convolution filter to the canvas. Use the following definition:

    ```
    void FilterManager::ApplyInvert(void) {
      std::cout << "Apply has been clicked for Invert" << std::endl;
      ApplyNonConvolutionFilter(&FilterManager::Invert);
    }
    ```

    Make sure to also declare this function in src/lib/libimgtools/src/include/filter_manager.h.
 
4. In order to register the Apply button in the new Invert filter panel to its proper function callback, open up src/app/FlashPhoto/src/flashphoto_app.cc and add the following case to the switch statement on control_id within the GluiControl method:

    ```
    case UICtrl::UI_APPLY_INVERT:
      filter_manager_.ApplyInvert();
      state_manager_.RegisterNewCanvasState(display_buffer_->GetAllPixels());
      break;
    ```
