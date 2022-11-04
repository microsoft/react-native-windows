/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */
import React from 'react';
import {AppRegistry, requireNativeComponent} from 'react-native';

const GridView: any = requireNativeComponent('PlaygroundGridView');
const GridItemView: any = requireNativeComponent('PlaygroundGridItemView');

function Bootstrap() {
  return (
    <GridView
      style={{width: 200, height: 200}}
      columns={['*', '*']}
      rows={['*', '*']}>
      <GridItemView gridRow={0} gridColumn={0} style={{backgroundColor: 'red'}} />
      <GridItemView gridRow={0} gridColumn={1} style={{backgroundColor: 'green'}} />
      <GridItemView gridRow={1} gridColumn={0} style={{backgroundColor: 'blue'}} />
      <GridItemView gridRow={1} gridColumn={1} style={{backgroundColor: 'yellow'}} />
    </GridView>
  );
}

AppRegistry.registerComponent('Bootstrap', () => Bootstrap);
