/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @flow strict-local
 * @format
 */

// flowlint unsafe-getters-setters:off
// flowlint unclear-type:off
// flowlint untyped-import:off

'use strict';
'use client';

// [[Windows
// APIs
import typeof ActionSheetIOS from './Libraries/ActionSheetIOS/ActionSheetIOS';
import typeof Alert from './Libraries/Alert/Alert';
import typeof Animated from './Libraries/Animated/Animated';
import typeof * as AnimatedModule from './Libraries/Animated/Animated';
import typeof Easing from './Libraries/Animated/Easing';
import typeof useAnimatedValue from './Libraries/Animated/useAnimatedValue';
import typeof AppState from './Libraries/AppState/AppState';
import typeof NativeModules from './Libraries/BatchedBridge/NativeModules';
import typeof codegenNativeCommands from './Libraries/Utilities/codegenNativeCommands';
import typeof codegenNativeComponent from './Libraries/Utilities/codegenNativeComponent';
import {findNodeHandle} from './Libraries/ReactNative/RendererProxy';
import {unstable_batchedUpdates} from './Libraries/ReactNative/RendererProxy';
// Components
import typeof AccessibilityInfo from './Libraries/Components/AccessibilityInfo/AccessibilityInfo';
import typeof ActivityIndicator from './Libraries/Components/ActivityIndicator/ActivityIndicator';
// [Windows
import typeof AppTheme from './Libraries/AppTheme/AppTheme';
// Windows]
import typeof Button from './Libraries/Components/Button';
import typeof Clipboard from './Libraries/Components/Clipboard/Clipboard';
import typeof DrawerLayoutAndroid from './Libraries/Components/DrawerAndroid/DrawerLayoutAndroid';
import typeof Keyboard from './Libraries/Components/Keyboard/Keyboard';
import typeof KeyboardAvoidingView from './Libraries/Components/Keyboard/KeyboardAvoidingView';
import typeof LayoutConformance from './Libraries/Components/LayoutConformance/LayoutConformance';
import typeof Pressable from './Libraries/Components/Pressable/Pressable';
import typeof ProgressBarAndroid from './Libraries/Components/ProgressBarAndroid/ProgressBarAndroid';
import typeof RefreshControl from './Libraries/Components/RefreshControl/RefreshControl';
import typeof SafeAreaView from './Libraries/Components/SafeAreaView/SafeAreaView';
import typeof ScrollView from './Libraries/Components/ScrollView/ScrollView';
import typeof StatusBar from './Libraries/Components/StatusBar/StatusBar';
import typeof Switch from './Libraries/Components/Switch/Switch';
import typeof InputAccessoryView from './Libraries/Components/TextInput/InputAccessoryView';
import TextInput from './Libraries/Components/TextInput/TextInput';
import typeof ToastAndroid from './Libraries/Components/ToastAndroid/ToastAndroid';
import typeof Touchable from './Libraries/Components/Touchable/Touchable';
import typeof TouchableHighlight from './Libraries/Components/Touchable/TouchableHighlight';
import typeof TouchableNativeFeedback from './Libraries/Components/Touchable/TouchableNativeFeedback';
import typeof TouchableOpacity from './Libraries/Components/Touchable/TouchableOpacity';
import typeof TouchableWithoutFeedback from './Libraries/Components/Touchable/TouchableWithoutFeedback';
import typeof View from './Libraries/Components/View/View';
import typeof registerCallableModule from './Libraries/Core/registerCallableModule';
import typeof NativeEventEmitter from './Libraries/EventEmitter/NativeEventEmitter';
import typeof RCTDeviceEventEmitter from './Libraries/EventEmitter/RCTDeviceEventEmitter';
import typeof RCTNativeAppEventEmitter from './Libraries/EventEmitter/RCTNativeAppEventEmitter';
import typeof Image from './Libraries/Image/Image';
import typeof ImageBackground from './Libraries/Image/ImageBackground';
import typeof InteractionManager from './Libraries/Interaction/InteractionManager';
import typeof PanResponder from './Libraries/Interaction/PanResponder';
import typeof LayoutAnimation from './Libraries/LayoutAnimation/LayoutAnimation';
import typeof Linking from './Libraries/Linking/Linking';
import typeof FlatList from './Libraries/Lists/FlatList';
import typeof SectionList from './Libraries/Lists/SectionList';
import typeof VirtualizedList from './Libraries/Lists/VirtualizedList';
import typeof VirtualizedSectionList from './Libraries/Lists/VirtualizedSectionList';
import typeof LogBox from './Libraries/LogBox/LogBox';
import typeof Modal from './Libraries/Modal/Modal';
// $FlowFixMe[invalid-exported-annotation]
import typeof NativeDialogManagerAndroid from './Libraries/NativeModules/specs/NativeDialogManagerAndroid';
import typeof Networking from './Libraries/Network/RCTNetworking';
import typeof * as Systrace from './Libraries/Performance/Systrace';
import typeof PermissionsAndroid from './Libraries/PermissionsAndroid/PermissionsAndroid';
import typeof PushNotificationIOS from './Libraries/PushNotificationIOS/PushNotificationIOS';
import typeof {AppRegistry} from './Libraries/ReactNative/AppRegistry';
import typeof I18nManager from './Libraries/ReactNative/I18nManager';
import typeof {RootTagContext} from './Libraries/ReactNative/RootTag';
import typeof UIManager from './Libraries/ReactNative/UIManager';
import typeof ReactNative from './Libraries/Renderer/shims/ReactNative';
import type {HostComponent} from './src/private/types/HostComponent';
import type {HostInstance} from './src/private/types/HostInstance';
import typeof Settings from './Libraries/Settings/Settings';
import typeof Share from './Libraries/Share/Share';
import typeof {PlatformColor} from './Libraries/StyleSheet/PlatformColorValueTypes';
// Plugins
import typeof {DynamicColorIOS} from './Libraries/StyleSheet/PlatformColorValueTypesIOS';
import typeof processColor from './Libraries/StyleSheet/processColor';
import typeof StyleSheet from './Libraries/StyleSheet/StyleSheet';
import typeof Text from './Libraries/Text/Text';
import typeof * as TurboModuleRegistry from './Libraries/TurboModule/TurboModuleRegistry';
import typeof UTFSequence from './Libraries/UTFSequence';
import typeof * as Appearance from './Libraries/Utilities/Appearance';
import typeof BackHandler from './Libraries/Utilities/BackHandler';
import typeof DeviceInfo from './Libraries/Utilities/DeviceInfo';
import typeof DevSettings from './Libraries/Utilities/DevSettings';
import typeof Dimensions from './Libraries/Utilities/Dimensions';
import typeof PixelRatio from './Libraries/Utilities/PixelRatio';
import typeof Platform from './Libraries/Utilities/Platform';
import typeof useColorScheme from './Libraries/Utilities/useColorScheme';
import typeof useWindowDimensions from './Libraries/Utilities/useWindowDimensions';
import typeof Vibration from './Libraries/Vibration/Vibration';
import typeof DevMenu from './src/private/devsupport/devmenu/DevMenu';

