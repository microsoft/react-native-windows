/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as _ from 'lodash';
import {RpcClient} from 'jest-environment-winappdriver';
import 'jest-extended';

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

  // eslint-disable-next-line @typescript-eslint/no-namespace
  namespace jest {
    interface Matchers<R> {
      toRoughlyMatchSnapshot(element: UIElement): CustomMatcherResult;
    }

    interface Expect {
      toMatchRenderSize(actual: number[], epsilon: number): CustomMatcherResult;
    }
  }
}

expect.extend({
  /**
   * Checks that a UIElement snapshot matches, with fuzzy comparision
   */
  toRoughlyMatchSnapshot: (
    element: UIElement,
    epsilon: number = 1,
  ): jest.CustomMatcherResult => {
    expect(element).toMatchSnapshot(
      createUIElementProprtyMatcher(element, epsilon),
    );

    return {pass: true, message: () => ''};
  },

  /**
   * Checks that a UIElement snapshot matches, with fuzzy comparision
   */
  toMatchRenderSize: (
    actual: number[],
    expected: number[],
    epsilon: number,
  ): jest.CustomMatcherResult => {
    const message = () =>
      `Recieved RenderSize "${JSON.stringify(
        actual,
      )}" does not match the expected "${JSON.stringify(expected)}"`;

    if (actual.length !== expected.length) {
      return {pass: false, message};
    }

    for (let i = 0; i < expected.length; i++) {
      if (
        actual[i] > expected[i] + epsilon ||
        actual[i] < expected[i] - epsilon
      ) {
        return {pass: false, message};
      }
    }

    return {pass: true, message};
  },
});

function createUIElementProprtyMatcher(
  element: UIElement,
  epsilon: number,
): Record<keyof UIElement, any> {
  const matcher: Record<keyof UIElement, any> = {};

  if (element.Height !== null && element.Height !== undefined) {
    matcher.Height = expect.toBeWithin(
      element.Height - epsilon,
      element.Height + epsilon,
    );
  }

  if (element.Width !== null && element.Width !== undefined) {
    matcher.Width = expect.toBeWithin(
      element.Width - epsilon,
      element.Width + epsilon,
    );
  }

  if (element.RenderSize) {
    matcher.RenderSize = expect.toMatchRenderSize(element.RenderSize, epsilon);
  }

  if (element.children) {
    const childMatchers = element.children.map(child =>
      createUIElementProprtyMatcher(child, epsilon),
    );

    if (childMatchers.length < 1) {
      throw new Error();
    }

    matcher.children = _.merge({}, ...childMatchers);
  }

  return matcher;
}

/**
 * Dump a section of the native visual tree.
 */
export async function dumpVisualTree(
  accessibilityId: string,
  opts?: {
    pruneCollapsed?: boolean;
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
