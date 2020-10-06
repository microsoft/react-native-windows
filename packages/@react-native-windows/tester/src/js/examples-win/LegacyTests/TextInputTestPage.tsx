/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import * as React from 'react';
import {Text, TextInput, View} from 'react-native';
import {
  TEXTINPUT_ON_TEXTINPUT,
  ML_TEXTINPUT_ON_TEXTINPUT,
  CURTEXT_ON_TEXTINPUT,
  CAP_TEXTINPUT_ON_TEXTINPUT,
} from './Consts';

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
          testID={TEXTINPUT_ON_TEXTINPUT}
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
          testID={ML_TEXTINPUT_ON_TEXTINPUT}
          style={{height: 80}}
          placeholder="MultiLine"
          multiline={true}
        />
        <TextInput
          testID={CAP_TEXTINPUT_ON_TEXTINPUT}
          style={{height: 80}}
          placeholder="autoCapitalize"
          autoCapitalize="characters"
        />
        <Text testID={CURTEXT_ON_TEXTINPUT}>{this.state.log}</Text>
      </View>
    );
  }
}

export const displayName = (_undefined?: string) => {};
export const title = '<LegacyTextInputTest>';
export const description = 'Legacy e2e test for TextInput';
export const examples = [
  {
    render: function(): JSX.Element {
      return <TextInputTestPage />;
    },
  },
];
