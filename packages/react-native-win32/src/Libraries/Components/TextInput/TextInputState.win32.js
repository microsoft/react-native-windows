/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @format
 * @flow strict-local
 */

// This class is responsible for coordinating the "focused" state for
// TextInputs. All calls relating to the keyboard should be funneled
// through here.

'use strict';

const Platform = require('../../Utilities/Platform');
const UIManager = require('../../ReactNative/UIManager');

let currentlyFocusedID: ?number = null;
const inputs = new Set();

/**
 * Returns the ID of the currently focused text field, if one exists
 * If no text field is focused it returns null
 */
function currentlyFocusedField(): ?number {
  return currentlyFocusedID;
}

function focusField(textFieldID: ?number): void {
  if (currentlyFocusedID !== textFieldID && textFieldID != null) {
    currentlyFocusedID = textFieldID;
  }
}

function blurField(textFieldID: ?number) {
  if (currentlyFocusedID === textFieldID && textFieldID != null) {
    currentlyFocusedID = null;
  }
}

/**
 * @param {number} TextInputID id of the text field to focus
 * Focuses the specified text field
 * noop if the text field was already focused
 */
function focusTextInput(textFieldID: ?number) {
  if (currentlyFocusedID !== textFieldID && textFieldID != null) {
    focusField(textFieldID);
    // [Windows
    if (Platform.OS === 'ios' || Platform.OS === 'win32') {
      // Windows]
      UIManager.focus(textFieldID);
    } else if (Platform.OS === 'android') {
      UIManager.dispatchViewManagerCommand(
        textFieldID,
        UIManager.getViewManagerConfig('AndroidTextInput').Commands
          .focusTextInput,
        null,
      );
    }
  }
}

/**
 * @param {number} textFieldID id of the text field to unfocus
 * Unfocuses the specified text field
 * noop if it wasn't focused
 */
function blurTextInput(textFieldID: ?number) {
  if (currentlyFocusedID === textFieldID && textFieldID != null) {
    blurField(textFieldID);
    // [Windows
    if (Platform.OS === 'ios' || Platform.OS === 'win32') {
      // Windows]
      UIManager.blur(textFieldID);
    } else if (Platform.OS === 'android') {
      UIManager.dispatchViewManagerCommand(
        textFieldID,
        UIManager.getViewManagerConfig('AndroidTextInput').Commands
          .blurTextInput,
        null,
      );
    }
  }
}

// [Windows/Android?
/**
 * @param {number} TextInputID id of the text field that has received focus
 * Should be called after the view has received focus and fired the onFocus event
 * noop if the focused text field is same
 */
function setFocusedTextInput(textFieldID: ?number) {
  if (currentlyFocusedID !== textFieldID && textFieldID !== null) {
    currentlyFocusedID = textFieldID;
  }
}

/**
 * @param {number} TextInputID id of the text field whose focus has to be cleared
 * Should be called after the view has cleared focus and fired the onFocus event
 * noop if the focused text field is not same
 */
function clearFocusedTextInput(textFieldID: ?number) {
  if (currentlyFocusedID === textFieldID && textFieldID !== null) {
    currentlyFocusedID = null;
  }
}
// Windows/Android?]

function registerInput(textFieldID: number) {
  inputs.add(textFieldID);
}

function unregisterInput(textFieldID: number) {
  inputs.delete(textFieldID);
}

function isTextInput(textFieldID: number): boolean {
  return inputs.has(textFieldID);
}

module.exports = {
  currentlyFocusedField,
  focusField,
  blurField,
  setFocusedTextInput, // TODO(android ISS)
  clearFocusedTextInput, // TODO(android ISS)
  focusTextInput,
  blurTextInput,
  registerInput,
  unregisterInput,
  isTextInput,
};
