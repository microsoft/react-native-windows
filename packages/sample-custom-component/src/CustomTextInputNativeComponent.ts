// This file is used for codegen of the CustomTextInput native component
// It is not included in the bundle, since codegen only works on View components
// Changes to this file should be reflected in CustomTextInputSpec.ts which 
// provides the view spec and actual component implementation

import { codegenNativeComponent, codegenNativeCommands } from 'react-native';
import type { HostComponent, ViewProps } from 'react-native';

import type {
  DirectEventHandler,
  Int32,
  WithDefault,
} from 'react-native/Libraries/Types/CodegenTypes';

export type SomethingEvent = {
  value: string,
  target: Int32,
};

export interface CustomTextInputProps extends ViewProps {
  // Props
  test?: WithDefault<boolean, false>;

  // Events
  onSomething?: DirectEventHandler<SomethingEvent>,
}


type ComponentType = HostComponent<CustomTextInputProps>;

interface NativeCommands {
  setLightOn: (viewRef: React.ElementRef<ComponentType>, value: boolean) => void;
}

export const Commands: NativeCommands = codegenNativeCommands<NativeCommands>({
  supportedCommands: ['setLightOn'],
});

export default codegenNativeComponent<CustomTextInputProps>('CustomTextInput');

