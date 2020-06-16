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
    // [Windows Replace Android/iOS check
    focusField(textFieldID);
    UIManager.focus(textFieldID);
    // Windows]
  }
}

/**
 * @param {number} textFieldID id of the text field to unfocus
 * Unfocuses the specified text field
 * noop if it wasn't focused
 */
function blurTextInput(textFieldID: ?number) {
  if (currentlyFocusedID === textFieldID && textFieldID != null) {
    // [Windows Replace Android/iOS check
    blurField(textFieldID);
    UIManager.blur(textFieldID);
    // Windows]
  }
}

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
  focusTextInput,
  blurTextInput,
  registerInput,
  unregisterInput,
  isTextInput,
};
