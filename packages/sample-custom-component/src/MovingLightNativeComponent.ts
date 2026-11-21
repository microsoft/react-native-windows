import { codegenNativeComponent, codegenNativeCommands } from 'react-native';
import type { CodegenTypes, ColorValue, HostComponent, ViewProps } from 'react-native';

export type SomethingEvent = {
  value: string,
  target: CodegenTypes.Int32,
};

export interface MovingLightProps extends ViewProps {
  // Props
  size?: CodegenTypes.WithDefault<CodegenTypes.Float, 42>;
  color?: ColorValue;
  testMixed?: CodegenTypes.UnsafeMixed;
  eventParam?: string;
  objectProp?: { number: CodegenTypes.Double, string: string };

  // Events
  onSomething?: CodegenTypes.DirectEventHandler<SomethingEvent>,
  onTestObjectEvent?: CodegenTypes.DirectEventHandler<{ target: CodegenTypes.Int32; testObject: CodegenTypes.UnsafeMixed }>;
  onEventWithInlineTypes?: CodegenTypes.DirectEventHandler<{ target: CodegenTypes.Int32; contentInset: { top: CodegenTypes.Double; bottom: CodegenTypes.Double; left: CodegenTypes.Double; right: CodegenTypes.Double }; contentOffset: { x: CodegenTypes.Double; y: CodegenTypes.Double }; contentSize: { width: CodegenTypes.Double; height: CodegenTypes.Double }; layoutMeasurement: { width: CodegenTypes.Double; height: CodegenTypes.Double }; velocity: { x: CodegenTypes.Double; y: CodegenTypes.Double }; isUserTriggered: boolean }>;
  onEventWithMultipleAliasTypes?: CodegenTypes.DirectEventHandler<{ target: CodegenTypes.Int32; contentInset: { top: CodegenTypes.Double; bottom: CodegenTypes.Double; left: CodegenTypes.Double; right: CodegenTypes.Double }; contentOffset: { x: CodegenTypes.Double; y: CodegenTypes.Double }; contentSize: { width: CodegenTypes.Double; height: CodegenTypes.Double }; layoutMeasurement: { width: CodegenTypes.Double; height: CodegenTypes.Double }; velocity: { x: CodegenTypes.Double; y: CodegenTypes.Double }; isUserTriggered: boolean }>;
}


type ComponentType = HostComponent<MovingLightProps>;

interface NativeCommands {
  setLightOn: (viewRef: React.ElementRef<ComponentType>, value: boolean) => void;
}

export const Commands: NativeCommands = codegenNativeCommands<NativeCommands>({
  supportedCommands: ['setLightOn'],
});

export default codegenNativeComponent<MovingLightProps>('MovingLight');
