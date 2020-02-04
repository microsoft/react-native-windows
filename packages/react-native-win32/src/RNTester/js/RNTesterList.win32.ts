/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */
'use strict';
import React = require('react');

interface IRNTesterExample {
  key: string;
  // tslint:disable-next-line no-reserved-keywords
  module: IRNTesterModule;
}

interface IRNTesterModule {
  title: string;
  description: string;
  external: boolean;
  examples: [IRNTesterModuleExample];
}

interface IRNTesterModuleExample {
  title: string;
  description: string;
  render(): React.Component;
}

const ComponentExamples: Array<IRNTesterExample> = [
  {
    key: 'TouchableWin32Example',
    module: require('../../Libraries/Components/Touchable/Tests/TouchableWin32Test'),
  },
  {
    key: 'ScrollViewSimpleExample',
    module: require('react-native/RNTester/js/ScrollViewSimpleExample'),
  },
  {
    key: 'TouchableExample',
    module: require('react-native/RNTester/js/TouchableExample'),
  },
  {
    key: 'ViewExample',
    module: require('react-native/RNTester/js/ViewExample'),
  },
  {
    key: 'ViewWin32Test',
    module: require('../../Libraries/Components/View/Tests/ViewWin32Test'),
  },
  {
    key: 'TextInputExample',
    module: require('../../Libraries/Components/TextInput/Tests/TextInputTest'),
  },
  {
    key: 'ImageWin32Test',
    module: require('../../Libraries/Image/Tests/ImageWin32Test'),
  },
  {
    key: 'PickerExample',
    module: require('../../Libraries/Components/Picker/Tests/PickerWin32Example'),
  }
];

const APIExamples: Array<IRNTesterExample> = [
  {
    key: 'AccessibilityExampleWin32',
    module: require('../APIExamples/AccessibilityExampleWin32'),
  },
  {
    key: 'AppStateExample',
    module: require('react-native/RNTester/js/AppStateExample'),
  },
  {
    key: 'BorderExample',
    module: require('react-native/RNTester/js/BorderExample'),
  },
  {
    key: 'LayoutExample',
    module: require('react-native/RNTester/js/LayoutExample'),
  },
  {
    key: 'PointerEventsExample',
    module: require('react-native/RNTester/js/PointerEventsExample'),
  },
  {
    key: 'ThemingExample',
    module: require('../APIExamples/ThemingModuleAPI'),
  },
];

const Modules: {[key: string]: IRNTesterModule} = {};

APIExamples.concat(ComponentExamples).forEach((Example: IRNTesterExample) => {
  Modules[Example.key] = Example.module;
});

const RNTesterList = {
  APIExamples,
  ComponentExamples,
  Modules,
};

export = RNTesterList;
