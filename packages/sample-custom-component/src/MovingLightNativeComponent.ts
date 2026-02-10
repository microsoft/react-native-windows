import { codegenNativeComponent, codegenNativeCommands } from 'react-native';
import type { ColorValue, HostComponent, ViewProps } from 'react-native';

import type {
  DirectEventHandler,
  Float,
  Double,
  Int32,
  WithDefault,
  UnsafeMixed,
} from 'react-native/Libraries/Types/CodegenTypes';

export type SomethingEvent = {
  value: string,
  target: Int32,
};

export interface MovingLightProps extends ViewProps {
  // Props
  size?: WithDefault<Float, 42>;
  color?: ColorValue;
  testMixed?: UnsafeMixed;
  eventParam?: string;
  objectProp?: { number: Double, string: string };

  // Events
  onSomething?: DirectEventHandler<SomethingEvent>,
  onTestObjectEvent?: DirectEventHandler<{ target: Int32; testObject: UnsafeMixed }>;
  onEventWithInlineTypes?: DirectEventHandler<{ target: Int32; contentInset: { top: Double; bottom: Double; left: Double; right: Double }; contentOffset: { x: Double; y: Double }; contentSize: { width: Double; height: Double }; layoutMeasurement: { width: Double; height: Double }; velocity: { x: Double; y: Double }; isUserTriggered: boolean }>;
  onEventWithMultipleAliasTypes?: DirectEventHandler<{ target: Int32; contentInset: { top: Double; bottom: Double; left: Double; right: Double }; contentOffset: { x: Double; y: Double }; contentSize: { width: Double; height: Double }; layoutMeasurement: { width: Double; height: Double }; velocity: { x: Double; y: Double }; isUserTriggered: boolean }>;
}


type ComponentType = HostComponent<MovingLightProps>;

interface NativeCommands {
  setLightOn: (viewRef: React.ElementRef<ComponentType>, value: boolean) => void;
}

export const Commands: NativeCommands = codegenNativeCommands<NativeCommands>({
  supportedCommands: ['setLightOn'],
});

export default codegenNativeComponent<MovingLightProps>('MovingLight');
