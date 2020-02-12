/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 * @flow
 */

'use strict';

const React = require('react');
const SampleTurboModuleExample = require('./SampleTurboModuleExample');

exports.displayName = (undefined: ?string);
exports.title = 'TurboModule';
exports.description = 'Usage of TurboModule';
exports.examples = [
  {
    title: 'SampleTurboModule',
    render: function(): React.Element<any> {
      return <SampleTurboModuleExample />;
    },
  },
];
