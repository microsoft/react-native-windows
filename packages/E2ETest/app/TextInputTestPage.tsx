/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */

import React from 'react';
import {Text, TextInput, View} from 'react-native';
import {TEXTINPUT_ON_TEXTINPUT, ML_TEXTINPUT_ON_TEXTINPUT, CAP_TEXTINPUT_ON_TEXTINPUT} from './Consts';

interface ITextInputTestPageState {
  curText: string;
  prevText: string;
  prev2Text: string;
  prev3Text: string;
}

export class TextInputTestPage extends React.Component<
  {},
  ITextInputTestPageState
> {
  public state = {
    curText: '<No Event>',
    prevText: '<No Event>',
    prev2Text: '<No Event>',
    prev3Text: '<No Event>',
  };

  public updateText = (text: string) => {
    this.setState(state => {
      return {
        curText: text,
        prevText: state.curText,
        prev2Text: state.prevText,
        prev3Text: state.prev2Text,
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
        <View style={{flexDirection: 'row', justifyContent: 'space-between'}}>
          <Text testID="CurText">curText: {this.state.curText}</Text>
          <Text testID="PrevText">prev: {this.state.prevText}</Text>
          <Text testID="Prev2Text">prev2: {this.state.prev2Text})</Text>
          <Text testID="Prev3Text">prev3: {this.state.prev3Text}</Text>
        </View>
      </View>
    );
  }
}
