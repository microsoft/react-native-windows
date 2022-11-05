/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @format
 * @flow
 */

'use strict';

const React = require('react');

const {StyleSheet, Text, TextInput, View, Platform} = require('react-native');

class XHRExampleFetch extends React.Component<any, any> {
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
    // fetch(uri)
    //   .then(response => {
    //     this.responseURL = response.url;
    //     this.responseHeaders = response.headers;
    //     return response.text();
    //   })
    //   .then(body => {
    //     this.setState({responseText: body});
    //   });

    const formData = new FormData();

    // formData.append('user', {
    //   string: JSON.stringify({name: 'Marcin', family: 'Nowak'}),
    //   type: 'application/json',
    // });

    // const config = {
    //   headers: {
    //     'Content-Type': 'multipart/form-data',
    //   },
    // };

    // await fetch(
    //   'https://example.com/api/user/b52a6886-d02f-4121-95e3-48e3c7420081',
    //   {
    //     body: formData,
    //     method: 'POST',
    //     headers: {
    //       'Content-Type': 'multipart/form-data',
    //       Authorization: 'Bearer ' + '123',
    //     },
    //   },
    // );

    // fetch(
    //   'https://example.com/api/user/b52a6886-d02f-4121-95e3-48e3c7420081',
    //   {
    //     body: formData,
    //     method: 'POST',
    //     headers: {
    //       'Content-Type': 'multipart/form-data',
    //       Authorization: 'Bearer ' + '123',
    //     },
    //   },)
    //   .then(response => {
    //     this.responseURL = response.url;
    //     this.responseHeaders = response.headers;

    //     return response.text();
    //   })
    //   .then(body => {
    //     this.setState({responseText: body});
    //   });

    //curl.exe
      // -X POST
      // https://tryphp.w3schools.com/demo/demo_form_validation_complete.php
      // -F name=Name
      // -F email=me@mail.com
      // -F website=http://web.com
      // -F comment=Hello
      // -F gender=Other

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

      fetch(
      'https://tryphp.w3schools.com/demo/demo_form_validation_complete.php',
      {
        body: formData,
        method: 'POST',
        headers: {
          'Content-Type': 'multipart/form-data',
          Authorization: 'Bearer ' + '123',
        },
      },)
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
          defaultValue="http://www.posttestserver.com/post.php"
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
    height: Platform.OS === 'android' ? 44 : 30,
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

module.exports = XHRExampleFetch;
