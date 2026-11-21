/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<a1e18c9f25240f870c211796b2aed5d7>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\Components\LayoutConformance\LayoutConformance.js
 */

import * as React from "react";
export type LayoutConformanceProps = Readonly<{
  /**
   * strict: Layout in accordance with W3C spec, even when breaking
   * compatibility: Layout with the same behavior as previous versions of React Native
   */
  mode: "strict" | "compatibility";
  children: React.ReactNode;
}>;
declare function LayoutConformance(props: LayoutConformanceProps): React.ReactNode;
export default LayoutConformance;
