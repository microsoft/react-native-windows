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
import {unstable_TextAncestorContext} from 'react-native';
export {Commands as CustomTextInputCommands};

const CustomTextInput = React.forwardRef<
  React.ElementRef<typeof CustomTextInputSpec>,
  CustomTextInputProps
>((props, ref) => {
  const {
    children,
    value,
    defaultValue,
    onChange,
    onChangeText,
    onSelectionChange,
    ...rest
  } = props;
const inputRef = React.useRef<React.ElementRef<
    typeof CustomTextInputSpec
  > | null>(null);
  const [mostRecentEventCount, setMostRecentEventCount] = React.useState(0);
  const [lastNativeText, setLastNativeText] = React.useState(value);
  const [lastNativeSelection, setLastNativeSelection] = React.useState({
    start: -1,
    end: -1,
  });
  const text = typeof value === 'string' ? value : defaultValue;
  const selection = React.useMemo(
    () =>
      props.selection == null
        ? null
        : {
            start: props.selection.start,
            end: props.selection.end ?? props.selection.start,
          },
    [props.selection],
  );

  const setRef = React.useCallback(
    (instance: React.ElementRef<typeof CustomTextInputSpec> | null) => {
      inputRef.current = instance;
      if (typeof ref === 'function') {
        ref(instance);
      } else if (ref) {
        ref.current = instance;
      }
    },
    [ref],
  );

  React.useLayoutEffect(() => {
    const textChanged = typeof value === 'string' && lastNativeText !== value;
    const selectionChanged =
      selection != null &&
      (selection.start !== lastNativeSelection.start ||
        selection.end !== lastNativeSelection.end);
    if (!textChanged && !selectionChanged) {
      return;
    }

    if (textChanged) {
      setLastNativeText(value);
    }
    if (selectionChanged) {
      setLastNativeSelection(selection);
    }
    if (inputRef.current != null) {
      Commands.setTextAndSelection(
        inputRef.current,
        mostRecentEventCount,
        text,
        selection?.start ?? -1,
        selection?.end ?? -1,
      );
    }
  }, [
    lastNativeSelection,
    lastNativeText,
    mostRecentEventCount,
    selection,
    text,
    value,
  ]);

  return (
    <CustomTextInputSpec
      ref={setRef}
      {...rest}
      text={text}
      selection={selection ?? undefined}
      mostRecentEventCount={mostRecentEventCount}
      onChange={event => {
        onChange?.(event);
        onChangeText?.(event.nativeEvent.text);
        if (inputRef.current == null) {
          return;
        }
        setLastNativeText(event.nativeEvent.text);
        setMostRecentEventCount(event.nativeEvent.eventCount);
      }}
      onSelectionChange={event => {
        onSelectionChange?.(event);
        if (inputRef.current != null) {
          setLastNativeSelection(event.nativeEvent.selection);
        }
      }}
      focusable={props.focusable !== false}>
      <unstable_TextAncestorContext.Provider value={true}>
        {children}
      </unstable_TextAncestorContext.Provider>
    </CustomTextInputSpec>
  );
});

export default CustomTextInput;
