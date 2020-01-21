/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @flow
 * @format
 */

'use strict';
const React = require('react');
const View = require('../../Components/View/View');

// The RN YellowBoxInspectorHeader uses PixelRatio, which isn't multi DPI aware.
// So we just stub the header for now
module.exports = () => <View />;
