
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import * as React from 'react';
import {
  AppRegistry,
  Button,
  StyleSheet,
  TextInput,
  Keyboard,
  View,
  ScrollView,
  Text,
  KeyboardAvoidingView,
  requireNativeComponent,
  FlatList
} from 'react-native';

const FocusZoneView = requireNativeComponent(
      'FocusZoneView'
    );

const DATA = [
  {
    id: 'bd7acbea-c1b1-46c2-aed5-3ad53abb28ba',
    title: 'First Item',
  },
  {
    id: '3ac68afc-c605-48d3-a4f8-fbd91aa97f63',
    title: 'Second Item',
  },
  {
    id: '58694a0f-3da1-471f-bd96-145571e29d72',
    title: 'Third Item',
  },
  {
    id: 'bd7acbea-c1b1-46c2-aed5-3ad53abb28ba',
    title: 'First Item',
  },
  {
    id: '3ac68afc-c605-48d3-a4f8-fbd91aa97f63',
    title: 'Second Item',
  },
  {
    id: '58694a0f-3da1-471f-bd96-145571e29d72',
    title: 'Third Item',
  },
  {
    id: 'bd7acbea-c1b1-46c2-aed5-3ad53abb28ba',
    title: 'First Item',
  },
  {
    id: '3ac68afc-c605-48d3-a4f8-fbd91aa97f63',
    title: 'Second Item',
  },
  {
    id: '58694a0f-3da1-471f-bd96-145571e29d72',
    title: 'Third Item',
  },
  {
    id: 'bd7acbea-c1b1-46c2-aed5-3ad53abb28ba',
    title: 'First Item',
  },
  {
    id: '3ac68afc-c605-48d3-a4f8-fbd91aa97f63',
    title: 'Second Item',
  },
  {
    id: '58694a0f-3da1-471f-bd96-145571e29d72',
    title: 'Third Item',
  },
  {
    id: '3ac68afc-c605-48d3-a4f8-fbd91aa97f63',
    title: 'Second Item',
  },
  {
    id: '58694a0f-3da1-471f-bd96-145571e29d72',
    title: 'Third Item',
  },
  {
    id: 'bd7acbea-c1b1-46c2-aed5-3ad53abb28ba',
    title: 'First Item',
  },
  {
    id: '3ac68afc-c605-48d3-a4f8-fbd91aa97f63',
    title: 'Second Item',
  },
  {
    id: '58694a0f-3da1-471f-bd96-145571e29d72',
    title: 'Third Item',
  },
  {
    id: '3ac68afc-c605-48d3-a4f8-fbd91aa97f63',
    title: 'Second Item',
  },
  {
    id: '58694a0f-3da1-471f-bd96-145571e29d72',
    title: 'Third Item',
  },
  {
    id: 'bd7acbea-c1b1-46c2-aed5-3ad53abb28ba',
    title: 'First Item',
  },
  {
    id: '3ac68afc-c605-48d3-a4f8-fbd91aa97f63',
    title: 'Second Item',
  },
  {
    id: '58694a0f-3da1-471f-bd96-145571e29d72',
    title: 'Third Item',
  },
];

function onButtonPress(buttonName) {
  Alert.alert(`${buttonName} has been pressed!`);
}
export default class Bootstrap extends React.Component<{}, any> {
  componentDidMount() {
    Keyboard.addListener('keyboardDidShow', this.keyboardDidShow);

    Keyboard.addListener('keyboardDidHide', this.keyboardDidHide);
  }

  keyboardDidShow = () => {
    console.log('keyboardDidShow');
  };

  keyboardDidHide = () => {
    console.log('keyboardDidHide');
  };

  componentWillUnmount() {
    Keyboard.removeListener('keyboardDidShow', this.keyboardDidShow);

    Keyboard.removeListener('keyboardDidHide', this.keyboardDidHide);
  }

  state = {
    passwordHidden: true,
    text: '',
  };

  onPressShowPassword = () => {
    const previousState = this.state.passwordHidden;
    this.setState({passwordHidden: !previousState});
  };


  Item = ({ title }) => (
  <View style={styles.item}>
    <Text style={styles.title}>{title}</Text>
  </View>
);

