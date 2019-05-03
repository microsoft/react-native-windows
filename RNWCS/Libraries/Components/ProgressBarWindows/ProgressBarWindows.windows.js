/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 *
 * @providesModule ProgressBarWindows
 */
'use strict';

var PropTypes = require('prop-types');
var React = require('React');
var ViewPropTypes = require('ViewPropTypes');

var ColorPropType = require('DeprecatedColorPropType');

var requireNativeComponent = require('requireNativeComponent');

/**
 * React component that wraps the Windows-only `ProgressBar`. This component is used to indicate
 * that the app is loading or there is some activity in the app.
 */
class ProgressBarWindows extends React.Component {
 props: {
  indeterminate?: boolean,
  progress?: number,
  color?: $FlowFixMe,
 };

 static propTypes = {
   ...ViewPropTypes,

   /**
    * If the progress bar will show indeterminate progress.
    */
   indeterminate: PropTypes.bool,
   /**
    * The progress value (between 0 and 100).
    */
   progress: PropTypes.number,
   /**
    * Color of the progress bar.
    */
   color: ColorPropType,
 };

 static defaultProps = {
   indeterminate: true
 };

 render() {
   return <WindowsProgressBar {...this.props}/>;
 }
}

var WindowsProgressBar = requireNativeComponent('WindowsProgressBar', ProgressBarWindows);

module.exports = ProgressBarWindows;
