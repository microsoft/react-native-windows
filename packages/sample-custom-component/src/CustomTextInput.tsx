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
import {InternalTextInputWithCustomType} from 'react-native-windows';
export {Commands as CustomTextInputCommands};

function useRefEffect<Instance>(
  refEffect: (current: Instance) => void | (() => void),
): React.Ref<Instance> {
  const cleanupRef = React.useRef<void | (() => void)>(undefined);
  return React.useCallback(
    (current: Instance) => {
      cleanupRef.current?.();
      cleanupRef.current = refEffect(current);
      return () => {
        cleanupRef.current?.();
        cleanupRef.current = undefined;
      };
    },
    [refEffect],
  );
}

function useMergeRefs<Instance>(
  // @ts-expect-error
  ...refs
): React.Ref<Instance> {
  const refEffect = React.useCallback(
    (current: Instance) => {
      const cleanups: ReadonlyArray<void | (() => void)> = refs.map(ref => {
        if (ref == null) {
          return undefined;
        } else {
          if (typeof ref === 'function') {
            const cleanup: void | (() => void) = ref(current);
            return typeof cleanup === 'function'
              ? cleanup
              : () => {
                  ref(null);
                };
          } else {
            ref.current = current;
            return () => {
              ref.current = null;
            };
          }
        }
      });

      return () => {
        for (const cleanup of cleanups) {
          cleanup?.();
        }
      };
    },
    [...refs], // eslint-disable-line react-hooks/exhaustive-deps
  );
  return useRefEffect(refEffect);
}

export interface ICustomTextInput {
  setFormattedText(delta: string): void;
  setLightOn(value: boolean): void;
}

export type CustomTextInputInstance = React.ComponentRef<
  typeof CustomTextInputSpec
> &
  ICustomTextInput;

const InternalCustomTextInput = (
  props: CustomTextInputProps & {
    forwardedRef?: React.Ref<CustomTextInputInstance>;
  },
) => InternalTextInputWithCustomType(props, CustomTextInputSpec as any);

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
    const nativeRef = React.useRef<CustomTextInputInstance>(null);

    const setLocalRef = React.useCallback(
      (instance: CustomTextInputInstance | null) => {
        nativeRef.current = instance;
        if (instance != null) {
          Object.assign(instance, {
            setFormattedText: (delta: string) => {
              Commands.setFormattedText(instance, delta);
            },
            setLightOn: (value: boolean) => {
              Commands.setLightOn(instance, value);
            },
          });
        }
      },
      [],
    );

    const forwardedRef = useMergeRefs<CustomTextInputInstance>(
      setLocalRef,
      ref,
    );

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
        forwardedRef={forwardedRef as any}
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
