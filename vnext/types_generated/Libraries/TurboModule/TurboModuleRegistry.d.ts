/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<964707895ad63997d36bb6d4f2b7b2ee>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\TurboModule\TurboModuleRegistry.js
 */

import type { TurboModule } from "./RCTExport";
export declare function get<T extends TurboModule>(name: string): null | undefined | T;
export declare function getEnforcing<T extends TurboModule>(name: string): T;
