'use strict';

import * as React from 'react';
import { StyleSheet, View, ViewProps } from 'react-native';

const styles = StyleSheet.create({
  unimplementedView: {
    borderWidth: 1,
    borderColor: 'red',
    alignSelf: 'flex-start'
  }
});

export default class UnimplementedView<T extends ViewProps> extends React.Component<T, {}> {
  public setNativeProps = () => {
    // Do nothing.
    // This method is required in order to use this view as a Touchable* child.
    // See ensureComponentIsNative.js for more info
  };

  public render() {
    // Workaround require cycle from requireNativeComponent
    return (
      // tslint:disable-next-line:no-any
      <View style={[styles.unimplementedView, this.props.style] as any}>{this.props.children}</View>
    );
  }
}
