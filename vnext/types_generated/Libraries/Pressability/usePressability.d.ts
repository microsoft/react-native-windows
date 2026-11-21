/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<44c02703043914812c0ac2c02588ca5e>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\Pressability\usePressability.js
 */

import { type EventHandlers, type PressabilityConfig } from "./Pressability";
/**
 * Creates a persistent instance of `Pressability` that automatically configures
 * itself and resets. Accepts null `config` to support lazy initialization. Once
 * initialized, will not un-initialize until the component has been unmounted.
 *
 * In order to use `usePressability`, do the following:
 *
 *   const config = useMemo(...);
 *   const eventHandlers = usePressability(config);
 *   const pressableView = <View {...eventHandlers} />;
 *
 */
declare function usePressability(config: null | undefined | PressabilityConfig): null | EventHandlers;
export default usePressability;
