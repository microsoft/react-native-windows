import React from 'react'
import RN = require('react-native');
import type { TextProps } from '../../Text/Text';

/**
 * All of TOrigin except Key from TUse
 */
type UseFrom<TOrigin, TUse, Key extends keyof TUse> = Pick<TOrigin, Exclude<keyof TOrigin, Key>> & Pick<TUse, Key>;

/**
 * React-native <Text> control with additional Win32-specific functionality.
 */

type InnerViewProps = UseFrom<TextProps, RN.TextProps, 'accessibilityRole'> &
  UseFrom<TextProps, RN.TextProps, 'accessibilityState'> &
  UseFrom<TextProps, RN.TextProps, 'accessibilityActions'>;
export class TextWin32 extends React.Component<TextProps, {}> {
  constructor(props: TextProps) {
    super(props);
  }

  public render() {
    return <RN.Text {...(this.props as InnerViewProps)} />;
  }
}
