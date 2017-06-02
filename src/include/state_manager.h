/*******************************************************************************
 * Name            : state_manager.h
 * Project         : FlashPhoto
 * Module          : state_manager
 * Description     : Header for IoHandler class
 * Copyright       : 2016 CSCI3081W TAs. All rights reserved.
 * Creation Date   : Wed Sep 21 20:40:20 2016
 * Original Author : jharwell, Erik Husby, J. Jason Mitchell, William Meusing
 *
 ******************************************************************************/

#ifndef PROJECT_ITERATION2_SRC_INCLUDE_STATE_MANAGER_H_
#define PROJECT_ITERATION2_SRC_INCLUDE_STATE_MANAGER_H_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <string>
#include <stack>
#include "GL/glui.h"
#include "./ui_ctrl.h"
#include "./state_manager.h"
#include "./color_data.h"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
namespace image_tools {

/*******************************************************************************
 * Class Definitions
 ******************************************************************************/
/**
 * @brief The manager for simulation state in FlashPhoto.
 *
 * Simulation state is defined as the sequence of operations that comprises the
 * current canvas. An operation is defined as what happens during 1 click/drag
 * operation, or when 1 button on the control panel that affects the canvas is
 * clicked.
 *
 * A sequence of undos followed by some edits, followed by more undos will
 * FIRST undo the new edits, until you get back to the state before you made the
 * edits. You will not be able to go back any further.
 */
class StateManager {
 public:
  StateManager();
  ~StateManager() {}

  void InitGlui(const GLUI *const glui,
                void (*s_gluicallback)(int));

  /**
   * @brief Undoes the last operation applied to the canvas (not permanently; it
   * can still be re-done later, unless another non-redo/undo operation is made)
   *
   * @return The array of pixels that was popped off of the undo stack
   */
  ColorData* UndoOperation(void);

  /**
   * @brief Re-does the last un-done operation applied to the canvas (not
   * permanently; it can be undone again later, unless another non-redo/undo operation is made)
   *
   * @return The array of pixels that was popped off of the redo stack
   */
  ColorData* RedoOperation(void);

  /**
   * @brief Adds the new pixel buffer state to the undo stack.
   *
   * @param[in] pixels The array of pixels to be pushed onto the stack
   */
  void RegisterNewCanvasState(ColorData* pixels);

  /**
   * @brief Clears the undo and redo stacks
   */
  void ClearUndoAndRedoStacks(void);

 private:
  void redo_toggle(bool select) {
    UICtrl::button_toggle(redo_btn_, select);
  }

  void undo_toggle(bool select) {
    UICtrl::button_toggle(undo_btn_, select);
  }

  void ToggleStateButtons(void);

  void ClearRedoStack(void);
  void ClearUndoStack(void);

  /* Copy/move assignment/construction disallowed */
  StateManager(const StateManager &rhs) = delete;
  StateManager& operator=(const StateManager &rhs) = delete;

  /* data members */
  GLUI_Button *undo_btn_;
  GLUI_Button *redo_btn_;

  std::stack<ColorData *> undo_stack_;
  std::stack<ColorData *> redo_stack_;
};

}  /* namespace image_tools */

#endif  /* PROJECT_ITERATION2_SRC_INCLUDE_STATE_MANAGER_H_ */
