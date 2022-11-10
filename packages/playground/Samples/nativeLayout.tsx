/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */
import React from 'react';
import {AppRegistry, View, Text, requireNativeComponent} from 'react-native';

const GridView: any = requireNativeComponent('PlaygroundGridView');
const GridItemView: any = requireNativeComponent('PlaygroundGridItemView');

function Bootstrap() {
  return (
    <GridView
      style={{width: 200, height: 200}}
      columns={['*', '*']}
      rows={['*', '*']}>
      <GridItemView gridRow={0} gridColumn={0}>
        <View
          style={{
            backgroundColor: '#F25022',
            justifyContent: 'center',
            alignItems: 'center',
          }}>
          <Text>1</Text>
        </View>
      </GridItemView>
      <GridItemView gridRow={0} gridColumn={1}>
        <View
          style={{
            backgroundColor: '#7FBA00',
            justifyContent: 'center',
            alignItems: 'center',
          }}>
          <Text>2</Text>
        </View>
      </GridItemView>
      <GridItemView gridRow={1} gridColumn={0}>
        <View
          style={{
            backgroundColor: '#00A4EF',
            justifyContent: 'center',
            alignItems: 'center',
          }}>
          <Text>3</Text>
        </View>
      </GridItemView>
      <GridItemView gridRow={1} gridColumn={1}>
        <View
          style={{
            backgroundColor: '#FFB900',
            justifyContent: 'center',
            alignItems: 'center',
          }}>
          <Text>4</Text>
        </View>
      </GridItemView>
    </GridView>
  );
}

AppRegistry.registerComponent('Bootstrap', () => Bootstrap);
