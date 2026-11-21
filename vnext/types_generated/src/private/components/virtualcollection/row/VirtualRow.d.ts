/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<06d0583cbe888be7194271560432b0e7>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\src\private\components\virtualcollection\row\VirtualRow.js
 */

import type { Item, VirtualCollection } from "../Virtual";
import * as React from "react";
declare const VirtualRow: <TItem extends Item>(props: {
  children: (item: TItem, key: string) => React.ReactNode;
  items: VirtualCollection<TItem>;
  itemToKey?: (($$PARAM_0$$: TItem) => string) | undefined;
  removeClippedSubviews?: boolean | undefined;
  testID?: null | undefined | string;
}) => React.ReactNode;
declare const $$VirtualRow: typeof VirtualRow;
declare type $$VirtualRow = typeof $$VirtualRow;
export default $$VirtualRow;
