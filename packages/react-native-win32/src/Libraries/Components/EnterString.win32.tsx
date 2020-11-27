/**
 * @providesModule EnterString
 */
'use strict';

import { requireNativeComponent, NativeSyntheticEvent } from 'react-native';
import * as React from 'react';
import PropTypes = require('prop-types');

export interface IEnterStringProps /*extends React.ClassAttributes<React.View>*/ {
  // ViewProperties {
  disabled?: boolean;
  value?: string;
  label?: string;
  onChanged?: (newValue: string) => void;
}

interface INativeOnChangedEventArgs {
  text: string;
}

// Same as EnterStringProps, but the event args are objects
interface IEnterStringNativeProps /*extends React.ClassAttributes<React.View>*/ {
  // ViewProperties {
  disabled?: boolean;
  value?: string;
  label?: string;
  onChanged?: (event: NativeSyntheticEvent<INativeOnChangedEventArgs>) => void;
}

class EnterStringNative extends React.Component<IEnterStringNativeProps, {}> {
  // mixins: [NativeMethodsMixin],

  /**
   * `NativeMethodsMixin` will look for this when invoking `setNativeProps`. We
   * make `this` look like an actual native component class.
   */
  /*
  viewConfig: {
    uiViewClassName: 'RCTEnterString',
    validAttributes: {
        ...ReactNativeViewAttributes.RCTView,
        text: true,
        disabled: true,
        label: true,
    },
  },
  */

 // TODO figure out how to get a real type here
  public static propTypes: any = {
    // React.ValidationMap<IEnterStringNativeProps> = {
    // ...View.propTypes,
    /**
     * Text to display inside the EnterString
     */
    value: PropTypes.string,

    /**
     * If true, disable all interactions for this component.
     */
    disabled: PropTypes.bool,

    /**
     * Label
     */
    label: PropTypes.string,

    /**
     * Callback when text is changed
     */
    onChanged: PropTypes.func,
  };

  public static DefaultProps: IEnterStringNativeProps = {
    disabled: false,
    value: '',
    label: '',
    onChanged: null,
  };

  public render() {
    return <RCTEnterString {...this.props} />;
  }
}

const RCTEnterString = requireNativeComponent('RCTEnterString');

export default class EnterString extends React.Component<IEnterStringProps, {}> {
  public static DefaultProps: IEnterStringProps = {
    disabled: false,
    value: '',
    label: '',
    onChanged: null,
  };

  public render() {
    const args = { ...this.props };
    if (args.onChanged) {
      args.onChanged = this._onChangedNative.bind(this);
    } // TODO how should this work without the cast

    return <EnterStringNative {...(args as any) as IEnterStringNativeProps} />;  }

  private _onChangedNative(event: NativeSyntheticEvent<INativeOnChangedEventArgs>) {
    if (this.props.onChanged) {
      this.props.onChanged(event.nativeEvent.text);
    }
  }
}
