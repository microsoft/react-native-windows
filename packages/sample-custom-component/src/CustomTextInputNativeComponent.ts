// This file is used for codegen of the CustomTextInput native component
// It is not included in the bundle, since codegen only works on View components
// Changes to this file should be reflected in CustomTextInputSpec.ts which 
// provides the view spec and actual component implementation

import { codegenNativeComponent, codegenNativeCommands, CodegenTypes } from 'react-native';
import type { HostComponent, ViewProps } from 'react-native';

export type SomethingEvent = {
  value: string,
  target: CodegenTypes.Int32,
};

export type IDelta = string;

export interface CustomTextInputProps extends ViewProps {
  // Props
  test?: CodegenTypes.WithDefault<boolean, false>;

  // Events
  onSomething?: CodegenTypes.DirectEventHandler<SomethingEvent>,
}

type ComponentType = HostComponent<CustomTextInputProps>;

interface NativeCommands {
  setLightOn: (viewRef: React.ElementRef<ComponentType>, value: boolean) => void;
  setFormattedText: (
    viewRef: React.ElementRef<ComponentType>,
    json: IDelta,
  ) => void;
}

export const Commands: NativeCommands = codegenNativeCommands<NativeCommands>({
  supportedCommands: ['setLightOn', 'setFormattedText'],
});

export default codegenNativeComponent<CustomTextInputProps>('CustomTextInput');

