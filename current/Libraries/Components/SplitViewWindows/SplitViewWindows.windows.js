/**
 * Copyright (c) 2015-present, Facebook, Inc.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 * 
 * Portions copyright for react-native-windows:
 * 
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * 
 * @providesModule SplitViewWindows
 */
'use strict';

var ColorPropType = require('DeprecatedColorPropType');
var NativeMethodsMixin = require('NativeMethodsMixin');
var React = require('React');
var PropTypes = require('prop-types');
var ReactNative = require('ReactNative');
var StyleSheet = require('StyleSheet');
var UIManager = require('UIManager');
var View = require('View');
var ViewPropTypes = require('ViewPropTypes');

var SplitViewConsts = UIManager.getViewManagerConfig('WindowsSplitView').Constants;

var createReactClass = require('create-react-class');
var dismissKeyboard = require('dismissKeyboard');
var requireNativeComponent = require('requireNativeComponent');

var RK_PANE_REF = 'paneView';
var CONTENT_REF = 'contentView';

var SplitViewValidAttributes = {
  paneWidth: true,
  panePosition: true
};

/**
 * React component that wraps the platform `SplitView` (Windows only). The
 * Pane (typically used for navigation) is rendered with `renderPaneView`
 * and direct children are the main view (where your content goes). The pane
 * view is initially not visible on the screen, but can be pulled in from the
 * side of the window specified by the `panePosition` prop and its width can
 * be set by the `paneWidth` prop.
 *
 * Example:
 *
 * ```
 * render: function() {
 *   var paneView = (
 *     <View style={{flex: 1, backgroundColor: '#fff'}}>
 *       <Text style={{margin: 10, fontSize: 15, textAlign: 'left'}}>I'm in the Pane!</Text>
 *     </View>
 *   );
 *   return (
 *     <SplitViewWindows
 *       paneWidth={300}
 *       panePosition={SplitViewWindows.positions.Left}
 *       renderPaneView={() => paneView}>
 *       <View style={{flex: 1, alignItems: 'center'}}>
 *         <Text style={{margin: 10, fontSize: 15, textAlign: 'right'}}>Hello</Text>
 *         <Text style={{margin: 10, fontSize: 15, textAlign: 'right'}}>World!</Text>
 *       </View>
 *     </SplitViewWindows>
 *   );
 * },
 * ```
 */
var SplitViewWindows = createReactClass({
  displayName: 'SplitViewWindows',
  statics: {
    positions: SplitViewConsts.PanePositions,
  },

  propTypes: {
    ...ViewPropTypes,
    /**
     * Determines whether the keyboard gets dismissed in response to a drag.
     *   - 'none' (the default), drags do not dismiss the keyboard.
     *   - 'on-drag', the keyboard is dismissed when a drag begins.
     */
    keyboardDismissMode: PropTypes.oneOf([
      'none', // default
      'on-drag',
    ]),
    /**
     * Specifies the background color of the pane. The default value is white.
     * If you want to set the opacity of the pane, use rgba. Example:
     *
     * ```
     * return (
     *   <SplitViewWindows paneBackgroundColor="rgba(0,0,0,0.5)">
     *   </SplitViewWindows>
     * );
     * ```
     */
    paneBackgroundColor: ColorPropType,
    /**
     * Specifies the side of the screen from which the pane will slide in.
     */
    panePosition: PropTypes.oneOf([
      SplitViewConsts.PanePositions.Left,
      SplitViewConsts.PanePositions.Right
    ]),
    /**
     * Specifies the width of the pane, more precisely the width of the view that be pulled in
     * from the edge of the window.
     */
    paneWidth: PropTypes.number,
    /**
     * Function called whenever the pane view has been opened.
     */
    onPaneOpen: PropTypes.func,
    /**
     * Function called whenever the pane view has been closed.
     */
    onPaneClose: PropTypes.func,
    /**
     * The pane view that will be rendered to the side of the screen and can be pulled in.
     */
    renderPaneView: PropTypes.func.isRequired,
  },

  mixins: [NativeMethodsMixin],

  getDefaultProps: function(): Object {
    return {
      paneBackgroundColor: 'white',
    };
  },

  getInnerViewNode: function() {
    return this.refs[CONTENT_REF].getInnerViewNode();
  },

  render: function() {
    var paneViewWrapper =
      <View
        style={[
          styles.paneSubview,
          {width: this.props.paneWidth, backgroundColor: this.props.paneBackgroundColor}
        ]}
        collapsable={false}>
        {this.props.renderPaneView()}
      </View>;
    var childrenWrapper =
      <View ref={CONTENT_REF} style={styles.mainSubview} collapsable={false}>
        {this.props.children}
      </View>;
    return (
      <WindowsSplitView
        {...this.props}
        ref={RK_PANE_REF}
        paneWidth={this.props.paneWidth}
        panePosition={this.props.panePosition}
        style={[styles.base, this.props.style]}
        onPaneOpen={this._onPaneOpen}
        onPaneClose={this._onPaneClose}>
        {childrenWrapper}
        {paneViewWrapper}
      </WindowsSplitView>
    );
  },

  _onPaneOpen: function() {
    if (this.props.onPaneOpen) {
      this.props.onPaneOpen();
    }
  },

  _onPaneClose: function() {
    if (this.props.onPaneClose) {
      this.props.onPaneClose();
    }
  },

  /**
   * Opens the pane.
   */
  openPane: function() {
    UIManager.dispatchViewManagerCommand(
      this._getPaneLayoutHandle(),
      UIManager.getViewManagerConfig('WindowsSplitView').Commands.openPane,
      null
    );
  },

  /**
   * Closes the pane.
   */
  closePane: function() {
    UIManager.dispatchViewManagerCommand(
      this._getPaneLayoutHandle(),
      UIManager.getViewManagerConfig('WindowsSplitView').Commands.closePane,
      null
    );
  },
  /**
  * Closing and opening example
  * Note: To access the pane you have to give it a ref. Refs do not work on stateless components
  * render () {
  *   this.openPane = () => {
  *     this.refs.PANE.openPane()
  *   }
  *   this.closePane = () => {
  *     this.refs.PANE.closePane()
  *   }
  *   return (
  *     <SplitViewWindows ref={'PANE'}>
  *     </SplitViewWindows>
  *   )
  * }
  */
  _getPaneLayoutHandle: function() {
    return ReactNative.findNodeHandle(this.refs[RK_PANE_REF]);
  },

});

var styles = StyleSheet.create({
  base: {
    flex: 1,
    elevation: 16,
  },
  mainSubview: {
    position: 'absolute',
    top: 0,
    left: 0,
    right: 0,
    bottom: 0,
  },
  paneSubview: {
    position: 'absolute',
    top: 0,
    bottom: 0,
  },
});

// The View that contains both the actual pane and the main view
var WindowsSplitView = requireNativeComponent('WindowsSplitView', SplitViewWindows);

module.exports = SplitViewWindows;