  render() {
    return (
      <>
      {/* <FocusZoneView>
      <ScrollView style={styles.container}>
        <Button
                onPress={() => onButtonPress('Simple')}
                testID="simple_button"
                
                title="Press Me"
                
              />
              <Button
                onPress={() => onButtonPress('Simple')}
                testID="simple_button"
                
                title="Press Me"
                
              />
              <Button
                onPress={() => onButtonPress('Simple')}
                testID="simple_button"
                
                title="Press Me"
                
              />
              <Button
                onPress={() => onButtonPress('Simple')}
                testID="simple_button"
                
                title="Press Me"
                
              />
              <Button
                onPress={() => onButtonPress('Simple')}
                testID="simple_button"
                
                title="Press Me"
                
              />
              <Button
                onPress={() => onButtonPress('Simple')}
                testID="simple_button"
                
                title="Press Me"
                
              />
              <Button
                onPress={() => onButtonPress('Simple')}
                testID="simple_button"
                
                title="Press Me"
                
              />
              <Button
                onPress={() => onButtonPress('Simple')}
                testID="simple_button"
                
                title="Press Me"
                
              />
              <Button
                onPress={() => onButtonPress('Simple')}
                testID="simple_button"
                title="Press Me"
              />
              <Button
                onPress={() => onButtonPress('Simple')}
                testID="simple_button"
                
                title="Press Me"
                
              />
              <Button
                onPress={() => onButtonPress('Simple')}
                testID="simple_button"
                
                title="Press Me"
                
              />
              <Button
                onPress={() => onButtonPress('Simple')}
                testID="simple_button"
                
                title="Press Me"
                
              />
              <Button
                onPress={() => onButtonPress('Simple')}
                testID="simple_button"
                
                title="Press Me"
                
              />
              <Button
                onPress={() => onButtonPress('Simple')}
                testID="simple_button"
                title="Press Me"
              />
              <Button
                onPress={() => onButtonPress('Simple')}
                testID="simple_button"
                title="Press Me"
              />
              <Button
                onPress={() => onButtonPress('Simple')}
                testID="simple_button"
                title="Press Me"
              />
              <Button
                onPress={() => onButtonPress('Simple')}
                testID="simple_button"
                title="Press Me"
              />
              <Button
                onPress={() => onButtonPress('Simple')}
                testID="simple_button"
                title="Press Me"
              />
              <Button
                onPress={() => onButtonPress('Simple')}
                testID="simple_button"
                title="Press Me"
              />
              <Button
                onPress={() => onButtonPress('Simple')}
                testID="simple_button"
                title="Press Me"
              />
              <Button
                onPress={() => onButtonPress('Simple')}
                testID="simple_button"
                title="Press Me"
              />
</ScrollView>
      </FocusZoneView> */}
      <FocusZoneView>
      <FlatList
        data={DATA}
        renderItem={({ item }) => (
          <Button title={item.title} onPress={() => onButtonPress('Simple')}></Button>
        )}
        keyExtractor={item => item.id}
        />
        </FocusZoneView>
      <View style={styles.container}>
        {/* <TextInput
          style={styles.input}
          placeholder={'MultiLine'}
          multiline={true}
        /> */}
        {/* <TextInput
          style={styles.input}
          placeholder={'ReadOnly'}
          editable={false}
        />
        <TextInput
          style={styles.input}
          placeholder={'SpellChecking Disabled'}
          spellCheck={false}
        />
        <TextInput
          style={styles.input}
          placeholder={'PlaceHolder color blue'}
          placeholderTextColor="blue"
        />
        <TextInput
          style={styles.input}
          placeholder={'contextMenuHidden'}
          contextMenuHidden={true}
        />
        <TextInput
          style={styles.input}
          caretHidden={true}
          placeholder={'caretHidden'}
        />
        <TextInput
          style={styles.input}
          keyboardType="number-pad"
          placeholder={'number-pad keyboardType'}
        />
        <TextInput
          style={styles.input}
          autoCapitalize="characters"
          placeholder={'autoCapitalize characters'}
        /> */}
        <TextInput
          style={styles.input}
          placeholder={this.state.passwordHidden ? 'Password' : 'Text'}
          autoCapitalize="none"
          secureTextEntry={this.state.passwordHidden}
          onChangeText={text => {
            this.setState({text});
          }}
          value={this.state.text}
          selectionColor="red"
          maxLength={10}
          keyboardType="numeric"
        />
        <Button
          title={
            this.state.passwordHidden
              ? 'SecureTextEntry On'
              : 'SecureTextEntry Off'
          }
          onPress={this.onPressShowPassword}
        />
      </View>
      {/* <FocusZoneView >
        <Button
                onPress={() => onButtonPress('Simple')}
                testID="simple_button"
                
                title="Press Me"
                
              />
              <Button
                onPress={() => onButtonPress('Simple')}
                testID="simple_button"
                
                title="Press Me"
                
              />
              <Button
                onPress={() => onButtonPress('Simple')}
                testID="simple_button"
                
                title="Press Me"
                
              />
              <Button
                onPress={() => onButtonPress('Simple')}
                testID="simple_button"
                
                title="Press Me"
                
              />
              <Button
                onPress={() => onButtonPress('Simple')}
                testID="simple_button"
                
                title="Press Me"
                
              />
              <Button
                onPress={() => onButtonPress('Simple')}
                testID="simple_button"
                
                title="Press Me"
                
              />
              <Button
                onPress={() => onButtonPress('Simple')}
                testID="simple_button"
                
                title="Press Me"
                
              />
              <Button
                onPress={() => onButtonPress('Simple')}
                testID="simple_button"
                
                title="Press Me"
                
              />
              <Button
                onPress={() => onButtonPress('Simple')}
                testID="simple_button"
                title="Press Me"
              />
              <Button
                onPress={() => onButtonPress('Simple')}
                testID="simple_button"
                
                title="Press Me"
                
              />
              
      </FocusZoneView> */}
      </>
    );
  }
}

const styles = StyleSheet.create({
  container: {
    //height: 250,
    //paddingTop: 350,
    flex: 1,
    flexDirection: 'column',
  },
  input: {
    margin: 5,
    width: 700,
    borderColor: '#7a42f4',
    borderWidth: 1,
    fontSize: 15,
  },
});

AppRegistry.registerComponent('Bootstrap', () => Bootstrap);
