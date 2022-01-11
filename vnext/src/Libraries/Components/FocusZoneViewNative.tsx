// We extract FocusZoneViewNative to its own file to workaround getting errors when editing files with requireNativeComponent.

import React from 'react';
import {ViewProps, requireNativeComponent} from 'react-native';

export type FocusZoneProps = {
  // Mac Only
  focusDirection?: 'horizontal' | 'vertical';

  // Windows only. For details see https://docs.microsoft.com/en-us/uwp/api/windows.ui.xaml.input.xyfocusnavigationstrategy?view=winrt-20348. Default is projection.
  xyFocusNavigationStrategy?:
    | 'projection'
    | 'navigationDirectionDistance'
    | 'rectilinearDistance';
};

export type Props = ViewProps &
  FocusZoneProps & {
    children?: React.ReactNode;
  };

export const FocusZoneViewNative = requireNativeComponent(
  'FocusZoneView',
) as React.ComponentClass<Props>;
