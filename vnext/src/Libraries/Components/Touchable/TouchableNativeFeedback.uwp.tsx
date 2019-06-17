// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
'use strict';

import * as React from 'react';
import {
  StyleSheet,
  View,
  Text,
  TouchableNativeFeedbackProps
} from 'react-native';

const styles = StyleSheet.create({
  container: {
    height: 100,
    width: 300,
    backgroundColor: '#ffbcbc',
    borderWidth: 1,
    borderColor: 'red',
    alignItems: 'center',
    justifyContent: 'center',
    margin: 10
  },
  info: {
    color: '#333333',
    margin: 20
  }
});

class DummyTouchableNativeFeedback extends React.Component<
  TouchableNativeFeedbackProps
> {
  public static SelectableBackground = () => ({});
  public static SelectableBackgroundBorderless = () => ({});
  public static Ripple = () => ({});
  public static canUseNativeForeground = () => false;

  public render(): JSX.Element {
    return (
      <View style={[styles.container, this.props.style]}>
        <Text style={styles.info}>
          TouchableNativeFeedback is not supported on this platform!
        </Text>
      </View>
    );
  }
}

export default DummyTouchableNativeFeedback;
