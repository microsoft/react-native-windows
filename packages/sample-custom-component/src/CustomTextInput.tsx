/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import React from 'react';
import CustomTextInputSpec, {
  Commands,
  CustomTextInputProps,
} from './CustomTextInputSpec';
import {
  findNodeHandle,
  InternalTextInputWithCustomType,
} from 'react-native-windows';
export {Commands as CustomTextInputCommands};

export interface ICustomTextInput {
  setFormattedText(delta: string): void;
  setLightOn(value: boolean): void;
}

export type CustomTextInputInstance = React.ComponentRef<
  typeof CustomTextInputSpec
> &
  ICustomTextInput;

function useCustomTextInputNativeRefForwarded(
  ref: React.ForwardedRef<CustomTextInputInstance>,
  nativeRef: React.RefObject<any>,
) {
  const nativeRefHandle = React.useRef<number | null>(null);
  React.useLayoutEffect(() => {
    if (nativeRef.current) {
      nativeRefHandle.current = findNodeHandle(nativeRef.current);
      if (!nativeRefHandle.current) {
        console.warn(
          'findNodeHandle returned null for nativeRef; imperative CustomTextInput methods will not work',
        );
      }
    } else {
      console.warn(
        'nativeRef is not available to findNodeHandle; imperative CustomTextInput methods will not work',
      );
    }
  }, [nativeRef]);

  React.useImperativeHandle(
    ref,
    () => {
      if (!nativeRef.current) {
        throw new Error(
          'NativeCustomTextInput ref is not available to create the imperative handle',
        );
      }
      return Object.assign(nativeRef.current, {
        // PickerHost methods
        setFormattedText: (delta: string) => {
          if (nativeRefHandle.current) {
            Commands.setFormattedText(nativeRef.current, delta);
          }
        },
        setLightOn: (value: boolean) => {
          if (nativeRefHandle.current) {
            Commands.setLightOn(nativeRef.current, value);
          }
        },
      });
    },
    [nativeRefHandle, nativeRef],
  );
}

const InternalCustomTextInput = (
  props: React.ComponentPropsWithRef<typeof CustomTextInputSpec>,
) => InternalTextInputWithCustomType(props, CustomTextInputSpec);

const CustomTextInput = React.forwardRef<
  CustomTextInputInstance,
  CustomTextInputProps
>(
  (
    {
      allowFontScaling = true,
      rejectResponderTermination = true,
      underlineColorAndroid = 'transparent',
      readOnly,
      editable,
      enterKeyHint,
      returnKeyType,
      inputMode,
      showSoftInputOnFocus,
      keyboardType,
      ...restProps
    },
    ref,
  ) => {
    const nativeRef = React.useRef(null);
    useCustomTextInputNativeRefForwarded(ref, nativeRef);

    return (
      <InternalCustomTextInput
        allowFontScaling={allowFontScaling}
        rejectResponderTermination={rejectResponderTermination}
        underlineColorAndroid={underlineColorAndroid}
        editable={readOnly !== undefined ? !readOnly : editable}
        returnKeyType={
          enterKeyHint
            ? enterKeyHintToReturnTypeMap[enterKeyHint]
            : returnKeyType
        }
        keyboardType={
          inputMode ? inputModeToKeyboardTypeMap[inputMode] : keyboardType
        }
        showSoftInputOnFocus={
          inputMode == null ? showSoftInputOnFocus : inputMode !== 'none'
        }
        {...restProps}
        forwardedRef={nativeRef}
      />
    );
  },
);

const enterKeyHintToReturnTypeMap = {
  enter: 'default',
  done: 'done',
  go: 'go',
  next: 'next',
  previous: 'previous',
  search: 'search',
  send: 'send',
} as const;

const inputModeToKeyboardTypeMap = {
  none: 'default',
  text: 'default',
  decimal: 'decimal-pad',
  numeric: 'number-pad',
  tel: 'phone-pad',
  search: 'default',
  email: 'email-address',
  url: 'url',
} as const;

export default CustomTextInput;
