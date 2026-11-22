/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

'use strict';

const React = require('react');
const renderer = require('@react-native/jest-preset/jest/renderer');
const View = require('../../View/View').default;
const TouchableHighlight = require('../TouchableHighlight.windows').default;

describe('TouchableHighlight Windows accessibility', () => {
  let component;

  afterEach(async () => {
    if (component != null) {
      await renderer.unmount(component);
      component = null;
    }
  });

  it.each([true, false])(
    'preserves Windows accessibility states set to %s while normalizing ARIA states',
    async value => {
      const accessibilityState = {
        readOnly: value,
        multiselectable: value,
        required: value,
        busy: true,
        checked: true,
        disabled: true,
        expanded: true,
        selected: true,
      };
      component = await renderer.create(
        <TouchableHighlight
          accessibilityState={accessibilityState}
          aria-busy={false}
          aria-checked={false}
          aria-disabled={false}
          aria-expanded={false}
          aria-selected={false}>
          <View />
        </TouchableHighlight>,
      );

      const view = component.root.findAllByType(View)[0];
      expect(view.props.accessibilityState).toEqual({
        readOnly: value,
        multiselectable: value,
        required: value,
        busy: false,
        checked: false,
        disabled: false,
        expanded: false,
        selected: false,
      });
      expect(accessibilityState.busy).toBe(true);
      expect(accessibilityState.disabled).toBe(true);
    },
  );

  it.each([
    [
      'aria-disabled=false overrides accessibilityState.disabled=true',
      {'aria-disabled': false, accessibilityState: {disabled: true}},
      false,
    ],
    [
      'aria-disabled=true overrides accessibilityState.disabled=false',
      {'aria-disabled': true, accessibilityState: {disabled: false}},
      true,
    ],
    [
      'disabled=false overrides both accessibility inputs',
      {
        disabled: false,
        'aria-disabled': true,
        accessibilityState: {disabled: true},
      },
      false,
    ],
    [
      'disabled=true overrides both accessibility inputs',
      {
        disabled: true,
        'aria-disabled': false,
        accessibilityState: {disabled: false},
      },
      true,
    ],
    [
      'accessibilityState.disabled remains the fallback',
      {accessibilityState: {disabled: true}},
      true,
    ],
    ['unspecified disabled states remain enabled', {}, undefined],
  ])(
    'aligns accessibility and activation when %s',
    async (_, props, disabled) => {
      const onPress = jest.fn();
      component = await renderer.create(
        <TouchableHighlight {...props} onPress={onPress}>
          <View />
        </TouchableHighlight>,
      );

      const view = component.root.findAllByType(View)[0];
      expect(view.props.accessibilityState?.disabled).toBe(disabled);
      expect(view.props.onStartShouldSetResponder()).toBe(disabled !== true);

      await React.act(() => {
        view.props.onClick({
          nativeEvent: {},
          currentTarget: 1,
          target: 1,
        });
      });
      expect(onPress).toHaveBeenCalledTimes(disabled === true ? 0 : 1);
    },
  );
});
