// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

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
    key: 'ActivityIndicatorExample',
    module: require('@microsoft/react-native/RNTester/js/ActivityIndicatorExample'),
  },
  {
    key: 'ButtonExample',
    module: require('@microsoft/react-native/RNTester/js/ButtonExample'),
  },
  {
    key: 'FlatListExample',
    module: require('@microsoft/react-native/RNTester/js/FlatListExample'),
  },
  {
    key: 'ImageExample',
    module: require('@microsoft/react-native/RNTester/js/ImageExample'),
  },
  {
    key: 'LayoutEventsExample',
    module: require('@microsoft/react-native/RNTester/js/LayoutEventsExample'),
  },
  {
    key: 'PickerUWPExample',
    module: require('./PickerUWPExample'),
  },
  {
    key: 'ScrollViewSimpleExample',
    module: require('@microsoft/react-native/RNTester/js/ScrollViewSimpleExample')
  },
  /*  TODO: Enable SectionList when Animation is supported
  {
    key: 'SectionListExample',
    module: require('@microsoft/react-native/RNTester/js/SectionListExample'),
  },
  */
  {
    key: 'SwitchExample',
    module: require('@microsoft/react-native/RNTester/js/SwitchExample'),
  },
  {
    key: 'TextExample',
    module: require('./TextExample.uwp').default,
  },
  {
    key: 'TextInputExample',
    module: require('./TextInputExample.uwp'),
  },
  {
    key: 'TouchableExample',
    module: require('@microsoft/react-native/RNTester/js/TouchableExample'),
  },
  {
    key: 'TransparentHitTestExample',
    module: require('@microsoft/react-native/RNTester/js/TransparentHitTestExample'),
  },
  {
    key: 'ViewExample',
    module: require('@microsoft/react-native/RNTester/js/ViewExample'),
  },
  /* TODO: Not enough of WebView is implemented to load the Example
  {
    key: 'WebViewExample',
    module: require('@microsoft/react-native/RNTester/js/WebViewExample'),
  },*/
];

const APIExamples: Array<IRNTesterExample> = [
  {
    key: 'AppStateExample',
    module: require('@microsoft/react-native/RNTester/js/AppStateExample')
  },
  {
    key: 'BorderExample',
    module: require('@microsoft/react-native/RNTester/js/BorderExample'),
  },
  {
    key: 'BoxShadowExample - UWP NotYetImplemented',
    module: require('@microsoft/react-native/RNTester/js/BoxShadowExample'),
  },
  {
    key: 'ClipboardExample',
    module: require('@microsoft/react-native/RNTester/js/ClipboardExample'),
  },
  {
    key: 'Dimensions',
    module: require('@microsoft/react-native/RNTester/js/DimensionsExample'),
  },
  {
    key: 'GeolocationExample',
    module: require('@microsoft/react-native/RNTester/js/GeolocationExample'),
  },
  {
    key: 'LayoutExample',
    module: require('@microsoft/react-native/RNTester/js/LayoutExample'),
  },
  {
    key: 'LinkingExample',
    module: require('@microsoft/react-native/RNTester/js/LinkingExample'),
  },
  {
    key: 'PanResponderExample',
    module: require('@microsoft/react-native/RNTester/js/PanResponderExample'),
  },
  {
    key: 'PointerEventsExample',
    module: require('@microsoft/react-native/RNTester/js/PointerEventsExample'),
  },
  {
    key: 'RTLExample',
    module: require('@microsoft/react-native/RNTester/js/RTLExample'),
  },
  {
    key: 'TimerExample',
    module: require('@microsoft/react-native/RNTester/js/TimerExample'),
  },
  {
    key: 'WebSocketExample',
    module: require('@microsoft/react-native/RNTester/js/WebSocketExample'),
  },
  /* TODO: ? XHRExample requires CameraRoll access
  {
    key: 'XHRExample',
    module: require('@microsoft/react-native/RNTester/js/XHRExample'),
  },*/
];

const Modules: {[key:string]: IRNTesterModule} = {};

APIExamples.concat(ComponentExamples).forEach((Example: IRNTesterExample) => {
  Modules[Example.key] = Example.module;
});

const RNTesterList = {
  APIExamples,
  ComponentExamples,
  Modules
};

export default RNTesterList;
