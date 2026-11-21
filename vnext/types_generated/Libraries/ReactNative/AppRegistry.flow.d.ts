/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<7363d7ba3659a2f48600442d86996d7d>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\ReactNative\AppRegistry.flow.js
 */

import * as React from "react";
import type { ViewStyleProp } from "../StyleSheet/StyleSheet";
import type { RootTag } from "../Types/RootTagTypes";
import type { DisplayModeType } from "./DisplayMode";
import type { IPerformanceLogger } from "./IPerformanceLogger.flow";
type HeadlessTask = (taskData: any) => Promise<void>;
export type TaskProvider = () => HeadlessTask;
export type ComponentProvider = () => React.ComponentType<any>;
export type ComponentProviderInstrumentationHook = (component_: ComponentProvider, scopedPerformanceLogger: IPerformanceLogger) => React.ComponentType<any>;
export type AppConfig = {
  appKey: string;
  component?: ComponentProvider | undefined;
  run?: Runnable | undefined;
  section?: boolean | undefined;
};
export type AppParameters = {
  initialProps: Readonly<{
    [$$Key$$: string]: unknown;
  }>;
  rootTag: RootTag;
};
export type Runnable = (appParameters: AppParameters, displayMode: DisplayModeType) => void;
export type Runnables = {
  [appKey: string]: Runnable;
};
export type Registry = {
  sections: ReadonlyArray<string>;
  runnables: Runnables;
};
export type WrapperComponentProvider = (appParameters?: AppParameters, appKey?: string) => React.ComponentType<any>;
export type RootViewStyleProvider = (appParameters: AppParameters) => ViewStyleProp;
