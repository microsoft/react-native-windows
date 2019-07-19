/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */

import {Text, View, Button} from 'react-native';

import React = require('react');

class WithoutCurlyBracketsExample extends React.Component<{}, any> {
  constructor(props: {}) {
    super(props);
  }
  public render() {
    return (
      <View>
        <Text>Text without curly brackets</Text>
      </View>
    );
  }
}

class WithinCurlyBracketsExample extends React.Component<{}, any> {
  constructor(props: {}) {
    super(props);
  }
  public render() {
    return (
      <View>
        <Text>{'text within curly brackets'}</Text>
      </View>
    );
  }
}

class InterpolationExample extends React.Component<{}, any> {
  constructor(props: {}) {
    super(props);
  }
  public render() {
    const someVariable = 'I am a variable';
    return (
      <View>
        <Text>{'I am a string / ' + someVariable}</Text>
      </View>
    );
  }
}

class ChangingStateExample extends React.Component<{}, any> {
  constructor(props: {}) {
    super(props);
    this.state = {text: 'initial state text'};
  }
  public render() {
    return (
      <View>
        <Button
          title={'UPDATE STATE'}
          onPress={() => this.setState({text: 'updated state text'})}
        />
        <Text>{'non state text: ' + this.state.text}</Text>
      </View>
    );
  }
}

class SlowExamples extends React.Component<{}, any> {
  constructor(props: {}) {
    super(props);
  }
  public render() {
    const slowText = 'slowText';
    return (
      <View>
        <Text>
          I'm a {slowText} because my string interpolation needs to all be
          within a pair of curly brackets.
        </Text>
        <Text>
          I'm
          <Text>a slow text because I'm nested with other</Text>
          <Text>{'texts'}</Text>
        </Text>
      </View>
    );
  }
}

export const displayName = undefined;
export const title = 'Fast Path Texts';
export const description =
  'Examples of performant fast path texts, turn on IsTextPerformanceVisualizationEnabled to visualize examples';
export const examples = [
  {
    title: 'Without curly brackets example',
    render: function() {
      return <WithoutCurlyBracketsExample />;
    },
  },
  {
    title: 'Within curly brackets example',
    render: function() {
      return <WithinCurlyBracketsExample />;
    },
  },
  {
    title: 'String interpolation example',
    render: function() {
      return <InterpolationExample />;
    },
  },
  {
    title: 'Changing states within text example',
    render: function() {
      return <ChangingStateExample />;
    },
  },
  {
    title: 'Slow path text examples',
    render: function() {
      return <SlowExamples />;
    },
  },
];
