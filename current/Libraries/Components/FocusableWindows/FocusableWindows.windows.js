/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 *
 * Helper component for building keyboard focusable custom controls when the original native one has no such support
 *
 * For example, assuming a SomeComponent already exists, a custom KeyboardSupporting Component can
 * be created by using createFocusableComponent:
 *
 *   const FocusableSomeComponent = createFocusableComponent(SomeComponent);
 *
 *   class Whatever extends React.Component {
 *     render () {
 *         return (
 *            <FocusableSomeComponent
 *               {...this.props }
 *               onKeyDown={ ... }
 *               onFocus={ ... }
 *               handledKeyDownKeys: {[FocusableSomeComponent.keys.Enter]}
 *            >
 *               ... .
 *            </FocusableSomeComponent>
 *         );
 *      }
 *   }
 *
 * The generated component uses a helper FocusableWindow component that relies on WindowsControl native view.
 * Implementation allows for splitting the property/styles (setNativeProps included )between WindowsControl and the original Component appropriately.
 *
 * @providesModule FocusableWindows
 */
'use strict';

var PropTypes = require('prop-types');
var React = require('React');
var ReactNative = require('ReactNative');
var ViewPropTypes = require('ViewPropTypes');
var requireNativeComponent = require('requireNativeComponent');
var UIManager = require('UIManager');
const flattenStyle = require('flattenStyle');
const {
  DeprecatedAccessibilityTraits,
 } = require('DeprecatedViewAccessibility');
/**
 * Add 'listItem' to supported traits.
 *
 * @platform windows
 */
const FocusableWindowsAccessibilityTraits = Array.from(DeprecatedAccessibilityTraits).concat(['listItem']);

// This describes the propType based interface for WindowsControl
class FocusableWindowsTemplate {
  static focusablePropTypes = {
    /**
     * Controls whether the view is a tab stop. Useful for buttons and other
     * controls that can be focused.
     *
     * @platform windows
     */
    isTabStop: PropTypes.bool,

    /**
     * Sets the tab index for the view.
     *
     * @platform windows
     */
    tabIndex: PropTypes.number,

    /**
     * Sets the keyboard navigation mode for the view.
     *
     * @platform windows
     */
    tabNavigation: PropTypes.oneOf([
      'local',
      'cycle',
      'once'
    ]),

    /**
     * Controls whether control should use system default provided focus rects
     *
     * @platform windows
     */
    disableSystemFocusVisuals: PropTypes.bool,

    /**
     * Determines control accessibility behavior.
     * For more details refer to Libraries\Components\View\ViewPropTypes.js
     *
     * @platform windows
     */
    importantForAccessibility: PropTypes.oneOf([
      'auto',
      'yes',
      'no',
      'no-hide-descendants',
      'yes-dont-hide-descendants', // Windows only
    ]),

    /**
     * Provides accessibility traits to the screen reader. By default no traits are set.
     * For more details refer to Libraries\Components\View\ViewPropTypes.js.
     *
     * @platform windows
     */
    accessibilityTraits: PropTypes.oneOfType([
      PropTypes.oneOf(FocusableWindowsAccessibilityTraits),
      PropTypes.arrayOf(PropTypes.oneOf(FocusableWindowsAccessibilityTraits)),
    ]),

    /**
     * Provides element accessibility text to the screen reader.
     * For more details refer to Libraries\Components\View\ViewPropTypes.js.
     *
     * @platform windows
     */
    accessibilityLabel: PropTypes.string,

    /**
     * Called when the user performs accessibility tap gesture.
     *
     * @platform windows
     */
    onAccessibilityTap: PropTypes.func,

    /**
     * Called when the view receives focus.
     *
     * @platform windows
     */
    onFocus: PropTypes.func,

    /**
     * Called when the view focus is lost.
     *
     * @platform windows
     */
    onBlur: PropTypes.func,

    /**
     * Set of keys that should be handled on key down by this component.
     *
     * @platform windows
     */
    handledKeyDownKeys: PropTypes.arrayOf(PropTypes.number),

    /**
     * Set of keys that should be handled on key up by this component.
     *
     * @platform windows
     */
    handledKeyUpKeys: PropTypes.arrayOf(PropTypes.number),

    /**
     * Called when key down while component has focus.
     *
     * @platform windows
     */
    onKeyDown: PropTypes.func,

    /**
     * Called when key up while component has focus.
     *
     * @platform windows
     */
    onKeyUp: PropTypes.func,
    /**
     * A callback based "ref"-like property for the embedded Component
     * Care must be taken not to call setNativeProps on this object directly, or, if not possible, at least to avoid
     * properties that are meant to be intercepted by the implementation of the focusable component ("style/transform" currently).
     *
     * @platform windows
     */
    componentRef: PropTypes.func,
  };
}

// WindowsControl implementation peculiarities make it check for presence of RCTView like properties in propTypes,
// even though those are not really supported/implemented.
// We add those properties for the sake of the native component property validation.
FocusableWindowsTemplate.propTypes = Object.assign({}, ViewPropTypes, FocusableWindowsTemplate.focusablePropTypes);

