/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @flow
 * @format
 */
'use strict';

const React = require('react');
const {Text} = require('react-native');

exports.framework = 'React';
exports.title = 'Test';
exports.category = 'Test';
exports.description = ('Test': string);
exports.examples = [
  {
    title: 'Test',
    render(): React.Node {
      return <Text>Testing</Text>;
    },
  },
  {
    title: 'Fetch Test',
    render(): React.Node {
      return <Text>Testing</Text>;
    },
  },
];
