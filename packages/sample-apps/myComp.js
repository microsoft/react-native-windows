import React, { Component } from 'react';
import {
  StyleSheet,
  Text,
  View,
} from 'react-native';


class MyComp extends Component {

    render() {
        return (
        <View style={styles.container}>
          <Text style={styles.welcome}>
            My Component!
          </Text>
        </View>
      );
    }
  }


  const styles = StyleSheet.create({
    container: {
      flex: 0,
      justifyContent: 'center',
      alignItems: 'center',
      backgroundColor: '#F5FCFF',
    },
    welcome: {
      fontSize: 20,
      textAlign: 'center',
      color: 'green',
      margin: 10,
    },
});

module.exports = {
    MyComp,
};
