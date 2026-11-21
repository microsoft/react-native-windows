/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<54a4dfc0cdfe37b3ca88c02a8005d2f0>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\Utilities\useColorScheme.js
 */

import type { ColorSchemeName } from "./NativeAppearance";
/**
 * React hook that provides and subscribes to color scheme updates from the
 * `Appearance` module. Returns `'light'`, `'dark'`, or `null`.
 *
 * Notes:
 * - `null` will only be returned if the native Appearance module is unavailable
 *   (out of tree platforms).
 *
 * @see https://reactnative.dev/docs/usecolorscheme
 */
declare function useColorScheme(): ColorSchemeName | null;
export default useColorScheme;
