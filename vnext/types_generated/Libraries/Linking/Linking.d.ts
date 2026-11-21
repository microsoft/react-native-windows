/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<7192d62238aa3936236e435dcda4031a>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\Linking\Linking.js
 */

import type { EventSubscription } from "../vendor/emitter/EventEmitter";
import NativeEventEmitter from "../EventEmitter/NativeEventEmitter";
type LinkingEventDefinitions = {
  url: [{
    url: string;
  }];
};
declare class LinkingImpl extends NativeEventEmitter<LinkingEventDefinitions> {
  constructor();
  /**
   * Listen for incoming URL changes.
   */
  addEventListener<K extends keyof LinkingEventDefinitions>(eventType: K, listener: (...$$REST$$: LinkingEventDefinitions[K]) => unknown): EventSubscription;
  /**
   * Open the given URL with any installed app that can handle it. This
   * includes URLs such as locations (e.g. "geo:37.484847,-122.148386"),
   * contacts, or any other URL that can be opened with installed apps.
   *
   * This method will fail if the system doesn't know how to open the
   * specified URL. If you're passing in a non-http(s) URL, it's best to
   * check `canOpenURL` first. For web URLs, the protocol ("http://",
   * "https://") must be set accordingly.
   */
  openURL(url: string): Promise<void>;
  /**
   * Check whether an installed app can handle a given URL. For web URLs,
   * the protocol ("http://", "https://") must be set accordingly. As of
   * iOS 9, your app needs to provide the `LSApplicationQueriesSchemes`
   * key inside Info.plist.
   */
  canOpenURL(url: string): Promise<boolean>;
  /**
   * Open the device Settings app and display the app’s custom settings, if
   * it has any.
   */
  openSettings(): Promise<void>;
  /**
   * Get the URL that launched the app, or `null` if it was not launched from
   * a link. To support deep linking on Android, see
   * https://developer.android.com/training/app-indexing/deep-linking.html#handling-intents.
   */
  getInitialURL(): Promise<null | undefined | string>;
  /**
   * Launch an Android intent with optional extras. Useful for deep-linking
   * to settings pages, opening an SMS app with a message draft in place,
   * and more. See https://developer.android.com/reference/kotlin/android/content/Intent.
   *
   * @platform android
   */
  sendIntent(action: string, extras?: Array<{
    key: string;
    value: string | number | boolean;
  }>): Promise<void>;
}
declare const Linking: LinkingImpl;
/**
 * General interface to interact with both incoming and outgoing app links,
 * including deep links and universal links.
 *
 * @see https://reactnative.dev/docs/linking
 */
declare const $$Linking: typeof Linking;
declare type $$Linking = typeof $$Linking;
export default $$Linking;
