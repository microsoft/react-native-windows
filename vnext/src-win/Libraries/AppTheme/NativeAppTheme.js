/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @flow strict
 * @format
 */

import type {TurboModule} from '../TurboModule/RCTExport';
import * as TurboModuleRegistry from '../TurboModule/TurboModuleRegistry';

export type HighContrastColors = {|
  ButtonFaceColor: string,
  ButtonTextColor: string,
  GrayTextColor: string,
  HighlightColor: string,
  HighlightTextColor: string,
  HotlightColor: string,
  WindowColor: string,
  WindowTextColor: string,
|};

export type AppThemeData = {|
  isHighContrast: boolean,
  highContrastColors: HighContrastColors,
|};

export interface Spec extends TurboModule {
  +getConstants: () => AppThemeData;
}

export default (TurboModuleRegistry.get<Spec>('AppTheme'): ?Spec);
