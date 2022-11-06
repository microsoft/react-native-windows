/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @flow
 * @format
 */
'use strict';

const React = require('react');

const {StyleSheet, Text, TextInput, View} = require('react-native');

/**
 * See https://www.w3schools.com/php/php_form_validation.asp
 */
class HTTPExampleMultiPartFormData extends React.Component<any, any> {
  responseURL: ?string;
  responseHeaders: ?Object;

  constructor(props: any) {
    super(props);
    this.state = {
      responseText: null,
    };
    this.responseURL = null;
    this.responseHeaders = null;
  }

  submit(uri: string) {
    const formData = new FormData();

    formData.append('name', {
      string: 'Name',
      type: 'application/text',
    });
    formData.append('email', {
      string: 'me@mail.com',
      type: 'application/text',
    });
    formData.append('website', {
      string: 'http://aweb.com',
      type: 'application/text',
    });
    formData.append('comment', {
      string: 'Hello',
      type: 'application/text',
    });
    formData.append('gender', {
      string: 'Other',
      type: 'application/text',
    });

    fetch(uri, {
      method: 'POST',
      headers: {
        'Content-Type': 'multipart/form-data',
      },
      body: formData,
    })
      .then(response => {
        this.responseURL = response.url;
        this.responseHeaders = response.headers;

        return response.text();
      })
      .then(body => {
        this.setState({responseText: body});
      });
  }

  _renderHeaders(): null | Array<React.Node> {
    if (!this.responseHeaders) {
      return null;
    }

    const responseHeaders = [];
    const keys = Object.keys(this.responseHeaders.map);
    for (let i = 0; i < keys.length; i++) {
      const key = keys[i];
      // $FlowFixMe[incompatible-use]
      const value = this.responseHeaders.get(key);
      responseHeaders.push(
        <Text>
          {key}: {value}
        </Text>,
      );
    }
    return responseHeaders;
  }

  render(): React.Node {
    const responseURL = this.responseURL ? (
      <View style={{marginTop: 10}}>
        <Text style={styles.label}>Server response URL:</Text>
        <Text>{this.responseURL}</Text>
      </View>
    ) : null;

    const responseHeaders = this.responseHeaders ? (
      <View style={{marginTop: 10}}>
        <Text style={styles.label}>Server response headers:</Text>
        {this._renderHeaders()}
      </View>
    ) : null;

    const response = this.state.responseText ? (
      <View style={{marginTop: 10}}>
        <Text style={styles.label}>Server response:</Text>
        <TextInput
          editable={false}
          multiline={true}
          defaultValue={this.state.responseText}
          style={styles.textOutput}
        />
      </View>
    ) : null;

    return (
      <View>
        <Text style={styles.label}>Edit URL to submit:</Text>
        <TextInput
          returnKeyType="go"
          defaultValue="https://tryphp.w3schools.com/demo/demo_form_validation_complete.php"
          onSubmitEditing={event => {
            this.submit(event.nativeEvent.text);
          }}
          style={styles.textInput}
        />
        {responseURL}
        {responseHeaders}
        {response}
      </View>
    );
  }
}

const styles = StyleSheet.create({
  textInput: {
    flex: 1,
    borderRadius: 3,
    borderColor: 'grey',
    borderWidth: 1,
    height: 30,
    paddingLeft: 8,
  },
  label: {
    flex: 1,
    color: '#aaa',
    fontWeight: '500',
    height: 20,
  },
  textOutput: {
    flex: 1,
    fontSize: 17,
    borderRadius: 3,
    borderColor: 'grey',
    borderWidth: 1,
    height: 200,
    paddingLeft: 8,
  },
});

module.exports = HTTPExampleMultiPartFormData;