export type {HostComponent, HostInstance};
// Windows]]

const warnOnce = require('./Libraries/Utilities/warnOnce').default;
const invariant = require('invariant');

module.exports = {
  // #region Components
  get ActivityIndicator(): ActivityIndicator {
    return require('./Libraries/Components/ActivityIndicator/ActivityIndicator')
      .default;
  },
  get Button(): Button {
    return require('./Libraries/Components/Button').default;
  },
  // $FlowFixMe[value-as-type]
  get DrawerLayoutAndroid(): DrawerLayoutAndroid {
    return require('./Libraries/Components/DrawerAndroid/DrawerLayoutAndroid')
      .default;
  },
  get FlatList(): FlatList {
    return require('./Libraries/Lists/FlatList').default;
  },
  get Image(): Image {
    return require('./Libraries/Image/Image').default;
  },
  get ImageBackground(): ImageBackground {
    return require('./Libraries/Image/ImageBackground').default;
  },
  get InputAccessoryView(): InputAccessoryView {
    return require('./Libraries/Components/TextInput/InputAccessoryView')
      .default;
  },
  get KeyboardAvoidingView(): KeyboardAvoidingView {
    return require('./Libraries/Components/Keyboard/KeyboardAvoidingView')
      .default;
  },
  get experimental_LayoutConformance(): LayoutConformance {
    return require('./Libraries/Components/LayoutConformance/LayoutConformance')
      .default;
  },
  get Modal(): Modal {
    return require('./Libraries/Modal/Modal').default;
  },
  get Pressable(): Pressable {
    return require('./Libraries/Components/Pressable/Pressable').default;
  },
  // $FlowFixMe[value-as-type]
  get ProgressBarAndroid(): ProgressBarAndroid {
    warnOnce(
      'progress-bar-android-moved',
      'ProgressBarAndroid has been extracted from react-native core and will be removed in a future release. ' +
        "It can now be installed and imported from '@react-native-community/progress-bar-android' instead of 'react-native'. " +
        'See https://github.com/react-native-progress-view/progress-bar-android',
    );
    return require('./Libraries/Components/ProgressBarAndroid/ProgressBarAndroid')
      .default;
  },
  get RefreshControl(): RefreshControl {
    return require('./Libraries/Components/RefreshControl/RefreshControl')
      .default;
  },
  /**
   * @deprecated SafeAreaView has been deprecated and will be removed in a future release.
   * Please use 'react-native-safe-area-context' instead.
   * See https://github.com/th3rdwave/react-native-safe-area-context
   */
  get SafeAreaView(): SafeAreaView {
    warnOnce(
      'safe-area-view-deprecated',
      'SafeAreaView has been deprecated and will be removed in a future release. ' +
        "Please use 'react-native-safe-area-context' instead. " +
        'See https://github.com/th3rdwave/react-native-safe-area-context',
    );
    return require('./Libraries/Components/SafeAreaView/SafeAreaView').default;
  },
  get ScrollView(): ScrollView {
    return require('./Libraries/Components/ScrollView/ScrollView').default;
  },
  get SectionList(): SectionList {
    return require('./Libraries/Lists/SectionList').default;
  },
  get StatusBar(): StatusBar {
    return require('./Libraries/Components/StatusBar/StatusBar').default;
  },
  get Switch(): Switch {
    return require('./Libraries/Components/Switch/Switch').default;
  },
  get Text(): Text {
    return require('./Libraries/Text/Text').default;
  },
  get TextInput(): typeof TextInput {
    return require('./Libraries/Components/TextInput/TextInput').default;
  },
  get Touchable(): Touchable {
    return require('./Libraries/Components/Touchable/Touchable').default;
  },
  get TouchableHighlight(): TouchableHighlight {
    return require('./Libraries/Components/Touchable/TouchableHighlight')
      .default;
  },
  get TouchableNativeFeedback(): TouchableNativeFeedback {
    return require('./Libraries/Components/Touchable/TouchableNativeFeedback')
      .default;
  },
  get TouchableOpacity(): TouchableOpacity {
    return require('./Libraries/Components/Touchable/TouchableOpacity').default;
  },
  get TouchableWithoutFeedback(): TouchableWithoutFeedback {
    return require('./Libraries/Components/Touchable/TouchableWithoutFeedback')
      .default;
  },
  get View(): View {
    return require('./Libraries/Components/View/View').default;
  },
  get VirtualizedList(): VirtualizedList {
    return require('./Libraries/Lists/VirtualizedList').default;
  },
  get VirtualizedSectionList(): VirtualizedSectionList {
    return require('./Libraries/Lists/VirtualizedSectionList').default;
  },
  // #endregion
  // #region APIs
  get AccessibilityInfo(): AccessibilityInfo {
    return require('./Libraries/Components/AccessibilityInfo/AccessibilityInfo')
      .default;
  },
  get ActionSheetIOS(): ActionSheetIOS {
    return require('./Libraries/ActionSheetIOS/ActionSheetIOS').default;
  },
  get Alert(): Alert {
    return require('./Libraries/Alert/Alert').default;
  },
  // Include any types exported in the Animated module together with its default export, so
  // you can references types such as Animated.Numeric
  get Animated(): Animated {
    return require('./Libraries/Animated/Animated').default;
  },
  get Appearance(): Appearance {
    return require('./Libraries/Utilities/Appearance');
  },
  get AppRegistry(): AppRegistry {
    return require('./Libraries/ReactNative/AppRegistry').AppRegistry;
  },
  get AppState(): AppState {
    return require('./Libraries/AppState/AppState').default;
  },
  get BackHandler(): BackHandler {
    return require('./Libraries/Utilities/BackHandler').default;
  },
  get Clipboard(): Clipboard {
    warnOnce(
      'clipboard-moved',
      'Clipboard has been extracted from react-native core and will be removed in a future release. ' +
        "It can now be installed and imported from '@react-native-clipboard/clipboard' instead of 'react-native'. " +
        'See https://github.com/react-native-clipboard/clipboard',
    );
    return require('./Libraries/Components/Clipboard/Clipboard').default;
  },
  get codegenNativeCommands(): codegenNativeCommands<any> {
    return require('./Libraries/Utilities/codegenNativeCommands').default;
  },
  get codegenNativeComponent(): codegenNativeComponent<any> {
    return require('./Libraries/Utilities/codegenNativeComponent').default;
  },
  get DeviceEventEmitter(): RCTDeviceEventEmitter {
    return require('./Libraries/EventEmitter/RCTDeviceEventEmitter').default;
  },
  get DeviceInfo(): DeviceInfo {
    return require('./Libraries/Utilities/DeviceInfo').default;
  },
  get DevMenu(): DevMenu {
    return require('./src/private/devsupport/devmenu/DevMenu').default;
  },
  get DevSettings(): DevSettings {
    return require('./Libraries/Utilities/DevSettings').default;
  },
  get Dimensions(): Dimensions {
    return require('./Libraries/Utilities/Dimensions').default;
  },
  get DynamicColorIOS(): DynamicColorIOS {
    return require('./Libraries/StyleSheet/PlatformColorValueTypesIOS')
      .DynamicColorIOS;
  },
  get Easing(): Easing {
    return require('./Libraries/Animated/Easing').default;
  },
  get findNodeHandle(): typeof findNodeHandle {
    return require('./Libraries/ReactNative/RendererProxy').findNodeHandle;
  },
  get I18nManager(): I18nManager {
    return require('./Libraries/ReactNative/I18nManager').default;
  },
  get InteractionManager(): InteractionManager {
    return require('./Libraries/Interaction/InteractionManager').default;
  },
  get Keyboard(): Keyboard {
    return require('./Libraries/Components/Keyboard/Keyboard').default;
  },
  get LayoutAnimation(): LayoutAnimation {
    return require('./Libraries/LayoutAnimation/LayoutAnimation').default;
  },
  get Linking(): Linking {
    return require('./Libraries/Linking/Linking').default;
  },
  get LogBox(): LogBox {
    return require('./Libraries/LogBox/LogBox').default;
  },
  get NativeAppEventEmitter(): RCTNativeAppEventEmitter {
    return require('./Libraries/EventEmitter/RCTNativeAppEventEmitter').default;
  },
  get NativeDialogManagerAndroid(): NativeDialogManagerAndroid {
    return require('./Libraries/NativeModules/specs/NativeDialogManagerAndroid')
      .default;
  },
  get NativeEventEmitter(): NativeEventEmitter {
    return require('./Libraries/EventEmitter/NativeEventEmitter').default;
  },
  get NativeModules(): NativeModules {
    return require('./Libraries/BatchedBridge/NativeModules').default;
  },
  get Networking(): Networking {
    return require('./Libraries/Network/RCTNetworking').default;
  },
  get PanResponder(): PanResponder {
    return require('./Libraries/Interaction/PanResponder').default;
  },
  get PermissionsAndroid(): PermissionsAndroid {
    return require('./Libraries/PermissionsAndroid/PermissionsAndroid').default;
  },
  get PixelRatio(): PixelRatio {
    return require('./Libraries/Utilities/PixelRatio').default;
  },
  get Platform(): Platform {
    return require('./Libraries/Utilities/Platform').default;
  },
  get PlatformColor(): PlatformColor {
    return require('./Libraries/StyleSheet/PlatformColorValueTypes')
      .PlatformColor;
  },
  get PushNotificationIOS(): PushNotificationIOS {
    warnOnce(
      'pushNotificationIOS-moved',
      'PushNotificationIOS has been extracted from react-native core and will be removed in a future release. ' +
        "It can now be installed and imported from '@react-native-community/push-notification-ios' instead of 'react-native'. " +
        'See https://github.com/react-native-push-notification/ios',
    );
    return require('./Libraries/PushNotificationIOS/PushNotificationIOS')
      .default;
  },
  get processColor(): processColor {
    return require('./Libraries/StyleSheet/processColor').default;
  },
  get registerCallableModule(): registerCallableModule {
    return require('./Libraries/Core/registerCallableModule').default;
  },
  get requireNativeComponent(): <T: {...}>(
    uiViewClassName: string,
  ) => HostComponent<T> {
    return require('./Libraries/ReactNative/requireNativeComponent').default;
  },
  get RootTagContext(): RootTagContext {
    return require('./Libraries/ReactNative/RootTag').RootTagContext;
  },
  get Settings(): Settings {
    return require('./Libraries/Settings/Settings').default;
  },
  get Share(): Share {
    return require('./Libraries/Share/Share').default;
  },
  get StyleSheet(): StyleSheet {
    return require('./Libraries/StyleSheet/StyleSheet').default;
  },
  get Systrace(): Systrace {
    return require('./Libraries/Performance/Systrace');
  },
  // $FlowFixMe[value-as-type]
  get ToastAndroid(): ToastAndroid {
    return require('./Libraries/Components/ToastAndroid/ToastAndroid').default;
  },
  get TurboModuleRegistry(): TurboModuleRegistry {
    return require('./Libraries/TurboModule/TurboModuleRegistry');
  },
  get UIManager(): UIManager {
    return require('./Libraries/ReactNative/UIManager').default;
  },
  get unstable_batchedUpdates(): typeof unstable_batchedUpdates {
    return require('./Libraries/ReactNative/RendererProxy')
      .unstable_batchedUpdates;
  },
  get unstable_enableLogBox(): () => void {
    return () =>
      console.warn(
        'LogBox is enabled by default so there is no need to call unstable_enableLogBox() anymore. This is a no op and will be removed in the next version.',
      );
  },
  get useAnimatedValue(): useAnimatedValue {
    return require('./Libraries/Animated/useAnimatedValue').default;
  },
  get useColorScheme(): useColorScheme {
    return require('./Libraries/Utilities/useColorScheme').default;
  },
  get useWindowDimensions(): useWindowDimensions {
    return require('./Libraries/Utilities/useWindowDimensions').default;
  },
  get UTFSequence(): UTFSequence {
    return require('./Libraries/UTFSequence').default;
  },
  get Vibration(): Vibration {
    return require('./Libraries/Vibration/Vibration').default;
  },
  // Additional windows exports (Typescript components exported as flow any)
  get DatePicker(): any {
    invariant(
      false,
      'DatePicker has been extracted and removed from react-native-windows. ' +
        "It can now be installed and imported as DateTimePicker from '@react-native-community/datetimepicker'. " +
        'See https://github.com/react-native-datetimepicker/datetimepicker',
    );
  },
  get Flyout(): any {
    return require('./Libraries/Components/Flyout/Flyout').Flyout;
  },
  get Glyph(): any {
    return require('./Libraries/Components/Glyph/Glyph').Glyph;
  },
  get PickerWindows(): any {
    invariant(
      false,
      'PickerWindows has been extracted and removed from react-native-windows. ' +
        "It can now be installed and imported as Picker from '@react-native-picker/picker'. " +
        'See https://github.com/react-native-picker/picker',
    );
  },
  get Popup(): any {
    return require('./Libraries/Components/Popup/Popup').Popup;
  },
  get supportKeyboard(): any {
    return require('./Libraries/Components/Keyboard/KeyboardExt')
      .supportKeyboard;
  },
  get DayOfWeek(): any {
    invariant(
      false,
      'DatePicker has been extracted and removed from react-native-windows. ' +
        "It can now be installed and imported as DateTimePicker from '@react-native-community/datetimepicker'. " +
        'See https://github.com/react-native-datetimepicker/datetimepicker',
    );
  },
  get EventPhase(): any {
    return require('./Libraries/Components/View/ViewPropTypes')
      .HandledEventPhase;
  },
  get HandledEventPhase(): any {
    return require('./Libraries/Components/View/ViewPropTypes')
      .HandledEventPhase;
  },
  get ViewWindows(): any {
    return require('./Libraries/Components/View/View').default;
  },
  get AppTheme(): AppTheme {
    return require('./Libraries/AppTheme/AppTheme');
  },
};

