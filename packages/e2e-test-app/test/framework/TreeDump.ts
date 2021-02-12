/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {RpcClient} from 'jest-environment-winappdriver';

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
  HorizontalAlignmen?: string | null;
  Clip?: string | null;
  FlowDirection?: string | null;
  Name?: string | null;
  Text?: string | null;
  children?: UIElement[];
  [index: string]: unknown;
};

declare global {
  const rpcClient: RpcClient | undefined;
}

/**
 * Dump a section of the native visual tree.
 */
export async function dumpVisualTree(
  accessibilityId: string,
  opts?: {
    pruneCollapsed?: boolean;
    deterministicOnly?: boolean;
    additionalProperties?: string[];
  },
): Promise<UIElement> {
  if (!rpcClient) {
    throw new Error('RPC client is not enabled');
  }

  const dumpResponse = await rpcClient.invoke('DumpVisualTree', {
    accessibilityId,
    ...opts,
  });

  if (dumpResponse.type === 'error') {
    fail(dumpResponse.message);
  }

  const element = dumpResponse.result;

  if (opts?.pruneCollapsed !== false) {
    pruneCollapsedElements(element);
  }

  if (opts?.deterministicOnly !== false) {
    removeNonDeterministicProps(element);
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
 * Removes trees of properties that are not determinisitc
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
