// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

import React, { Component } from 'react';
import {
  AppRegistry,
  StyleSheet,
  Text,
  ScrollView,
  FlatList,
  Image,
  View,
} from 'react-native';

const styles = StyleSheet.create({
  container: {
    flex: 1,
    backgroundColor: '#dddddd',
  },
  message: {
    flex: 1,
    flexDirection: 'row',
    alignItems: 'flex-start',
    marginLeft: 20,
    marginTop: 10,
    marginRight: 20,
    marginBottom: 10,
  },
  messageAvatar: {
    width: 50,
    height: 50,
    marginRight: 10,
  },
  messageContents: {
    backgroundColor: '#ffffff',
    flexDirection: 'column',
    paddingLeft: 20,
    paddingTop: 10,
    paddingRight: 20,
    paddingBottom: 10,
  },
  messageHeader: {
    flexDirection: 'row',
    alignItems: 'flex-start',
  },
  messageUserName: {
    color: '#333333',
    fontWeight: 'bold',
    fontSize: 12,
  },
  messageTime: {
    color: '#666666',
    fontWeight: 'bold',
    marginLeft: 10,
    fontSize: 12,
  },
  messageText: {
    fontSize: 12,
  }
});

var TextValues = [
  "What time did the man go to the dentist?",
  "I don't know.",
  "Tooth hurt-y.",
  "Did you hear about the guy who invented Lifesavers?",
  "No.",
  "They say he made a mint.",
  "A ham sandwich walks into a bar and orders a beer. Bartender says, 'Sorry we don't serve food here.'",
  "Whenever the cashier at the grocery store asks my dad if he would like the milk in a bag he replies, 'No, just leave it in the carton!'",
  "Why do chicken coops only have two doors?",
  "I don't know, why?",
  "Because if they had four, they would be chicken sedans!",
];

var UserNameValues = [
  "Parent",
  "Child",
];

var AvatarValues = [
  require("./images/blueuser.png"),
  require("./images/reduser.png"),
];

class Message {
  constructor() {
    this.Text = "";
    this.UserName = "";
    this.Avatar = null;
    this.Timestamp = null;
    this.Time = "";
  }
}

function GetValue(index, values) {
  return values[index % values.length];
}

function CreateMessage(id) {
  let m = new Message();
  m.Text = GetValue(id, TextValues);
  m.UserName = GetValue(id, UserNameValues);
  m.Avatar = GetValue(id, AvatarValues);
  m.Timestamp = new Date(new Date().getTime() + id * 60000);
  m.Time = '' + m.Timestamp.getHours() % 12 + ':' + ('0' + m.Timestamp.getMinutes()).slice(-2) + ' ' + (m.Timestamp.getHours() > 12 ? 'PM' : 'AM' );
  return m;
}

function LoadMessages(count) {
  let messages = [];
  for (var i = 0; i < count; i++) {
    messages[i] = CreateMessage(i);
  }
  return messages;
}

var messages = LoadMessages(1000);

class MessageView extends Component {
  render() {
    return (
      <View style={styles.message}>
        <Image style={styles.messageAvatar} source={this.props.message.Avatar} />
        <View style={styles.messageContents}>
          <View style={styles.messageHeader}>
            <Text style={styles.messageUserName}>{this.props.message.UserName}</Text>
            <Text style={styles.messageTime}>{this.props.message.Time}</Text>
          </View>
          <Text style={styles.messageText}>{this.props.message.Text}</Text>
        </View>
      </View>
    );
  }
};

class App extends Component {
  render() {
    return (
      <View style={styles.container}>
        <ScrollView>
          <FlatList
            data={messages}
            renderItem={({item}) => <MessageView message={item} />}
            keyExtractor={(item, index) => index.toString()}
          />
        </ScrollView>
      </View>
    );
  }
};

AppRegistry.registerComponent('PerfCompareRN', () => App);
