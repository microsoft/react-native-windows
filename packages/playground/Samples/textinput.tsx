/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */

import React, {Component} from 'react';
import {AppRegistry, Button, StyleSheet, Text, TextInput, View} from 'react-native';

export default class Bootstrap extends Component {
   state = {
      passwordHidden: true,
      text: ""
   }

   onPressShowPassword = () => {
      var previousState = this.state.passwordHidden;
      this.setState({ passwordHidden: !previousState })
   }
  
   render() {
      return (
         <View style = {styles.container}>
            <TextInput style = {styles.input}
               placeholder = {"MultiLine"}
               multiline = {true} />
            <TextInput style = {styles.input}
               placeholder = {"ReadOnly"}
               editable = {false} />
            <TextInput style = {styles.input}
               placeholder = {"SpellChecking Disabled"}
               spellCheck = {false} />
            <TextInput style = {styles.input}
               placeholder = {"PlaceHolder color blue"}
               placeholderTextColor = "blue"/>
            <TextInput style = {styles.input}
               placeholder = {this.state.passwordHidden?"Password":"Text"}
               autoCapitalize = "none"
               secureTextEntry={this.state.passwordHidden}
               onChangeText={(text) => {this.setState({text}); }}
               value={this.state.text}
               selectionColor = "red"
               maxLength = {10} />
            <Button style = {styles.button} title= {this.state.passwordHidden?"SecureTextEntry On":"SecureTextEntry Off"} onPress={this.onPressShowPassword}/>
         </View>
      )
   }
}

const styles = StyleSheet.create({
   container: {
      paddingTop: 23,
      flex: 1,
      flexDirection: 'column',
      alignItems: 'center',
      justifyContent: 'center',
   },
   input: {
      margin: 15,
      height: 80,
      width: 700,
      borderColor: '#7a42f4',
      borderWidth: 1,
      fontSize: 40,
   },
   button: {
      margin: 15,
      height: 25,
      width: 75,
   },
});

AppRegistry.registerComponent('Bootstrap', () => Bootstrap);
