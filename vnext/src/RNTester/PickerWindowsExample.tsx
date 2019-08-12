/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */

import React = require('react');
import {Button, Text, View} from 'react-native';
import {Picker} from '../index.windows';

interface MakesModels {
  name: string;
  models: string[];
}
const CAR_MAKES_AND_MODELS: {[key: string]: MakesModels} = {
  amc: {
    name: 'AMC',
    models: ['AMX', 'Concord', 'Eagle', 'Gremlin', 'Matador', 'Pacer'],
  },
  alfa: {
    name: 'Alfa-Romeo',
    models: [
      '159',
      '4C',
      'Alfasud',
      'Brera',
      'GTV6',
      'Giulia',
      'MiTo',
      'Spider',
    ],
  },
  aston: {
    name: 'Aston Martin',
    models: ['DB5', 'DB9', 'DBS', 'Rapide', 'Vanquish', 'Vantage'],
  },
  audi: {
    name: 'Audi',
    models: [
      '90',
      '4000',
      '5000',
      'A3',
      'A4',
      'A5',
      'A6',
      'A7',
      'A8',
      'Q5',
      'Q7',
    ],
  },
  austin: {
    name: 'Austin',
    models: ['America', 'Maestro', 'Maxi', 'Mini', 'Montego', 'Princess'],
  },
  borgward: {
    name: 'Borgward',
    models: ['Hansa', 'Isabella', 'P100'],
  },
  buick: {
    name: 'Buick',
    models: [
      'Electra',
      'LaCrosse',
      'LeSabre',
      'Park Avenue',
      'Regal',
      'Roadmaster',
      'Skylark',
    ],
  },
  cadillac: {
    name: 'Cadillac',
    models: ['Catera', 'Cimarron', 'Eldorado', 'Fleetwood', 'Sedan de Ville'],
  },
  chevrolet: {
    name: 'Chevrolet',
    models: [
      'Astro',
      'Aveo',
      'Bel Air',
      'Captiva',
      'Cavalier',
      'Chevelle',
      'Corvair',
      'Corvette',
      'Cruze',
      'Nova',
      'SS',
      'Vega',
      'Volt',
    ],
  },
};

class PickerExample extends React.Component<{}, any> {
  public state = {
    carMake: 'cadillac',
    modelIndex: 3,
  };

  public render() {
    const make = CAR_MAKES_AND_MODELS[this.state.carMake];
    const selectionString =
      make.name + ' ' + make.models[this.state.modelIndex];
    return (
      <View>
        <Text>Please choose a make for your car:</Text>
        <Picker
          selectedValue={this.state.carMake}
          onValueChange={(carMake: string) =>
            this.setState({carMake, modelIndex: 0})
          }>
          {Object.keys(CAR_MAKES_AND_MODELS).map(carMake => (
            <Picker.Item
              key={carMake}
              value={carMake}
              label={CAR_MAKES_AND_MODELS[carMake].name}
            />
          ))}
        </Picker>
        <Text>Please choose a model of {make.name}:</Text>
        <Picker
          selectedValue={this.state.modelIndex}
          key={this.state.carMake}
          onValueChange={(modelIndex: number) => this.setState({modelIndex})}>
          {CAR_MAKES_AND_MODELS[this.state.carMake].models.map(
            (modelName, modelIndex) => (
              <Picker.Item
                key={this.state.carMake + '_' + modelIndex}
                value={modelIndex}
                label={modelName}
              />
            ),
          )}
        </Picker>
        <Text>You selected: {selectionString}</Text>
      </View>
    );
  }
}

class PickerStyleExample extends React.Component<{}, any> {
  public state = {
    carMake: 'cadillac',
    modelIndex: 0,
  };

  public render() {
    /*
        itemStyle={{
          fontSize: 11,
          color: 'red',
          textAlign: 'left',
          fontWeight: 'bold',
        }}
      */
    return (
      <Picker
        selectedValue={this.state.carMake}
        onValueChange={(carMake: string) =>
          this.setState({carMake, modelIndex: 0})
        }>
        {Object.keys(CAR_MAKES_AND_MODELS).map(carMake => (
          <Picker.Item
            key={carMake}
            value={carMake}
            label={CAR_MAKES_AND_MODELS[carMake].name}
          />
        ))}
      </Picker>
    );
  }
}

class PickerUpdateItemsExample extends React.Component<{}, any> {
  public state = {
    selected: '111',
    items: ['111', '222', '333', '444', '555', '666'],
  };

  public render() {
    return (
      <View>
        <View style={{flexDirection: 'row', justifyContent: 'space-between'}}>
          <Button
            title="Remove first item"
            onPress={() => {
              let selected = this.state.selected;
              if (
                this.state.items.length > 1 &&
                this.state.items[0] === selected
              ) {
                selected = this.state.items[1];
              }
              this.setState({items: this.state.items.slice(1), selected});
            }}
          />
          <Button
            title="Remove last item"
            onPress={() => {
              let selected = this.state.selected;
              if (
                this.state.items.length > 1 &&
                this.state.items[this.state.items.length - 1] === selected
              ) {
                selected = this.state.items[this.state.items.length - 2];
              }
              this.setState({
                items: this.state.items.slice(0, this.state.items.length - 1),
                selected,
              });
            }}
          />
          <Button
            title="Reset"
            onPress={() =>
              this.setState({
                selected: '111',
                items: ['111', '222', '333', '444', '555', '666'],
              })
            }
          />
        </View>
        <Picker
          selectedValue={this.state.selected}
          onValueChange={(selected: string) => this.setState({selected})}>
          {this.state.items.map(item => (
            <Picker.Item key={item} value={item} label={item} />
          ))}
        </Picker>
        <Text>You selected: {this.state.selected}</Text>
      </View>
    );
  }
}

class PickerEditableExample extends React.Component<{}, any> {
  public state = {
    selected: '111',
    text: 'n',
    items: ['111', '222', '333', '444', '555', '666'],
  };

  public render() {
    return (
      <View style={{flexDirection: 'column'}}>
        <Text>You selected:{this.state.selected}</Text>
        <Text>Text change:{this.state.text}</Text>
        <Picker
          editable={true}
          selectedValue={this.state.selected}
          text={this.state.text}
          onValueChange={this.onValueChange}>
          {this.state.items.map(item => (
            <Picker.Item key={item} value={item} label={item} />
          ))}
        </Picker>
      </View>
    );
  }

  private onValueChange = (selected: string, _index: number, text: string) => {
    this.setState({selected, text});
  };
}

export const displayName = (_undefined?: string) => {};
export const title = '<Picker> UWP';
export const description =
  'Render lists of selectable options with uwp ComboBox.';
export const examples = [
  {
    title: '<Picker>',
    render: function(): JSX.Element {
      return <PickerExample />;
    },
  },
  {
    title: '<Picker> with custom styling (Not yet implemented)',
    render: function(): JSX.Element {
      return <PickerStyleExample />;
    },
  },
  {
    title: '<Picker> update items maintains selection test',
    render: function(): JSX.Element {
      return <PickerUpdateItemsExample />;
    },
  },
  {
    title: '<Picker> editable combobox example',
    render: function(): JSX.Element {
      return <PickerEditableExample />;
    },
  },
];
