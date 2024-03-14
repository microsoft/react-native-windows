import React from 'react'
import RN = require('react-native');
import type { TextPropsWin32 } from '../../Text/Text';

/**
 * All of TOrigin except Key from TUse
 */
type UseFrom<TOrigin, TUse, Key extends keyof TUse> = Pick<TOrigin, Exclude<keyof TOrigin, Key>> & Pick<TUse, Key>;

/**
 * React-native <Text> control with additional Win32-specific functionality.
 */

type InnerViewProps = UseFrom<TextPropsWin32, RN.TextProps, 'accessibilityRole'> &
  UseFrom<TextPropsWin32, RN.TextProps, 'accessibilityState'> &
  UseFrom<TextPropsWin32, RN.TextProps, 'accessibilityActions'>;
export class TextWin32 extends React.Component<TextPropsWin32, {}> {
  constructor(props: TextPropsWin32) {
    super(props);
  }

  public render() {
    return <RN.Text {...(this.props as InnerViewProps)} />;
  }
}
