/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */
import React, {useState} from 'react';
import {AppRegistry, View, Text, requireNativeComponent} from 'react-native';

const GridView: any = requireNativeComponent('PlaygroundGridView');
const GridItemView: any = requireNativeComponent('PlaygroundGridItemView');

function Bootstrap() {
  const [hover1, setHover1] = useState(false);
  const [hover2, setHover2] = useState(false);
  const [hover3, setHover3] = useState(false);
  const [hover4, setHover4] = useState(false);

  const getHoverProps = (setter: (value: boolean) => void) => {
    return {
      onMouseEnter: () => setter(true),
      onMouseLeave: () => setter(false),
    };
  };

  return (
    <GridView
      style={{width: 200, height: 200}}
      columns={['*', '*']}
      rows={['*', '*']}>
      <GridItemView gridRow={0} gridColumn={0}>
        <View
          {...getHoverProps(setHover1)}
          style={{
            backgroundColor: '#F25022',
            justifyContent: 'center',
            alignItems: 'center',
          }}>
          <Text style={{fontSize: hover1 ? 20 : undefined}}>1</Text>
        </View>
      </GridItemView>
      <GridItemView gridRow={0} gridColumn={1}>
        <View
          {...getHoverProps(setHover2)}
          style={{
            backgroundColor: '#7FBA00',
            justifyContent: 'center',
            alignItems: 'center',
          }}>
          <Text style={{fontSize: hover2 ? 20 : undefined}}>2</Text>
        </View>
      </GridItemView>
      <GridItemView gridRow={1} gridColumn={0}>
        <View
          {...getHoverProps(setHover3)}
          style={{
            backgroundColor: '#00A4EF',
            justifyContent: 'center',
            alignItems: 'center',
          }}>
          <Text style={{fontSize: hover3 ? 20 : undefined}}>3</Text>
        </View>
      </GridItemView>
      <GridItemView gridRow={1} gridColumn={1}>
        <View
          {...getHoverProps(setHover4)}
          style={{
            backgroundColor: '#FFB900',
            justifyContent: 'center',
            alignItems: 'center',
          }}>
          <Text style={{fontSize: hover4 ? 20 : undefined}}>4</Text>
        </View>
      </GridItemView>
    </GridView>
  );
}

AppRegistry.registerComponent('Bootstrap', () => Bootstrap);
