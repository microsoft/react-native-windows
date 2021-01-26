/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 */

import * as React from 'react'
import {
  Text,
  View,
} from 'react-native';

import { TreeDumpControl } from './TreeDumpControl';

export function TextBackgroundColorTestPage() {
  return (
    <View>
      <View testID={'text-color-background-view'}>
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
        uiaID={'text-color-background-view'}
        additionalProperties={['TextHighlighters']}
      />
    </View>
  );
}

export const displayName = (_undefined?: string) => { };
export const title = 'LegacyTextBackgroundColorTest';
export const description = 'Legacy e2e test for Text with backgroundColor';
export const examples = [
  {
    render: function (): JSX.Element {
      return <TextBackgroundColorTestPage />;
    },
  },
];