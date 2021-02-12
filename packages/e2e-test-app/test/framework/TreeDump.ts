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
    roundMeasurements?: boolean;
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

  if (opts?.roundMeasurements !== false) {
    roundToGrid(element);
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
 * Rounds render sizes to nearest 5 px grid to account for subtle differences
 * in measurement (e.g. from rounding on different DPI devices)
 *
 * This is a hack to get around Jest snapshots needing deterministic matchers
 * and snapshot values. It can break down, e.g. if one measurement is 27,
 * but the other is 28.
 */
function roundToGrid(element: UIElement) {
  const NEAREST = 5;

  if (element.RenderSize) {
    element.RenderSize = element.RenderSize.map(
      s => Math.round(s / NEAREST) * NEAREST,
    );
  }

  if (element.children) {
    element.children.forEach(roundToGrid);
  }
}
