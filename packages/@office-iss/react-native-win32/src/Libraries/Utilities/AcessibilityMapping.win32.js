/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @flow strict-local
 * @format
 */

'use strict';

import type {
  AccessibilityRole,
  Role,
} from '../Components/View/ViewAccessibility';

// Map role values to AccessibilityRole values
export function getAccessibilityRoleFromRole(role: Role): ?AccessibilityRole {
  switch (role) {
    case 'alert':
      return 'alert';
    case 'alertdialog':
      return 'alertdialog'; // Win32
    case 'application':
      return 'application'; // Win32
    case 'article':
      return undefined;
    case 'banner':
      return undefined;
    case 'button':
      return 'button';
    case 'cell':
      return undefined;
    case 'checkbox':
      return 'checkbox';
    case 'columnheader':
      return undefined;
    case 'combobox':
      return 'combobox';
    case 'complementary':
      return undefined;
    case 'contentinfo':
      return undefined;
    case 'definition':
      return undefined;
    case 'dialog':
      return 'dialog'; // Win32
    case 'directory':
      return undefined;
    case 'document':
      return undefined;
    case 'feed':
      return undefined;
    case 'figure':
      return undefined;
    case 'form':
      return undefined;
    case 'grid':
      return 'grid';
    case 'group':
      return 'group'; // Win32
    case 'heading':
      return 'header';
    case 'img':
      return 'image';
    case 'link':
      return 'link';
    case 'list':
      return 'list';
    case 'listitem':
      return 'listitem'; // Win32
    case 'log':
      return undefined;
    case 'main':
      return undefined;
    case 'marquee':
      return undefined;
    case 'math':
      return undefined;
    case 'menu':
      return 'menu';
    case 'menubar':
      return 'menubar';
    case 'menuitem':
      return 'menuitem';
    case 'meter':
      return undefined;
    case 'navigation':
      return undefined;
    case 'none':
      return 'none';
    case 'note':
      return undefined;
    case 'option':
      return undefined;
    case 'presentation':
      return 'presentation'; // Win32
    case 'progressbar':
      return 'progressbar';
    case 'radio':
      return 'radio';
    case 'radiogroup':
      return 'radiogroup';
    case 'region':
      return undefined;
    case 'row':
      return undefined;
    case 'rowgroup':
      return undefined;
    case 'rowheader':
      return undefined;
    case 'scrollbar':
      return 'scrollbar';
    case 'searchbox':
      return 'search';
    case 'separator':
      return undefined;
    case 'slider':
      return 'adjustable';
    case 'spinbutton':
      return 'spinbutton';
    case 'status':
      return undefined;
    case 'summary':
      return 'summary';
    case 'switch':
      return 'switch';
    case 'tab':
      return 'tab';
    case 'table':
      return undefined;
    case 'tablist':
      return 'tablist';
    case 'tabpanel':
      return 'tabpanel'; // Win32
    case 'term':
      return undefined;
    case 'textbox': // Win32
      return 'textbox'; // Win32
    case 'timer':
      return 'timer';
    case 'toolbar':
      return 'toolbar';
    case 'tooltip':
      return undefined;
    case 'tree':
      return 'tree'; // Win32
    case 'treegrid':
      return undefined;
    case 'treeitem':
      return 'treeitem'; // Win32
  }

  return undefined;
}
