import * as React from 'react';
import { Picker } from  'react-native';

export const title = '<PickerWin32Test>';
export const displayName = 'Picker Win32 example';
export const description = 'Show the Picker component with a set of options.';
export const examples = [
    {
      title: 'Win32 Picker control test',
      description: 'Simple scenario',
      render(): JSX.Element {        
        return (
          <Picker prompt="Select the state you want to live in:">
              <Picker.Item label='Alibama' value='A' />
              <Picker.Item label='California' value='C' />
              <Picker.Item label='Delware' value='D' />
              <Picker.Item label='Florida' value='F' />
              <Picker.Item label='Hawaii' value='H' />
              <Picker.Item label='New York' value='N' />
              <Picker.Item label='Washington' value='W' />
          </Picker>
        );
      },
    },
  ];
