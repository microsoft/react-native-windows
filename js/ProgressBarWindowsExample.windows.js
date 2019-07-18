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
var createReactClass = require('create-react-class');
var RNTesterBlock = require('RNTesterBlock');
var RNTesterPage = require('RNTesterPage');

var MovingBar = createReactClass({
  displayName: 'MovingBar',
  _intervalID: null,

  getInitialState: function() {
    return {
      progress: 0
    };
  },

  componentDidMount: function() {
    this._intervalID = setInterval(() => {
      const progress = (this.state.progress + 0.02) % 1;
      this.setState({progress});
    }, 50);
  },

  componentWillUnmount: function() {
    if (this._intervalID != null) {
      clearInterval(this._intervalID);
    }
  },

  render: function() {
    return <ProgressBar progress={this.state.progress} {...this.props} />;
  },
});

class ProgressBarWindowsExample extends React.Component {

  render() {
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
  }
}

exports.title = '<ProgressBarWindows>';
exports.description = 'Horizontal bar to show the progress of some operation.';
exports.examples = [
  {
    title: 'Simple progress bar',
    render: function(): React.Element<typeof ProgressBarWindowsExample> {
      return <ProgressBarWindowsExample />;
    },
  },
];
