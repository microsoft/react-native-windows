/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<6c6018937a5dbdf9811626be5507b3bc>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\src\private\types\HostComponent.js
 */

import * as React from "react";
import type { HostInstance } from "./HostInstance";
export type HostComponent<Config extends {}> = (props: Omit<Config, "ref"> & {
  ref?: React.Ref<HostInstance> | undefined;
}) => React.ReactNode;
