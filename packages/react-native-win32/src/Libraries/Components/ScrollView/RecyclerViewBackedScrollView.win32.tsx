/**
 * @providesModule RecyclerViewBackedScrollView
 */

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

class RecyclerViewBackedScrollView extends React.Component<ViewProps, {}> {
  public render() {
    return (
      // tslint:disable-next-line:no-any
      <View style={[styles.unimplementedView, this.props.style] as any}>{this.props.children}</View>
    );
  }
}

export = RecyclerViewBackedScrollView;
