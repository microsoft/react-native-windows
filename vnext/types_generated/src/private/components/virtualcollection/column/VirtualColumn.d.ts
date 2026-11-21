/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<3c31ce1718134bdb1987cb982edea01a>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\src\private\components\virtualcollection\column\VirtualColumn.js
 */

import type { Item, VirtualCollection } from "../Virtual";
import * as React from "react";
declare const VirtualColumn: <TItem extends Item>(props: {
  children: (item: TItem, key: string) => React.ReactNode;
  items: VirtualCollection<TItem>;
  itemToKey?: (($$PARAM_0$$: TItem) => string) | undefined;
  removeClippedSubviews?: boolean | undefined;
  testID?: null | undefined | string;
}) => React.ReactNode;
declare const $$VirtualColumn: typeof VirtualColumn;
declare type $$VirtualColumn = typeof $$VirtualColumn;
export default $$VirtualColumn;
