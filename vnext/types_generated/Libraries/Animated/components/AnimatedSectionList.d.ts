/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<8552ed11648e18da8ac2bec91a23d39d>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\Animated\components\AnimatedSectionList.js
 */

import type { AnimatedProps } from "../createAnimatedComponent";
import SectionList, { type SectionListProps } from "../../Lists/SectionList";
import * as React from "react";
declare const AnimatedSectionList: <ItemT = any, SectionT = any>(props: Omit<AnimatedProps<SectionListProps<ItemT, SectionT>>, "ref"> & {
  ref?: React.Ref<SectionList<ItemT, SectionT>> | undefined;
}) => React.ReactNode;
declare const $$AnimatedSectionList: typeof AnimatedSectionList;
declare type $$AnimatedSectionList = typeof $$AnimatedSectionList;
export default $$AnimatedSectionList;
