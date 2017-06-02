### Programming Tutorial: Adding a New Pencil Tool to FlashPhoto

1. Open src/lib/libimgtools/src/toolbelt.cc and add the following line to the ToolBelt::add_flashphoto_tools() method:

       ```
       add_tool(reinterpret_cast<Tool *>(new Brush(this, 1, 1, 1.0)));
       ```

2. Open src/app/FlashPhoto/src/flashphoto_app.cc and locate the definition of the method that initializes the UI for the FlashPhoto app (FlashPhoto::InitGlui()). Add the following line to the bottom of the code block with a header comment that reads "// Create interface buttons for different tools":
       ```
       new GLUI_RadioButton(radio, "Pencil");
       ```
