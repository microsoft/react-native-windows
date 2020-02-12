/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 * @flow strict
 */

'use strict';

import type {SyntheticEvent} from '../../Types/CoreEventTypes';

export type AccessibilityTrait =
  | 'none'
  | 'button'
  | 'link'
  | 'header'
  | 'search'
  | 'image'
  | 'selected'
  | 'plays'
  | 'key'
  | 'text'
  | 'summary'
  | 'disabled'
  | 'frequentUpdates'
  | 'startsMedia'
  | 'adjustable'
  | 'allowsDirectInteraction'
  | 'pageTurn' // [TODO(macOS ISS#2323203)
  | 'group'
  | 'list'; // ]TODO(macOS ISS#2323203)

export type AccessibilityTraits =
  | AccessibilityTrait
  | $ReadOnlyArray<AccessibilityTrait>;

export type AccessibilityComponentType =
  | 'none'
  | 'button'
  | 'radiobutton_checked'
  | 'radiobutton_unchecked';

// [TODO(android ISS)
export type AccessibilityNodeInfoProp = {
  clickable: boolean,
}; // ]TODO(android ISS)

// This must be kept in sync with the AccessibilityRolesMask in RCTViewManager.m
export type AccessibilityRole =
  | 'none'
  | 'button'
  | 'link'
  | 'search'
  | 'image'
  | 'keyboardkey'
  | 'text'
  | 'adjustable'
  | 'imagebutton'
  | 'header'
  | 'summary'
  | 'alert'
  | 'checkbox'
  | 'combobox'
  | 'menu'
  | 'menubar'
  | 'menuitem'
  | 'progressbar'
  | 'radio'
  | 'radiogroup'
  | 'scrollbar'
  | 'spinbutton'
  | 'switch'
  | 'tab'
  | 'tablist'
  | 'timer'
  | 'toolbar'
  | 'list' // RNW-only
  | 'listitem'; // RNW-only

export type AccessibilityState =
  | 'selected'
  | 'disabled'
  | 'checked'
  | 'unchecked'
  | 'busy'
  | 'expanded'
  | 'collapsed';

export type AccessibilityStates =
  | AccessibilityState
  | $ReadOnlyArray<AccessibilityState>;

module.exports = {
  AccessibilityTraits: [
    'none',
    'button',
    'link',
    'header',
    'search',
    'image',
    'selected',
    'plays',
    'key',
    'text',
    'summary',
    'disabled',
    'frequentUpdates',
    'startsMedia',
    'adjustable',
    'allowsDirectInteraction',
    'pageTurn',
    'group', // [TODO(macOS ISS#2323203)
    'list', // ]TODO(macOS ISS#2323203)
  ],
  AccessibilityComponentTypes: [
    'none',
    'button',
    'radiobutton_checked',
    'radiobutton_unchecked',
  ],
  AccessibilityRoles: [
    'none',
    'button',
    'link',
    'search',
    'image',
    'keyboardkey',
    'text',
    'adjustable',
    'imagebutton',
    'header',
    'summary',
    'alert',
    'checkbox',
    'combobox',
    'menu',
    'menubar',
    'menuitem',
    'progressbar',
    'radio',
    'radiogroup',
    'scrollbar',
    'spinbutton',
    'switch',
    'tab',
    'tablist',
    'timer',
    'toolbar',
    'list', // RNW-only
    'listitem', // RNW-only
  ],
  AccessibilityStates: [
    'selected',
    'disabled',
    'checked',
    'unchecked',
    'busy',
    'expanded',
    'collapsed',
  ],
};

// the info associated with an accessibility action
export type AccessibilityActionInfo = $ReadOnly<{
  name: string,
  label?: string,
}>;

// The info included in the event sent to onAccessibilityAction
export type AccessibilityActionEvent = SyntheticEvent<
  $ReadOnly<{
    actionName: string,
  }>,
>;
