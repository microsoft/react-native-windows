/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 */

import { StyleSheet, View, Image, Button } from 'react-native';
import * as React from 'react'
import { useState } from 'react';

const styles = StyleSheet.create({
  container: {
    height: 300,
    width: 500,
    backgroundColor: 'yellow',
    alignItems: 'center',
  },
  containerWithBorder: {
    height: 300,
    width: 500,
    borderRadius: 10.0,
    borderWidth: 10,
    borderColor: '#00ff0055',
    backgroundColor: 'yellow',
    alignItems: 'center',
  },
  imageWithBorder: {
    height: '100%',
    width: '100%',
    borderRadius: 10.0,
    borderWidth: 10,
    borderColor: '#0000ff55',
    backgroundColor: 'red',
  },
  image: {
    height: '100%',
    width: '100%',
    backgroundColor: 'red',
  },
  rtlImage: {
    height: '100%',
    width: '100%',
    backgroundColor: 'red',
    direction: 'rtl',
  },
});

export function ImageTestPage() {
  const [imageWithBorder, setImageBorder] = useState(false);
  const [rltMode, setRtlMode] = useState(false);
  const [clickCount, setClickCount] = useState(0);
  const onPressBorder = () => {
    const previousImageBorderState = imageWithBorder;
    setImageBorder(!previousImageBorderState);
    const previousClickCount = clickCount;
    setClickCount(previousClickCount + 1);
  };

  const imageStyle = () => {
    if (rltMode) {
      return styles.rtlImage;
    }
    if (imageWithBorder) {
      return styles.imageWithBorder;
    }
    return styles.image;
  };

  return (
    <View>
      <View
        testID="image-container"
        style={imageWithBorder ? styles.containerWithBorder : styles.container}
      >
        <Image
          style={imageStyle()}
          resizeMode={'center'}
          source={{
            uri:
              'data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAADMAAAAzCAYAAAA6oTAqAAAAEXRFWHRTb2Z0d2FyZQBwbmdjcnVzaEB1SfMAAABQSURBVGje7dSxCQBACARB+2/ab8BEeQNhFi6WSYzYLYudDQYGBgYGBgYGBgYGBgYGBgZmcvDqYGBgmhivGQYGBgYGBgYGBgYGBgYGBgbmQw+P/eMrC5UTVAAAAABJRU5ErkJggg==',
          }}
        />
      </View>
      <Button
        title={imageWithBorder ? 'Hide Border' : 'Show Border'}
        onPress={onPressBorder}
        testID="toggle-border-button"
      />
      <Button
        title={rltMode ? 'Set image to LTR' : 'Set image to RTL'}
        onPress={() => setRtlMode(!rltMode)}
        testID="set-rtl-button"
      />
    </View>
  );
}

export const displayName = (_undefined?: string) => {};
export const title = 'LegacyImageTest';
export const description = 'Legacy e2e test for Image';
export const examples = [
  {
    render: function(): JSX.Element {
      return <ImageTestPage />;
    },
  },
];