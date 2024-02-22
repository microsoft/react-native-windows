import React from 'react'
import RN = require('react-native');
import type { ITextWin32Props } from './TextWin32.Props';

/**
 * All of TOrigin except Key from TUse
 */
type UseFrom<TOrigin, TUse, Key extends keyof TUse> = Pick<TOrigin, Exclude<keyof TOrigin, Key>> & Pick<TUse, Key>;

/**
 * React-native <Text> control with additional Win32-specific functionality.
 */

type InnerViewProps = UseFrom<ITextWin32Props, RN.TextProps, 'accessibilityRole'> &
  UseFrom<ITextWin32Props, RN.TextProps, 'accessibilityState'> &
  UseFrom<ITextWin32Props, RN.TextProps, 'accessibilityActions'>;
export class TextWin32 extends React.Component<ITextWin32Props, {}> {
  constructor(props: ITextWin32Props) {
    super(props);
  }

  public render() {
    return <RN.Text {...(this.props as InnerViewProps)} />;
  }
}