if (__DEV__) {
  /* $FlowFixMe[prop-missing] This is intentional: Flow will error when
   * attempting to access AsyncStorage. */
  /* $FlowFixMe[invalid-export] This is intentional: Flow will error when
   * attempting to access AsyncStorage. */
  Object.defineProperty(module.exports, 'AsyncStorage', {
    configurable: true,
    get() {
      invariant(
        false,
        'AsyncStorage has been removed from react-native core. ' +
          "It can now be installed and imported from '@react-native-async-storage/async-storage' instead of 'react-native'. " +
          'See https://github.com/react-native-async-storage/async-storage',
      );
    },
  });
  /* $FlowFixMe[prop-missing] This is intentional: Flow will error when
   * attempting to access ImagePickerIOS. */
  /* $FlowFixMe[invalid-export] This is intentional: Flow will error when
   * attempting to access ImagePickerIOS. */
  Object.defineProperty(module.exports, 'ImagePickerIOS', {
    configurable: true,
    get() {
      invariant(
        false,
        'ImagePickerIOS has been removed from React Native. ' +
          "Please upgrade to use either 'react-native-image-picker' or 'expo-image-picker'. " +
          "If you cannot upgrade to a different library, please install the deprecated '@react-native-community/image-picker-ios' package. " +
          'See https://github.com/rnc-archive/react-native-image-picker-ios',
      );
    },
  });
  /* $FlowFixMe[prop-missing] This is intentional: Flow will error when
   * attempting to access ProgressViewIOS. */
  /* $FlowFixMe[invalid-export] This is intentional: Flow will error when
   * attempting to access ProgressViewIOS. */
  Object.defineProperty(module.exports, 'ProgressViewIOS', {
    configurable: true,
    get() {
      invariant(
        false,
        'ProgressViewIOS has been removed from react-native core. ' +
          "It can now be installed and imported from '@react-native-community/progress-view' instead of 'react-native'. " +
          'See https://github.com/react-native-progress-view/progress-view',
      );
    },
  });
  /* $FlowFixMe[prop-missing] This is intentional: Flow will error when
   * attempting to access DatePickerIOS. */
  /* $FlowFixMe[invalid-export] This is intentional: Flow will error when
   * attempting to access DatePickerIOS. */
  Object.defineProperty(module.exports, 'DatePickerIOS', {
    configurable: true,
    get() {
      invariant(
        false,
        'DatePickerIOS has been removed from react-native core. ' +
          "It can now be installed and imported from '@react-native-community/datetimepicker' instead of 'react-native'. " +
          'See https://github.com/react-native-datetimepicker/datetimepicker',
      );
    },
  });
  /* $FlowFixMe[prop-missing] This is intentional: Flow will error when
   * attempting to access Slider. */
  /* $FlowFixMe[invalid-export] This is intentional: Flow will error when
   * attempting to access Slider. */
  Object.defineProperty(module.exports, 'Slider', {
    configurable: true,
    get() {
      invariant(
        false,
        'Slider has been removed from react-native core. ' +
          "It can now be installed and imported from '@react-native-community/slider' instead of 'react-native'. " +
          'See https://github.com/callstack/react-native-slider',
      );
    },
  });
}
