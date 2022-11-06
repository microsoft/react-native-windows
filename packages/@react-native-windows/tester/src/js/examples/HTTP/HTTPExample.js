/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @flow
 * @format
 */
'use strict';

const React = require('react');
const HTTPExampleMultiPartFormData = require('./HTTPExampleMultiPartFormData');
const XHRExampleFetch = require('../XHR/XHRExampleFetch');

exports.framework = 'React';
exports.title = 'HTTP';
exports.category = 'Basic';
exports.description =
  ('Example that demonstrates direct and indirect HTTP native module usage.': string);
exports.examples = [
  {
    title: 'multipart/form-data POST',
    render(): React.Node {
      return <HTTPExampleMultiPartFormData />;
    },
  },
  {
    title: 'Fetch Test',
    render(): React.Node {
      return <XHRExampleFetch />;
    },
  },
];
