/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

// Manual spec implementation for CustomTextInput component
// Since it needs to include all the textinput related props and events

import type {ElementRef} from 'react';

// @ts-ignore
import {TextInputViewSpec, TextInputProps, HostComponent} from 'react-native';

// @ts-ignore
const NativeComponentRegistry = require('react-native/Libraries/NativeComponent/NativeComponentRegistry');
const dispatchCommand =
  require('react-native/Libraries/ReactNative/RendererProxy').dispatchCommand;

export type SomethingEvent = {
  value: string;
  target: number;
};

const nativeComponentName = 'CustomTextInput';
const spec = {
  ...TextInputViewSpec,
  uiViewClassName: nativeComponentName,
  directEventTypes: {
    ...TextInputViewSpec.directEventTypes,

    topSomething: {registrationName: 'onSomething'},
  },
  validAttributes: Object.assign({
    ...TextInputViewSpec.validAttributes,
    test: true,
    onSomething: true,
  }),
};

export interface CustomTextInputProps extends TextInputProps {
  test?: boolean;
  onSomething?: (event: SomethingEvent) => void;
}

export interface NativeCustomTextInputProps
  extends Omit<
    CustomTextInputProps,
    'value' | 'defaultValue' | 'onChangeText'
  > {
  text?: string;
  mostRecentEventCount?: number;
}

type CustomTextInputComponent = HostComponent<NativeCustomTextInputProps>;
type IDelta = string;

export default NativeComponentRegistry.get(nativeComponentName, () => {
  return spec;
}) as CustomTextInputComponent;

export const Commands = {
  setTextAndSelection(
    ref: ElementRef<CustomTextInputComponent>,
    mostRecentEventCount: number,
    text: string | undefined,
    start: number,
    end: number,
  ) {
    dispatchCommand(ref, 'setTextAndSelection', [
      mostRecentEventCount,
      text,
      start,
      end,
    ]);
  },
  setFormattedText(
    viewRef: ElementRef<CustomTextInputComponent>,
    json: IDelta,
  ) {
    dispatchCommand(viewRef, 'setFormattedText', [json]);
  },

  setLightOn: function setLightOn(ref: any, value: boolean) {
    dispatchCommand(ref, 'setLightOn', [value]);
  },
};
