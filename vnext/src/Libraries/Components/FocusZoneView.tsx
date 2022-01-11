import React, {useContext} from 'react';
import {Platform} from 'react-native';
import {FocusZoneViewNative, Props} from './FocusZoneViewNative';

const IsFocusZoneNestedContext = React.createContext(false);

const IS_MACOS = Platform.OS === 'macos';

export default function FocusZoneView({
  focusDirection = 'vertical',
  xyFocusNavigationStrategy,
  ...props
}: Props) {
  // Nesting FocusZoneViews on Windows leads to some navigation issues, so warn.
  const isFocusZoneNested = useContext(IsFocusZoneNestedContext);
  if (isFocusZoneNested) {
    console.warn('Nested FocusZoneViews are not supported.');
  }

  const focusZoneViewNativeProps = IS_MACOS
    ? {focusDirection}
    : {xyFocusNavigationStrategy};

  return (
    <IsFocusZoneNestedContext.Provider value={true}>
      <FocusZoneViewNative {...focusZoneViewNativeProps} {...props} />
    </IsFocusZoneNestedContext.Provider>
  );
}