// Creates a decorated component class
function createFocusableComponent(Component: any) {
  class FocusableWindows extends React.Component {

    static keys = UIManager.WindowsControl.Constants.Keys;

    _focusable: any;
    _component: any;

    _isMounted: boolean;

    _focusableProps: Object;
    _componentProps: Object;

    constructor(props: Object) {
      super(props);
      this._isMounted = false;
      this._splitProps(props);
    }

    UNSAFE_componentWillReceiveProps(nextProps: Object) {
      this._splitProps(nextProps);
    }

    componentDidMount() {
      this._isMounted = true;
      if (this.props.componentRef) {
        this.props.componentRef(this._component);
      }
    }

    componentDidUpdate(prevProps: Object) {
      if (prevProps.componentRef !== this.props.componentRef) {
        if (prevProps.componentRef) {
          prevProps.componentRef(null);
        }
        if (this.props.componentRef) {
          this.props.componentRef(this._component);
        }
      }
    }

    componentWillUnmount() {
      if (this.props.componentRef) {
        this.props.componentRef(null);
      }
      this._isMounted = false;
    }

    _splitProps(props: Object) {
      this._focusableProps = {};
      // Prevent the child (that is View in most of the cases) from being collapsed.
      // Passed parameters can override this
      this._componentProps = { collapsable: false };

      for (const key in props) {
        if (key in FocusableWindowsTemplate.focusablePropTypes) {
          // Property supported by WindowsControl
          this._focusableProps[key] = props[key];
        } else if (key !== 'style') {
          // Property supported by Component
          this._componentProps[key] = props[key];
        } else {
          // Style case is special because it has to be split:
          // - "transform" should go to WindowsControl
          // - everything else goes to Component
          const styles = props['style'];
          if (styles) {
            let focusableStyle = {};
            let componentStyle = {};
            const flattenedStyles = flattenStyle(styles);
            for (const styleName in flattenedStyles) {
              if (styleName === 'transform' || styleName === 'display') {
                focusableStyle[styleName] = flattenedStyles[styleName];
              } else {
                componentStyle[styleName] = flattenedStyles[styleName];
              }
            }
            this._focusableProps['style'] = focusableStyle;
            this._componentProps['style'] = componentStyle;
          }
        }
      }
    }

    render() {
      return (
        <WindowsControl
          { ...this._focusableProps }
          ref={ this._setFocusableRef }
        >
          <Component
            { ...this._componentProps }
            ref={ this._setComponentRef }
          />
        </WindowsControl>
      );
    }

    _setComponentRef = (ref): void => {
      let previousComponent = this._component;
      this._component = ref;
      if (this._isMounted && this.props.componentRef && this._component !== previousComponent) {
        this.props.componentRef(this._component);
      }
    }

    _setFocusableRef = (ref): void => {
      this._focusable = ref;
    }

    getComponent() {
      return this._component;
    }

    focus() {
      if (this._focusable) {
        UIManager.dispatchViewManagerCommand(
          ReactNative.findNodeHandle(this._focusable),
          UIManager.WindowsControl.Commands.focus,
          null);
      }
    }

    blur() {
      if (this._focusable) {
        UIManager.dispatchViewManagerCommand(
          ReactNative.findNodeHandle(this._focusable),
          UIManager.WindowsControl.Commands.blur,
          null);
      }
    }

    setNativeProps(nativeProps) {
      let focusableProps = {};
      let componentProps = {};

      let atLeastOneFocusableProp = false;
      let atLeastOneComponentProp = false;

      if (nativeProps) {
        for (const key in nativeProps) {
          if (key in FocusableWindowsTemplate.focusablePropTypes) {
            // Property supported by WindowsControl
            focusableProps[key] = nativeProps[key];
            atLeastOneFocusableProp = true;
          } else if (key !== 'style') {
            // Property supported by Component
            componentProps[key] = nativeProps[key];
            atLeastOneComponentProp = true;
          } else {
            // Style case is special because it has to be split:
            // - "transform" should go to WindowsControl
            // - everything else goes to Component
            // The difference (compared with normal property processing) is that no flattening/style id resolution is needed
            const styles = nativeProps['style'];
            if (styles) {
              let focusableStyle = {};
              let childStyle = {};
              for (const styleName in styles) {
                if (styleName === 'transform' || styleName === 'display') {
                  focusableStyle[styleName] = styles[styleName];
                  atLeastOneFocusableProp = true;
                } else {
                  childStyle[styleName] = styles[styleName];
                  atLeastOneComponentProp = true;
                }
              }
              focusableProps['style'] = focusableStyle;
              componentProps['style'] = childStyle;
            }
          }
        }

        if (this._focusable && atLeastOneFocusableProp) {
          this._focusable.setNativeProps(focusableProps);
        }

        if (this._component && atLeastOneComponentProp && this._component.setNativeProps) {
          this._component.setNativeProps(componentProps);
        }
      }
    }
  }

  // Prop types are a combination of what Component and WindowsControl expose
  FocusableWindows.propTypes = Object.assign({}, Component.propTypes, FocusableWindowsTemplate.focusablePropTypes);

  return FocusableWindows;
}

// This Component registration is shared by all decorated components.
const WindowsControl = requireNativeComponent(
  'WindowsControl',
  FocusableWindowsTemplate
);

module.exports = createFocusableComponent;
