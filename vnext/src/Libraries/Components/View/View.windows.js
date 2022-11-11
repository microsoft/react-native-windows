/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @format
 * @flow strict-local
 */

import type {ViewProps} from './ViewPropTypes';

import flattenStyle from '../../StyleSheet/flattenStyle';
import TextAncestor from '../../Text/TextAncestor';
import ViewNativeComponent from './ViewNativeComponent';
import * as React from 'react';
import invariant from 'invariant'; // [Windows]
// [Windows
import type {KeyEvent} from '../../Types/CoreEventTypes';
// Windows]

export type Props = ViewProps;

/**
 * The most fundamental component for building a UI, View is a container that
 * supports layout with flexbox, style, some touch handling, and accessibility
 * controls.
 *
 * @see https://reactnative.dev/docs/view
 */
const View: React.AbstractComponent<
  ViewProps,
  React.ElementRef<typeof ViewNativeComponent>,
> = React.forwardRef(
  (
    {
      accessibilityElementsHidden,
      accessibilityLiveRegion,
      'aria-live': ariaLive,
      accessibilityLabel,
      accessibilityRole,
      'aria-label': ariaLabel,
      'aria-hidden': ariaHidden,
      focusable,
      id,
      importantForAccessibility,
      nativeID,
      pointerEvents,
      role,
      style,
      tabIndex,
      ...otherProps
    }: ViewProps,
    forwardedRef,
  ) => {
    const {
      accessibilityState,
      'aria-busy': ariaBusy,
      'aria-checked': ariaChecked,
      'aria-disabled': ariaDisabled,
      'aria-expanded': ariaExpanded,
      'aria-selected': ariaSelected,
    } = otherProps;

    const _accessibilityState = {
      busy: ariaBusy ?? accessibilityState?.busy,
      checked: ariaChecked ?? accessibilityState?.checked,
      disabled: ariaDisabled ?? accessibilityState?.disabled,
      expanded: ariaExpanded ?? accessibilityState?.expanded,
      selected: ariaSelected ?? accessibilityState?.selected,
    };

    // Map role values to AccessibilityRole values
    const roleToAccessibilityRoleMapping = {
      alert: 'alert',
      alertdialog: undefined,
      application: undefined,
      article: undefined,
      banner: undefined,
      button: 'button',
      cell: undefined,
      checkbox: 'checkbox',
      columnheader: undefined,
      combobox: 'combobox',
      complementary: undefined,
      contentinfo: undefined,
      definition: undefined,
      dialog: undefined,
      directory: undefined,
      document: undefined,
      feed: undefined,
      figure: undefined,
      form: undefined,
      grid: 'grid',
      group: undefined,
      heading: 'header',
      img: 'image',
      link: 'link',
      list: 'list',
      listitem: undefined,
      log: undefined,
      main: undefined,
      marquee: undefined,
      math: undefined,
      menu: 'menu',
      menubar: 'menubar',
      menuitem: 'menuitem',
      meter: undefined,
      navigation: undefined,
      none: 'none',
      note: undefined,
      presentation: 'none',
      progressbar: 'progressbar',
      radio: 'radio',
      radiogroup: 'radiogroup',
      region: undefined,
      row: undefined,
      rowgroup: undefined,
      rowheader: undefined,
      scrollbar: 'scrollbar',
      searchbox: 'search',
      separator: undefined,
      slider: 'adjustable',
      spinbutton: 'spinbutton',
      status: undefined,
      summary: 'summary',
      switch: 'switch',
      tab: 'tab',
      table: undefined,
      tablist: 'tablist',
      tabpanel: undefined,
      term: undefined,
      timer: 'timer',
      toolbar: 'toolbar',
      tooltip: undefined,
      tree: undefined,
      treegrid: undefined,
      treeitem: undefined,
    };

    const accessibilityValue = {
      max: otherProps['aria-valuemax'] ?? otherProps.accessibilityValue?.max,
      min: otherProps['aria-valuemin'] ?? otherProps.accessibilityValue?.min,
      now: otherProps['aria-valuenow'] ?? otherProps.accessibilityValue?.now,
      text: otherProps['aria-valuetext'] ?? otherProps.accessibilityValue?.text,
    };
    const restWithDefaultProps = {...otherProps, accessibilityValue};

    const flattenedStyle = flattenStyle(style);
    const newPointerEvents = flattenedStyle?.pointerEvents || pointerEvents;

    const _keyDown = (event: KeyEvent) => {
      if (otherProps.keyDownEvents && event.isPropagationStopped() !== true) {
        // $FlowFixMe - keyDownEvents was already checked to not be undefined
        for (const el of otherProps.keyDownEvents) {
          if (event.nativeEvent.code == el.code && el.handledEventPhase == 3) {
            event.stopPropagation();
          }
        }
      }
      otherProps.onKeyDown && otherProps.onKeyDown(event);
    };

    const _keyUp = (event: KeyEvent) => {
      if (otherProps.keyUpEvents && event.isPropagationStopped() !== true) {
        // $FlowFixMe - keyDownEvents was already checked to not be undefined
        for (const el of otherProps.keyUpEvents) {
          if (event.nativeEvent.code == el.code && el.handledEventPhase == 3) {
            event.stopPropagation();
          }
        }
      }
      otherProps.onKeyUp && otherProps.onKeyUp(event);
    };

    const _keyDownCapture = (event: KeyEvent) => {
      if (otherProps.keyDownEvents && event.isPropagationStopped() !== true) {
        // $FlowFixMe - keyDownEvents was already checked to not be undefined
        for (const el of otherProps.keyDownEvents) {
          if (event.nativeEvent.code == el.code && el.handledEventPhase == 1) {
            event.stopPropagation();
          }
        }
      }
      otherProps.onKeyDownCapture && otherProps.onKeyDownCapture(event);
    };

    const _keyUpCapture = (event: KeyEvent) => {
      if (otherProps.keyUpEvents && event.isPropagationStopped() !== true) {
        // $FlowFixMe - keyDownEvents was already checked to not be undefined
        for (const el of otherProps.keyUpEvents) {
          if (event.nativeEvent.code == el.code && el.handledEventPhase == 1) {
            event.stopPropagation();
          }
        }
      }
      otherProps.onKeyUpCapture && otherProps.onKeyUpCapture(event);
    };

    // [Windows
    const childrenWithImportantForAccessibility = children => {
      const updatedChildren = React.Children.map(children, child => {
        if (React.isValidElement(child)) {
          if (child.props.children) {
            return React.cloneElement(child, {
              accessible: false,
              children: childrenWithImportantForAccessibility(
                child.props.children,
              ),
            });
          } else {
            return React.cloneElement(child, {accessible: false});
          }
        }
        return child;
      });
      if (updatedChildren.length == 1) {
        return updatedChildren[0];
      } else {
        return updatedChildren;
      }
    };
    // Windows]

    return (
      // [Windows
      // In core this is a TextAncestor.Provider value={false} See
      // https://github.com/facebook/react-native/commit/66601e755fcad10698e61d20878d52194ad0e90c
      // But since Views are not currently supported in Text, we do not need the extra provider
      <TextAncestor.Consumer>
        {hasTextAncestor => {
          invariant(
            !hasTextAncestor,
            'Nesting of <View> within <Text> is not currently supported.',
          );
          return (
            <ViewNativeComponent
              accessibilityLiveRegion={
                ariaLive === 'off'
                  ? 'none'
                  : ariaLive ?? accessibilityLiveRegion
              }
              accessibilityLabel={ariaLabel ?? accessibilityLabel}
              focusable={tabIndex !== undefined ? !tabIndex : focusable}
              accessibilityState={_accessibilityState}
              accessibilityRole={
                role ? roleToAccessibilityRoleMapping[role] : accessibilityRole
              }
              accessibilityElementsHidden={
                ariaHidden ?? accessibilityElementsHidden
              }
              importantForAccessibility={
                ariaHidden === true
                  ? 'no-hide-descendants'
                  : importantForAccessibility
              }
              nativeID={id ?? nativeID}
              {...restWithDefaultProps}
              style={style}
              pointerEvents={newPointerEvents}
              ref={forwardedRef}
              onKeyDown={_keyDown}
              onKeyDownCapture={_keyDownCapture}
              onKeyUp={_keyUp}
              onKeyUpCapture={_keyUpCapture}
              // [Windows
              accessible={
                importantForAccessibility === 'no-hide-descendants'
                  ? false
                  : otherProps.accessible
              }
              children={
                importantForAccessibility === 'no-hide-descendants'
                  ? childrenWithImportantForAccessibility(otherProps.children)
                  : otherProps.children
              }
              // Windows]
            />
          );
        }}
      </TextAncestor.Consumer>
      // Windows]
    );
  },
);

View.displayName = 'View';

module.exports = View;
