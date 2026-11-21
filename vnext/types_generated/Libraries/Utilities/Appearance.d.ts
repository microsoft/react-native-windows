/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<49eb86ff6cd24f59f88d5b66919e8ec3>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\Utilities\Appearance.js
 */

import type { EventSubscription } from "../vendor/emitter/EventEmitter";
import type { ColorSchemeName, ColorSchemeOverride } from "./NativeAppearance";
export type { ColorSchemeName, ColorSchemeOverride };
export type AppearancePreferences = {
  colorScheme: ColorSchemeName | null;
};
/**
 * Returns the active color scheme (`'light'` or `'dark'`). This value may
 * change at runtime, either at the system level (e.g. scheduled color scheme
 * change at sunrise or sunset) or when overridden at the app level via
 * `setColorScheme()`.
 *
 * Prefer `useColorScheme()` in React components.
 *
 * Notes:
 * - `null` will only be returned if the native Appearance module is unavailable
 *   (out of tree platforms).
 */
export declare function getColorScheme(): ColorSchemeName | null;
/**
 * Force the application to always adopt a light or dark interface style. Pass
 * `'auto'` to reset and follow the system default (removes any override).
 * This does not affect the system UI, only the application.
 */
export declare function setColorScheme(colorScheme: ColorSchemeOverride): void;
/**
 * Subscribe to color scheme changes. The listener receives the new appearance
 * preferences whenever the color scheme changes, whether from a system event
 * or a call to `setColorScheme()`.
 */
export declare function addChangeListener(listener: (preferences: AppearancePreferences) => void): EventSubscription;
