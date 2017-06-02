/*******************************************************************************
 * Name            : state_manager.cc
 * Project         : FlashPhoto
 * Module          : state_manager
 * Description     : Implementation of StateManager class
 * Copyright       : 2016 CSCI3081W TAs. All rights reserved.
 * Creation Date   : Sun Oct 2 20:47:05 2016
 * Original Author : jharwell, Erik Husby, J. Jason Mitchell, William Meusing
 *
 ******************************************************************************/

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "include/state_manager.h"
#include <iostream>
#include "include/ui_ctrl.h"
#include "include/filter_manager.h"
#include "include/io_manager.h"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
namespace image_tools {

/*******************************************************************************
 * Constructors/Destructor
 ******************************************************************************/
StateManager::StateManager(void) :
    undo_btn_(nullptr),
    redo_btn_(nullptr) {}

/*******************************************************************************
 * Member Functions
 ******************************************************************************/
void StateManager::InitGlui(const GLUI *const glui,
                            void (*s_gluicallback)(int)) {
  undo_btn_ = new GLUI_Button(const_cast<GLUI*>(glui), "Undo", UICtrl::UI_UNDO,
                              s_gluicallback);
  undo_toggle(false);

  redo_btn_  = new GLUI_Button(const_cast<GLUI*>(glui), "Redo", UICtrl::UI_REDO,
                               s_gluicallback);
  redo_toggle(false);
}

ColorData* StateManager::UndoOperation(void) {
  std::cout << "Undoing..." << std::endl;
  redo_stack_.push(undo_stack_.top());
  undo_stack_.pop();

  ToggleStateButtons();

  return undo_stack_.top();
}

ColorData* StateManager::RedoOperation(void) {
  std::cout << "Redoing..." << std::endl;

  undo_stack_.push(redo_stack_.top());
  redo_stack_.pop();

  ToggleStateButtons();

  return undo_stack_.top();
}

void StateManager::RegisterNewCanvasState(ColorData* pixels) {
  undo_stack_.push(pixels);
  ClearRedoStack();
  ToggleStateButtons();
}

void StateManager::ClearUndoAndRedoStacks(void) {
  ClearRedoStack();
  ClearUndoStack();
}

void StateManager::ClearRedoStack(void) {
  while (!redo_stack_.empty()) {
    delete redo_stack_.top();
    redo_stack_.pop();
  }
}

void StateManager::ClearUndoStack(void) {
  while (!undo_stack_.empty()) {
    delete undo_stack_.top();
    undo_stack_.pop();
  }
}

void StateManager::ToggleStateButtons(void) {
  redo_toggle(redo_stack_.size() > 0);
  undo_toggle(undo_stack_.size() > 1);
}

}  /* namespace image_tools */
