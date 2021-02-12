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

  const element = dumpResponse.result;
  massageForComparison(element);
  return element;
}

/**
 * Massages the element to values which are safe for exact comparison. E.g.
 * rounding or removing non-deterministic values.
 */
function massageForComparison(element: UIElement) {
  pruneCollapsedElements(element);
  roundMeasurements(element);
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
 * Round measurements, to account for differences in scale-factor, rounding,
 * etc.
 */
function roundMeasurements(element: UIElement) {
  // Allow measuresments to be a bit more than 1 DIP apart
  const DIPS_FUZZINESS = 1.25;

  const roundFactor = DIPS_FUZZINESS * 2;
  if (element.Width !== undefined && element.Width !== null) {
    element.Width = roundToNearest(element.Width, roundFactor);
  }

  if (element.Height !== undefined && element.Height !== null) {
    element.Height = roundToNearest(element.Height, roundFactor);
  }

  if (element.RenderSize) {
    element.RenderSize = element.RenderSize.map(size =>
      roundToNearest(size, roundFactor),
    );
  }

  if (element.children) {
    element.children.forEach(roundMeasurements);
  }
}

function roundToNearest(num: number, nearest: number): number {
  return Math.round(num / nearest) * nearest;
}
