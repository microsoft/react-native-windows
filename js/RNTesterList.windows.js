/**
 * Copyright (c) Microsoft Corporation and contributors. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * @flow
 */
'use strict';

export type RNTesterExample = {
  key: string;
  module: React.Component;
};

var ComponentExamples: Array<RNTesterExample> = [
  {
    key: 'ActivityIndicatorExample',
    module: require('./ActivityIndicatorExample'),
  },
  {
    key: 'ButtonExample',
    module: require('./ButtonExample'),
  },
  {
    key: 'FlipViewWindowsExample',
    module: require('./FlipViewWindowsExample'),
  },
  {
    key: 'ImageExample',
    module: require('./ImageExample'),
  },
  {
    key: 'ListViewExample',
    module: require('./ListViewWindowsExample'),
  },
  {
    key: 'ListViewGridLayoutExample',
    module: require('./ListViewGridLayoutExample'),
  },
  {
    key: 'ListViewPagingExample',
    module: require('./ListViewPagingExample'),
  },
  {
    key: 'PickerWindowsExample',
    module: require('./PickerWindowsExample'),
  },
  {
    key: 'ProgressBarWindowsExample',
    module: require('./ProgressBarWindowsExample'),
  },
  {
    key: 'ScrollViewSimpleExample',
    module: require('./ScrollViewSimpleExample'),
  },
  {
    key: 'SliderExample',
    module: require('./SliderExample'),
  },
  {
    key: 'StatusBarExample',
    module: require('./StatusBarExample'),
  },
  {
    key: 'SwitchExample',
    module: require('./SwitchExample'),
  },
  {
    key: 'TextExample',
    module: require('./TextExample'),
  },
  {
    key: 'TextInputExample',
    module: require('./TextInputExample'),
  },
  {
    key: 'TouchableExample',
    module: require('./TouchableExample'),
  },
  {
    key: 'ViewExample',
    module: require('./ViewExample'),
  },
  {
    key: 'WebViewExample',
    module: require('./WebViewExample'),
  },
];

const APIExamples = [
  {
    key: 'AlertExample',
    module: require('./AlertExample').AlertExample,
  },
  {
    key: 'AnimatedExample',
    module: require('./AnimatedExample'),
  },
  {
    key: 'AppStateExample',
    module: require('./AppStateExample'),
  },
  {
    key: 'BorderExample',
    module: require('./BorderExample'),
  },
  {
    key: 'ClipboardExample',
    module: require('./ClipboardExample'),
  },
  {
    key: 'GeolocationExample',
    module: require('./GeolocationExample'),
  },
  {
    key: 'LayoutEventsExample',
    module: require('./LayoutEventsExample'),
  },
  {
    key: 'LinkingExample',
    module: require('./LinkingExample'),
  },
  {
    key: 'LayoutAnimationExample',
    module: require('./LayoutAnimationExample'),
  },
  {
    key: 'LayoutExample',
    module: require('./LayoutExample'),
  },
  {
    key: 'NativeAnimationsExample',
    module: require('./NativeAnimationsExample'),
  },
  {
    key: 'NetInfoExample',
    module: require('./NetInfoExample'),
  },
  {
    key: 'OrientationChangeExample',
    module: require('./OrientationChangeExample'),
  },
  {
    key: 'PanResponderExample',
    module: require('./PanResponderExample'),
  },
  {
    key: 'PointerEventsExample',
    module: require('./PointerEventsExample'),
  },
  {
    key: 'RTLExample',
    module: require('./RTLExample'),
  },
  {
    key: 'TimerExample',
    module: require('./TimerExample'),
  },
  {
    key: 'TransformExample',
    module: require('./TransformExample'),
  },
  {
    key: 'VibrationExample',
    module: require('./VibrationExample'),
  },  
  {
    key: 'WebSocketExample',
    module: require('./WebSocketExample'),
  },
  {
    key: 'XHRExample',
    module: require('./XHRExample'),
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

module.exports = RNTesterList;
