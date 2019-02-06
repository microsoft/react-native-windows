/**
 *
 * @providesModule TextInputState
 * @flow
 *
 * This class is responsible for coordinating the "focused"
 * state for TextInputs. All calls relating to the keyboard
 * should be funneled through here
 */
'use strict';

const UIManager = require('UIManager');

const TextInputState = {
  /**
  * Internal state
  */
  _currentlyFocusedID: (null: ?number),

  /**
   * Returns the ID of the currently focused text field, if one exists
   * If no text field is focused it returns null
   */
  currentlyFocusedField: function (): ?number {
    return this._currentlyFocusedID;
  },

  /**
   * @param {number} TextInputID id of the text field to focus
   * Focuses the specified text field
   * noop if the text field was already focused
   */
  focusTextInput: function (textFieldID: ?number) {
    if (this._currentlyFocusedID !== textFieldID && textFieldID !== null) {
      UIManager.dispatchViewManagerCommand(
        textFieldID,
        UIManager.RCTTextInput.Commands.SetFocus,
        null
      );
    }
  },

  /** [TODO(android ISS)
   * @param {number} TextInputID id of the text field that has received focus
   * Should be called after the view has received focus and fired the onFocus event
   * noop if the focused text field is same
   */
  setFocusedTextInput: function (textFieldID: ?number) {
    if (this._currentlyFocusedID !== textFieldID && textFieldID !== null) {
      this._currentlyFocusedID = textFieldID;
    }
  }, // ]TODO(android ISS)

  /**
   * @param {number} textFieldID id of the text field to unfocus
   * Unfocuses the specified text field
   * noop if it wasn't focused
   */
  blurTextInput: function (textFieldID: ?number) {
    if (this._currentlyFocusedID === textFieldID && textFieldID !== null) {
      UIManager.dispatchViewManagerCommand(
        textFieldID,
        UIManager.RCTTextInput.Commands.Blur,
        null
      );
    }
  },

  /** [TODO(android ISS)
   * @param {number} TextInputID id of the text field whose focus has to be cleared
   * Should be called after the view has cleared focus and fired the onFocus event
   * noop if the focused text field is not same
   */
  clearFocusedTextInput: function (textFieldID: ?number) {
    if (this._currentlyFocusedID === textFieldID && textFieldID !== null) {
      this._currentlyFocusedID = null;
    } // ]TODO(android ISS)
  }
};

module.exports = TextInputState;
