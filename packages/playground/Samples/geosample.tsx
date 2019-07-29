/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */

import * as React from 'react';
import {
  AppRegistry,
  StyleSheet,
  Text,
  TouchableWithoutFeedback,
  View,
} from 'react-native';

// navigator is a browser polyfill defined by react-native
declare const navigator: any;
type GeoPos = {
  coords: {latitude: string; longitude: string};
  timestamp: string;
};

export default class Bootstrap extends React.Component<
  {},
  {
    lat: string;
    long: string;
    timestamp: string;
    lastCmd: string;
    lastError: string;
  }
> {
  constructor(props: {}) {
    super(props);
    this.state = {
      lat: '?',
      long: '?',
      timestamp: '?',
      lastCmd: '?',
      lastError: '?',
    };

    this.watchID = -1;
  }

  watchID: number;

  ResetState() {
    this.setState({
      lat: '?',
      long: '?',
      timestamp: '?',
      lastError: '?',
    });
  }

  OnRequestAuth() {
    // Instead of importing the Geolocation module, use browser polyfill
    navigator.geolocation.requestAuthorization();

    this.ResetState();
    this.setState({lastCmd: 'RequestAuth'});
  }

  OnGetCurrentPos() {
    // Instead of importing the Geolocation module, use browser polyfill
    navigator.geolocation.getCurrentPosition(
      (result: GeoPos) => {
        this.OnPosChanged(result);
      },
      (error: any) => {
        this.setState({lastError: error});
      },
    );

    this.ResetState();
    this.setState({lastCmd: 'GetCurrentPos'});
  }

  OnStartWatching() {
    this.watchID = navigator.geolocation.watchPosition((pos: GeoPos) => {
      this.OnPosChanged(pos);
    });
    this.ResetState();
    this.setState({lastCmd: 'StartWatching'});
  }

  OnStopWatching() {
    if (this.watchID !== -1) {
      navigator.geolocation.clearWatch(this.watchID);
    }
    this.watchID = -1;
    this.ResetState();
    this.setState({lastCmd: 'StopWatching'});
  }

  OnPosChanged(pos: {
    coords: {latitude: string; longitude: string};
    timestamp: string;
  }) {
    this.setState({
      lat: pos.coords.latitude,
      long: pos.coords.longitude,
      timestamp: pos.timestamp,
    });
  }

  render() {
    return (
      <View style={styles.container}>
        <TouchableWithoutFeedback onPress={() => this.OnRequestAuth()}>
          <View style={styles.fauxbutton}>
            <Text style={{textAlign: 'center'}}>Request Auth</Text>
          </View>
        </TouchableWithoutFeedback>
        <TouchableWithoutFeedback onPress={() => this.OnGetCurrentPos()}>
          <View style={styles.fauxbutton}>
            <Text style={{textAlign: 'center'}}>GetCurrentPosition</Text>
          </View>
        </TouchableWithoutFeedback>
        <TouchableWithoutFeedback onPress={() => this.OnStartWatching()}>
          <View style={styles.fauxbutton}>
            <Text style={{textAlign: 'center'}}>StartWatching</Text>
          </View>
        </TouchableWithoutFeedback>
        <TouchableWithoutFeedback onPress={() => this.OnStopWatching()}>
          <View style={styles.fauxbutton}>
            <Text style={{textAlign: 'center'}}>StopWatching</Text>
          </View>
        </TouchableWithoutFeedback>
        <Text style={styles.stuff}>Lat: {this.state.lat}</Text>
        <Text style={styles.stuff}>Long: {this.state.long}</Text>
        <Text style={styles.stuff}>timestamp: {this.state.timestamp}</Text>
        <Text style={styles.stuff}>lastcmd: {this.state.lastCmd}</Text>
        <Text style={styles.stuff}>lastError: {this.state.lastError}</Text>
      </View>
    );
  }
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    justifyContent: 'center',
    alignItems: 'center',
    backgroundColor: '#F5FCFF',
  },
  stuff: {
    textAlign: 'center',
    color: '#333333',
    width: 500,
  },
  fauxbutton: {
    backgroundColor: 'red',
    width: 150,
    borderWidth: 1,
    borderColor: 'black',
  },
});

AppRegistry.registerComponent('Bootstrap', () => Bootstrap);
