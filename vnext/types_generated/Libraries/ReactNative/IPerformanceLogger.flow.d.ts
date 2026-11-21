/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<545d368ba5114fc088d1b0b1abce592d>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\ReactNative\IPerformanceLogger.flow.js
 */

export type ExtraValue = number | string | boolean;
export type Extras = {
  [key: string]: ExtraValue;
};
export type Timespan = {
  startTime: number;
  endTime?: number | undefined;
  totalTime?: number | undefined;
  startExtras?: Extras | undefined;
  endExtras?: Extras | undefined;
};
/**
 * @deprecated The scoped performance logger argument passed to the
 * `componentProviderInstrumentationHook` is no longer used by `react-native`
 * itself and will be removed in a future release. The instance supplied today
 * is a no-op stub.
 */
export interface IPerformanceLogger {
  addTimespan(key: string, startTime: number, endTime: number, startExtras?: Extras, endExtras?: Extras): void;
  append(logger: IPerformanceLogger): void;
  clear(): void;
  clearCompleted(): void;
  close(): void;
  currentTimestamp(): number;
  getExtras(): Readonly<{
    [key: string]: ExtraValue | undefined;
  }>;
  getPoints(): Readonly<{
    [key: string]: number | undefined;
  }>;
  getPointExtras(): Readonly<{
    [key: string]: Extras | undefined;
  }>;
  getTimespans(): Readonly<{
    [key: string]: Timespan | undefined;
  }>;
  hasTimespan(key: string): boolean;
  isClosed(): boolean;
  logEverything(): void;
  markPoint(key: string, timestamp?: number, extras?: Extras): void;
  removeExtra(key: string): ExtraValue | undefined;
  setExtra(key: string, value: ExtraValue): void;
  startTimespan(key: string, timestamp?: number, extras?: Extras): void;
  stopTimespan(key: string, timestamp?: number, extras?: Extras): void;
}
