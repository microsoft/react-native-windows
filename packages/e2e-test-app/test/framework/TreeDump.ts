/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {RpcClient} from 'jest-environment-winappdriver';

declare global {
  const rpcClient: RpcClient | undefined;
}

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

/**
 * Dump a section of the native visual tree
 */
export async function dumpVisualTree(
  accessibilityId: string,
  opts?: {
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

  return pruneCollapsedElements(dumpResponse.result);
}

function pruneCollapsedElements(element: UIElement): UIElement {
  if (!element.children) {
    return element;
  }

  const newElement = {...element};

  newElement.children = element.children
    .filter(child => child.Visibility !== 'Collapsed')
    .map(pruneCollapsedElements);

  return newElement;
}
