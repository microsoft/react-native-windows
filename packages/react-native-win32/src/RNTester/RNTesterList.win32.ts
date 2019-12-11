'use strict';

interface IRNTesterExample {
  key: string;
  // tslint:disable-next-line no-reserved-keywords
  module: Object;
}

const ComponentExamples: Array<IRNTesterExample> = [
  {
    key: 'TouchableWin32Example',
    module: require('../Libraries/Components/Touchable/Tests/TouchableWin32Test'),
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
    module: require('../Libraries/Components/View/Tests/ViewWin32Test'),
  },
  {
    key: 'TextInputExample',
    module: require('../Libraries/Components/TextInput/Tests/TextInputTest'),
  },
  {
    key: 'ImageWin32Test',
    module: require('../Libraries/Image/Tests/ImageWin32Test'),
  },
];

const APIExamples: Array<IRNTesterExample> = [
  {
    key: 'AccessibilityExampleWin32',
    module: require('./APIExamples/AccessibilityExampleWin32'),
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
    module: require('./APIExamples/ThemingModuleAPI'),
  },
];

const Modules = {};

APIExamples.concat(ComponentExamples).forEach(Example => {
  Modules[Example.key] = Example.module;
});

const RNTesterList = {
  APIExamples,
  ComponentExamples,
  Modules,
};

export default RNTesterList;
