'use strict';
import * as React from 'react';
import { View, Text, StyleSheet, TextInput, NativeModules, ScrollView } from 'react-native';

const styles = StyleSheet.create({
  box: {
    borderWidth: 2,
    flexDirection: 'column',
    marginVertical: 5,
    backgroundColor: 'whitesmoke',
    justifyContent: 'space-around',
    padding: 5,
    maxHeight: 250,
  },
  header: {
    fontSize: 13,
    margin: 6,
  },
  nestedContainer: {
    margin: 6,
    padding: 6,
  },
});

const Theming = NativeModules.Theming;

const ifModuleAvailable = (wrappedComponent: JSX.Element) => {
  return Theming ? wrappedComponent : <Text>Theming Native Module not available</Text>;
};

const RenderThemeFunction = () => {
  const [, setText] = React.useState('');
  const onChangeText = React.useCallback(t => setText(t), [setText]);
  return (
    <View style={styles.nestedContainer}>
      <Text>Arguments: </Text>
      <TextInput onChangeText={onChangeText} />
    </View>
  );
};

const ThemingMethods: React.FunctionComponent<{}> = () => {
  return (
    <View>
      {Object.keys(Theming).map((val: string) => {
        return typeof Theming[val] === 'function' ? withBox(val, RenderThemeFunction()) : undefined;
      })}
    </View>
  );
};

const renderNestedObject = (obj: Record<string, any>) => {
  return (
    <View style={styles.nestedContainer}>
      <ScrollView>
        {Object.keys(obj).map((val: string) => {
          return <Text key={val}>{val + ': ' + JSON.stringify(obj[val])}</Text>;
        })}
      </ScrollView>
    </View>
  );
};

const renderObject = (obj: Record<string, any>) => {
  const firstKey = Object.keys(obj)[0];
  const formattedOutput = JSON.stringify(obj)
    .replace(/],/g, '],\n\n')
    .replace(/:/g, ': ')
    .replace(/,/g, ', ');
  return obj[firstKey] instanceof Array ? <Text style={styles.nestedContainer}>{formattedOutput}</Text> : renderNestedObject(obj);
};

const renderThemeObject = (key: string): React.ReactElement => withBox(key, renderObject(Theming[key]));

const withBox = (key: string, component: React.Component | JSX.Element) => (
  <View style={styles.box} key={key}>
    <Text style={styles.header}>{key}</Text>
    {component}
  </View>
);

const ThemingConstants: React.FunctionComponent<{}> = () => {
  return (
    <View>
      {Object.keys(Theming).map((val: string) => {
        return typeof Theming[val] === 'object' ? renderThemeObject(val) : undefined;
      })}
    </View>
  );
};

export const title = 'Theming Module APIs';
export const displayName =  'Theming Module APIs';
export const description = 'Tests shape of Theming Native Module';
export const examples = [
    {
      title: 'Theming Module Constants',
      description: 'All constants',
      render: () => ifModuleAvailable(<ThemingConstants />),
    },
    {
      title: 'Theming Module Methods',
      description: 'Method invoker',
      render: () => ifModuleAvailable(<ThemingMethods />),
    },
  ];
