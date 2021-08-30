/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 */

import * as React from 'react'
import {
  Text,
  View,
} from 'react-native';
import {
  TREE_DUMP_RESULT
} from './Consts';
import { TreeDumpControl } from './TreeDumpControl';

export function TextBackgroundColorTestPage() {
  return (
    <View>
      <View testID={'TextColorBackgroundView'}>
<Text style={{color: 'pink'}}>Outer pink</Text>
         <Text style={{}}>
           Outer <Text style={{color: 'red'}}>red nested</Text>
         </Text>
         <Text style={{}}>
           Outer{' '}
           <Text style={{backgroundColor: 'blue', color: 'white'}}>
             nested white on blue
           </Text>
         </Text>
         <Text style={{color: 'pink'}}>
           Outer pink <Text style={{color: 'red'}}>nested red</Text>
         </Text>
         <Text style={{backgroundColor: 'green'}}>
           Outer on green{' '}
           <Text style={{color: 'white'}}>nested white on inherit green</Text>
         </Text>
         <Text style={{backgroundColor: 'green', color: 'orange'}}>
           Outer orange on green{' '}
           <Text style={{backgroundColor: 'blue', color: 'white'}}>
             nested white on blue
           </Text>
         </Text>
         <Text style={{color: 'orange'}}>
           Outer orange{' '}
           <Text style={{backgroundColor: 'blue', color: 'white'}}>
             nested white on blue
           </Text>
         </Text>
         <Text style={{color: 'orange'}}>
           <Text style={{backgroundColor: 'blue'}}>
             nested orange inherit on blue
           </Text>
         </Text>
        <Text>
          Outer no_color{' '}
          <Text style={{ backgroundColor: 'green' }}>
            START_NESTED green{' '}
            <Text style={{ backgroundColor: 'blue' }}>DEEPER_NESTED blue</Text>{' '}
          END_NESTED
        </Text>{' '}
        attributes.
      </Text>
        <Text>
          Outer no_color{' '}
          <Text>
            START_NESTED no_color{' '}
            <Text style={{ backgroundColor: 'blue' }}>DEEPER_NESTED blue</Text>{' '}
          END_NESTED
        </Text>{' '}
        attributes.
      </Text>
        <Text>
          Outer no_color{' '}
          <Text style={{ backgroundColor: 'green' }}>
            START_NESTED green <Text>DEEPER_NESTED inherit green</Text>{' '}
          END_NESTED
        </Text>{' '}
        attributes.
      </Text>
        <Text style={{ backgroundColor: 'red' }}>
          Outer red{' '}
          <Text>
            START_NESTED inherit red <Text>DEEPER_NESTED inherit red</Text>{' '}
          END_NESTED
        </Text>{' '}
        attributes.
      </Text>
        <Text style={{ backgroundColor: 'red' }}>
          Outer red{' '}
          <Text style={{ backgroundColor: 'green' }}>
            START_NESTED green{' '}
            <Text style={{ backgroundColor: 'blue' }}>DEEPER_NESTED blue</Text>{' '}
          END_NESTED
        </Text>{' '}
        attributes.
      </Text>
      </View>

      <TreeDumpControl
        style={{
          height: 150,
          width: 500,
          margin: 10,
        }}
        dumpID={'TextColorBackground'}
        uiaID={'TextColorBackgroundView'}
        testID={TREE_DUMP_RESULT}
        additionalProperties={['TextHighlighters']}
      />
    </View>
  );
}

export const displayName = (_undefined?: string) => { };
export const title = '<LegacyTextBackgroundColorTest>';
export const description = 'Legacy e2e test for Text with backgroundColor';
export const examples = [
  {
    render: function (): JSX.Element {
      return <TextBackgroundColorTestPage />;
    },
  },
];
