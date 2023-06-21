/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {AutomationClient} from '@react-native-windows/automation-channel';

/**
 * Schema of tree dumped node
 */
export type UIElement = {
  XamlType: string;
  Foreground?: string | null;
  Background?: string | null;
  Padding?: string | null;
  Margin?: string | null;
  RenderSize?: number[] | null;
  Visibility?: 'Collapsed' | 'Visible' | null;
  CornerRadius?: string | null;
  BorderThickness?: string | null;
  Width?: number | null;
  Height?: number | null;
  BorderBrush?: string | null;
  VerticalAlignment?: string | null;
  HorizontalAlignment?: string | null;
  Clip?: string | null;
  FlowDirection?: string | null;
  Name?: string | null;
  Text?: string | null;
  children?: UIElement[];
  [index: string]: unknown;
};

declare global {
  const automationClient: AutomationClient | undefined;
}

/**
 * Dump a section of the native visual tree.
 */
export default async function dumpVisualTree(
  accessibilityId: string,
  opts?: {
    pruneCollapsed?: boolean;
    deterministicOnly?: boolean;
    removeDefaultProps?: boolean;
    additionalProperties?: string[];
  },
): Promise<UIElement> {
  if (!automationClient) {
    throw new Error('RPC client is not enabled');
  }

  const dumpResponse = await automationClient.invoke('DumpVisualTree', {
    accessibilityId,
    ...opts,
  });

  if (dumpResponse.type === 'error') {
    throw new Error(dumpResponse.message);
  }

  const element = dumpResponse.result;

  if (opts?.pruneCollapsed !== false) {
    pruneCollapsedElements(element);
  }

  if (opts?.deterministicOnly !== false) {
    removeNonDeterministicProps(element);
  }

  if (opts?.removeDefaultProps !== false) {
    removeDefaultProps(element);
  }

  return element;
}

/**
 * Removes trees of XAML that are not visible.
 */
function pruneCollapsedElements(element: UIElement) {
  if (!element.children) {
    return;
  }

  element.children = element.children.filter(
    child => child.Visibility !== 'Collapsed',
  );

  element.children.forEach(pruneCollapsedElements);
}

/**
 * Removes trees of properties that are not deterministic
 */
function removeNonDeterministicProps(element: UIElement) {
  if (element.RenderSize) {
    // RenderSize is subject to rounding, etc and should mostly be derived from
    // other deterministic properties in the tree.
    delete element.RenderSize;
  }

  if (element.children) {
    element.children.forEach(removeNonDeterministicProps);
  }
}

/**
 * Removes noise from snapshot by removing properties with the default value
 */
function removeDefaultProps(element: UIElement) {
  const defaultValues: [string, unknown][] = [
    ['Tooltip', null],
    ['AutomationLevel', -1],
    ['AutomationPositionInSet', -1],
    ['AutomationSizeOfSet', -1],
    ['AccessibilityRole', null],
    ['AccessibilityStateSelected', null],
    ['AccessibilityStateDisabled', null],
    ['AccessibilityStateChecked', null],
    ['AccessibilityStateBusy', null],
    ['AccessibilityStateExpanded', null],
  ];

  defaultValues.forEach(([propname, defaultValue]) => {
    if (element[propname] === defaultValue) {
      delete element[propname];
    }
  });

  if (element.children) {
    element.children.forEach(removeDefaultProps);
  }
}
