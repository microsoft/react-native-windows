import ReactNativeViewAttributes from '../Components/View/ReactNativeViewAttributes';
import UIManager from '../ReactNative/UIManager';
import {type HostComponent} from '../Renderer/shims/ReactNativeTypes';
import createReactNativeComponentClass from '../Renderer/shims/createReactNativeComponentClass';
import {type ProcessedColorValue} from '../StyleSheet/processColor';
import {type TextProps} from './TextProps';
import {type PressEvent} from '../Types/CoreEventTypes';

type NativeTextProps = $ReadOnly<{
  ...TextProps,
  isHighlighted?: ?boolean,
  selectionColor?: ?ProcessedColorValue,
  onClick?: ?(event: PressEvent) => mixed,
  // This is only needed for platforms that optimize text hit testing, e.g.,
  // react-native-windows. It can be used to only hit test virtual text spans
  // that have pressable events attached to them.
  isPressable?: ?boolean,
}>;

export const NativeText: HostComponent<NativeTextProps> =
  (createReactNativeComponentClass('RCTText', () => ({
    validAttributes: {
      ...ReactNativeViewAttributes.UIView,
      isHighlighted: true,
      isPressable: true,
      numberOfLines: true,
      ellipsizeMode: true,
      allowFontScaling: true,
      maxFontSizeMultiplier: true,
      disabled: true,
      selectable: true,
      selectionColor: true,
      adjustsFontSizeToFit: true,
      minimumFontScale: true,
      textBreakStrategy: true,
      onTextLayout: true,
      onInlineViewLayout: true,
      dataDetectorType: true,
      android_hyphenationFrequency: true,
      // [Windows
      // Listed for the property to be honored even though
      // the property tooltip and the events from ViewWin32
      //work without being listed.  Any Text-specific events
      // would need to be listed here.
      focusable: true,
      // Windows]
    },
    directEventTypes: {
      topTextLayout: {
        registrationName: 'onTextLayout',
      },
      topInlineViewLayout: {
        registrationName: 'onInlineViewLayout',
      },
    },
    uiViewClassName: 'RCTText',
  })): any);

export const NativeVirtualText: HostComponent<NativeTextProps> =
  !global.RN$Bridgeless &&
  UIManager.getViewManagerConfig('RCTVirtualText') == null
    ? NativeText
    : (createReactNativeComponentClass('RCTVirtualText', () => ({
        validAttributes: {
          ...ReactNativeViewAttributes.UIView,
          isHighlighted: true,
          isPressable: true,
          maxFontSizeMultiplier: true,
        },
        uiViewClassName: 'RCTVirtualText',
      })): any);
