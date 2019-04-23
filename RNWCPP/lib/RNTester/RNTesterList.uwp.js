// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
'use strict';
Object.defineProperty(exports, "__esModule", { value: true });
var ComponentExamples = [
    {
        key: 'ActivityIndicatorExample',
        module: require('react-native/RNTester/js/ActivityIndicatorExample'),
    },
    {
        key: 'ButtonExample',
        module: require('react-native/RNTester/js/ButtonExample'),
    },
    {
        key: 'CustomViewExample',
        module: require('./CustomViewExample.uwp'),
    },
    {
        key: 'DatePickerExample',
        module: require('./DatePickerExample.uwp'),
    },
    {
        key: 'FlatListExample',
        module: require('react-native/RNTester/js/FlatListExample'),
    },
    {
        key: 'ImageExample',
        module: require('react-native/RNTester/js/ImageExample'),
    },
    {
        key: 'LayoutEventsExample',
        module: require('react-native/RNTester/js/LayoutEventsExample'),
    },
    {
        key: 'PickerUWPExample',
        module: require('./PickerUWPExample'),
    },
    {
        key: 'PopupExample',
        module: require('./PopupExample.uwp'),
    },
    {
        key: 'ScrollViewSimpleExample',
        module: require('react-native/RNTester/js/ScrollViewSimpleExample')
    },
    /*  TODO: Enable SectionList when Animation is supported
    {
      key: 'SectionListExample',
      module: require('react-native/RNTester/js/SectionListExample'),
    },
    */
    {
        key: 'SwitchExample',
        module: require('react-native/RNTester/js/SwitchExample'),
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
        module: require('react-native/RNTester/js/TouchableExample'),
    },
    {
        key: 'TransparentHitTestExample',
        module: require('react-native/RNTester/js/TransparentHitTestExample'),
    },
    {
        key: 'ViewExample',
        module: require('react-native/RNTester/js/ViewExample'),
    },
];
var APIExamples = [
    {
        key: 'AccessibilityExample',
        module: require('./AccessibilityExample')
    },
    {
        key: 'AppStateExample',
        module: require('react-native/RNTester/js/AppStateExample')
    },
    {
        key: 'BorderExample',
        module: require('react-native/RNTester/js/BorderExample'),
    },
    {
        key: 'BoxShadowExample - UWP NotYetImplemented',
        module: require('react-native/RNTester/js/BoxShadowExample'),
    },
    {
        key: 'ClipboardExample',
        module: require('react-native/RNTester/js/ClipboardExample'),
    },
    {
        key: 'Dimensions',
        module: require('react-native/RNTester/js/DimensionsExample'),
    },
    {
        key: 'GeolocationExample',
        module: require('react-native/RNTester/js/GeolocationExample'),
    },
    {
        key: 'KeyboardExample',
        module: require('./KeyboardExample')
    },
    {
        key: 'LayoutExample',
        module: require('react-native/RNTester/js/LayoutExample'),
    },
    {
        key: 'LinkingExample',
        module: require('react-native/RNTester/js/LinkingExample'),
    },
    {
        key: 'PanResponderExample',
        module: require('react-native/RNTester/js/PanResponderExample'),
    },
    {
        key: 'PointerEventsExample',
        module: require('react-native/RNTester/js/PointerEventsExample'),
    },
    {
        key: 'RTLExample',
        module: require('react-native/RNTester/js/RTLExample'),
    },
    {
        key: 'TimerExample',
        module: require('react-native/RNTester/js/TimerExample'),
    },
    {
        key: 'WebSocketExample',
        module: require('react-native/RNTester/js/WebSocketExample'),
    },
];
var Modules = {};
APIExamples.concat(ComponentExamples).forEach(function (Example) {
    Modules[Example.key] = Example.module;
});
var RNTesterList = {
    APIExamples: APIExamples,
    ComponentExamples: ComponentExamples,
    Modules: Modules
};
exports.default = RNTesterList;
//# sourceMappingURL=RNTesterList.uwp.js.map