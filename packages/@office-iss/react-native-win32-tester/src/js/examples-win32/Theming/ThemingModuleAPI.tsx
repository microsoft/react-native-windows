'use strict';
import React from 'react'
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

const ifModuleAvailable = (wrappedComponent: React.ReactElement): React.ReactElement => {
  return Theming ? wrappedComponent : <Text>Theming Native Module not available</Text>;
};

const RenderThemeFunction = (): React.ReactElement => {
  const [, setText] = React.useState('');
  const onChangeText = React.useCallback((t: string) => setText(t), [setText]);
  return (
    <View style={styles.nestedContainer}>
      <Text>Arguments: </Text>
      <TextInput onChangeText={onChangeText} />
    </View>
  );
};

const withBox = (
  key: string,
  component: React.ReactElement | React.ReactElement[]
): React.ReactElement => (
  <View style={styles.box} key={key}>
    <Text style={styles.header}>{key}</Text>
    <>{component}</>
  </View>
);

const ThemingMethods: React.FunctionComponent = () => {
  return (
    <View>
      <>
        {Object.keys(Theming).map((val: string) =>
          typeof Theming[val] === 'function' ? withBox(val, <RenderThemeFunction />) : null
        )}
      </>
    </View>
  );
};

const renderNestedObject = (obj: Record<string, any>): React.ReactElement => {
  return (
    <View style={styles.nestedContainer}>
      <ScrollView>
        {Object.keys(obj).map((val: string) => (
          <Text key={val}>{val + ': ' + JSON.stringify(obj[val])}</Text>
        ))}
      </ScrollView>
    </View>
  );
};

const renderObject = (obj: Record<string, any>): React.ReactElement => {
  const firstKey = Object.keys(obj)[0];
  const formattedOutput = JSON.stringify(obj)
    .replace(/],/g, '],\n\n')
    .replace(/:/g, ': ')
    .replace(/,/g, ', ');

  return obj[firstKey] instanceof Array ? (
    <Text style={styles.nestedContainer}>{formattedOutput}</Text>
  ) : (
    renderNestedObject(obj)
  );
};

const renderThemeObject = (key: string): React.ReactElement =>
  withBox(key, renderObject(Theming[key]));

const ThemingConstants: React.FunctionComponent = () => {
  return (
    <View>
      <>
        {Object.keys(Theming).map((val: string) =>
          typeof Theming[val] === 'object' ? renderThemeObject(val) : null
        )}
      </>
    </View>
  );
};

export const title = 'Theming Module APIs';
export const displayName =  'Theming Module APIs';
export const description = 'Tests shape of Theming Native Module';
export const examples: {
  title: string;
  description: string;
  render: () => React.ReactElement;
}[] = [
  {
    title: 'Theming Module Constants',
    description: 'All constants',
    render: (): React.ReactElement => ifModuleAvailable(<ThemingConstants />),
  },
  {
    title: 'Theming Module Methods',
    description: 'Method invoker',
    render: (): React.ReactElement => ifModuleAvailable(<ThemingMethods />),
  },
];
