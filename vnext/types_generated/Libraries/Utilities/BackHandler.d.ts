/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<72bcfdeee33354c593bcc381f9de2ea0>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\Utilities\BackHandler.js.flow
 */

export type BackPressEventName = "backPress" | "hardwareBackPress";
/**
 * Event dispatched when the hardware back button is pressed.
 * The `timeStamp` property reflects the native timestamp captured
 * when the back press was emitted.
 */
export interface HardwareBackPressEvent {
  readonly type: string;
  readonly timeStamp: number;
}
type TBackHandler = {
  exitApp(): void;
  addEventListener(eventName: BackPressEventName, handler: (event: HardwareBackPressEvent) => boolean | undefined): {
    remove: () => void;
  };
};
declare const BackHandler: TBackHandler;
/**
 * Detect hardware back button presses, and programmatically invoke the
 * default back button functionality to exit the app if there are no
 * listeners or if none of the listeners return true.
 * The event subscriptions are called in reverse order
 * (i.e. last registered subscription first), and if one subscription
 * returns true then subscriptions registered earlier
 * will not be called.
 *
 * @see https://reactnative.dev/docs/backhandler
 */
declare const $$BackHandler: typeof BackHandler;
declare type $$BackHandler = typeof $$BackHandler;
export default $$BackHandler;
