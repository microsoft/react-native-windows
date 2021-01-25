/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import * as React from 'react';
import {Text, TextInput, View} from 'react-native';

interface ITextInputTestPageState {
  log: string;
}

export class TextInputTestPage extends React.Component<
  {},
  ITextInputTestPageState
> {
  public state = {
    log: '<Log Start>',
  };

  public updateText = (text: string) => {
    this.setState(state => {
      return {
        log: text + '\n' + state.log,
      };
    });
  };

  public render() {
    return (
      <View>
        <TextInput
          style={{height: 32}}
          testID="textinput-field"
          placeholder="Enter text to see events"
          onFocus={() => this.updateText('onFocus')}
          onBlur={() => this.updateText('onBlur')}
          onChange={event =>
            this.updateText('onChange text: ' + event.nativeEvent.text)
          }
          onEndEditing={event =>
            this.updateText('onEndEditing text: ' + event.nativeEvent.text)
          }
          onSubmitEditing={event =>
            this.updateText('onSubmitEditing text: ' + event.nativeEvent.text)
          }
          onSelectionChange={event => {
            this.updateText(
              'onSelectionChange range: ' +
                event.nativeEvent.selection.start +
                ',' +
                event.nativeEvent.selection.end,
            );
          }}
          onKeyPress={event => {
            this.updateText('onKeyPress key: ' + event.nativeEvent.key);
          }}
        />
        <TextInput
          testID="multi-line-textinput-field"
          style={{height: 80}}
          placeholder="MultiLine"
          multiline={true}
        />
        <TextInput
          testID="auto-caps-textinput-field"
          style={{height: 80}}
          placeholder="autoCapitalize"
          autoCapitalize="characters"
        />
        <Text testID="textinput-log">{this.state.log}</Text>
      </View>
    );
  }
}

export const displayName = (_undefined?: string) => {};
export const title = 'LegacyTextInputTest';
export const description = 'Legacy e2e test for TextInput';
export const examples = [
  {
    render: function(): JSX.Element {
      return <TextInputTestPage />;
    },
  },
];
