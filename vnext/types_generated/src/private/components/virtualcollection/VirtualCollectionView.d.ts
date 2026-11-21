/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<5de7b192b8e540237c5cd6238555c1d2>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\src\private\components\virtualcollection\VirtualCollectionView.js
 */

import type { ViewStyleProp } from "../../../../Libraries/StyleSheet/StyleSheet";
import type { ModeChangeEvent } from "../virtualview/VirtualView";
import type { Item, VirtualCollection } from "./Virtual";
import * as React from "react";
export type VirtualCollectionLayoutComponent<TLayoutProps extends {}> = (props: Omit<TLayoutProps, "children" | "spacer"> & {
  children: ReadonlyArray<React.ReactNode>;
  spacer: React.ReactNode;
}) => React.ReactNode;
export type VirtualCollectionGenerator = Readonly<{
  initial: Readonly<{
    itemCount: number;
    spacerStyle: (itemCount: number) => ViewStyleProp;
  }>;
  next: (event: ModeChangeEvent) => {
    itemCount: number;
    spacerStyle: (itemCount: number) => ViewStyleProp;
  };
}>;
export type VirtualCollectionViewComponent<TLayoutProps extends {}> = <TItem extends Item>(props: Omit<TLayoutProps, "children" | "items" | "itemToKey" | "removeClippedSubviews" | "testID"> & {
  children: (item: TItem, key: string) => React.ReactNode;
  items: VirtualCollection<TItem>;
  itemToKey?: (($$PARAM_0$$: TItem) => string) | undefined;
  removeClippedSubviews?: boolean | undefined;
  testID?: null | undefined | string;
}) => React.ReactNode;
/**
 * Creates a component that virtually renders a collection of items and manages
 * lazy rendering, memoization, and pagination. The resulting component accepts
 * the following base props:
 *
 * - `children`: A function maps an item to a React node.
 * - `items`: A collection of items to render.
 * - `itemToKey`: A function maps an item to a unique key.
 *
 * The first argument is a layout component that defines layout of the item and
 * spacer. It always receives the following props:
 *
 * - `children`: An array of React nodes (for items rendered so far).
 * - `spacer`: A React node (estimates layout for items not yet rendered).
 *
 * The layout component must render `children` and `spacer`. It can also define
 * additional props that will be passed through from the resulting component.
 *
 * The second argument is a generator that defines the initial rendering and
 * pagination behavior. The initial rendering behavior is defined by the
 * `initial` property with the following properties:
 *
 * - `itemCount`: Number of items to render initially.
 * - `spacerStyle`: A function that estimates the layout of the spacer. It
 *   receives the number of items being rendered as an argument.
 *
 * The pagination behavior is defined by the `next` function that receives a
 * `ModeChangeEvent` and then returns an object with the following properties:
 *
 * - `itemCount`: Number of additional items needed to fill `thresholdRect`.
 * - `spacerStyle`: A function that estimates the layout of the spacer. It
 *   receives the number of items being rendered as an argument.
 *
 */
export declare function createVirtualCollectionView<TLayoutProps extends {}>(VirtualLayout: VirtualCollectionLayoutComponent<TLayoutProps>, $$PARAM_1$$: VirtualCollectionGenerator): VirtualCollectionViewComponent<TLayoutProps>;
