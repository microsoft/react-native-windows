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

var ProgressBar = require('ProgressBarWindows');
var React = require('React');
var RNTesterBlock = require('RNTesterBlock');
var RNTesterPage = require('RNTesterPage');

var TimerMixin = require('react-timer-mixin');

var MovingBar = React.createClass({
  mixins: [TimerMixin],

  getInitialState: function() {
    return {
      progress: 0
    };
  },

  componentDidMount: function() {
    this.setInterval(
      () => {
        var progress = (this.state.progress + 0.1) % 100;
        this.setState({progress: progress});
      }, 10
    );
  },

  render: function() {
    return <ProgressBar progress={this.state.progress} {...this.props} />;
  },
});

var ProgressBarWindowsExample = React.createClass({

  statics: {
    title: '<ProgressBarWindows>',
    description: 'Visual indicator of progress of some operation. ' +
        'Shows a horizontal bar with either percent indeterminate progress.',
  },

  render: function() {
    return (
      <RNTesterPage title="ProgressBar Examples">
        <RNTesterBlock title="Indeterminate ProgressBar">
          <ProgressBar />
        </RNTesterBlock>

        <RNTesterBlock title="Determinate ProgressBar">
          <MovingBar indeterminate={false} />
        </RNTesterBlock>

        <RNTesterBlock title="Red Indeterminate ProgressBar">
          <ProgressBar color="red" />
        </RNTesterBlock>

        <RNTesterBlock title="Blue Determinate ProgressBar">
          <MovingBar color="blue" indeterminate={false} />
        </RNTesterBlock>
      </RNTesterPage>
    );
  },
});

module.exports = ProgressBarWindowsExample;
