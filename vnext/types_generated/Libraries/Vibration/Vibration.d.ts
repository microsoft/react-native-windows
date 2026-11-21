/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<b29d032fcbba4045694d6adfce9356b8>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\Vibration\Vibration.js
 */

declare const Vibration: {
  /**
   * Trigger a vibration with the specified `pattern`.
   *
   * The pattern can be a number (duration in ms) or an array of numbers. When
   * an array is provided on Android, odd indices represent vibration duration
   * and even indices represent separation time. On iOS, the duration value is
   * ignored and each vibration lasts approximately 400ms.
   */
  vibrate: (pattern?: number | Array<number>, repeat?: boolean) => void;
  /**
   * Stop vibrating after `vibrate()` was called with repetition enabled.
   */
  cancel: () => void;
};
/**
 * Vibrates the device. On Android, requires the `android.permission.VIBRATE`
 * permission. On iOS, vibration duration is fixed at approximately 400ms
 * (implemented via `AudioServicesPlaySystemSound(kSystemSoundID_Vibrate)`).
 *
 * @see https://reactnative.dev/docs/vibration
 */
declare const $$Vibration: typeof Vibration;
declare type $$Vibration = typeof $$Vibration;
export default $$Vibration;
