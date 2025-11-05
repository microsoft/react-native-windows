import {codegenNativeComponent, codegenNativeCommands} from 'react-native';
import type { ColorValue, HostComponent, ViewProps } from 'react-native';

import type {
  DirectEventHandler,
  Float,
  Double,
  Int32,
  WithDefault,
} from 'react-native/Libraries/Types/CodegenTypes';

export type SomethingEvent = {
  value: string,
  target: Int32,
};

export interface MovingLightProps extends ViewProps {
  // Props
  size?: WithDefault<Float, 42>;
  color?: ColorValue
  eventParam?: string;
  objectProp?: { number: Double, string: string};

  // Events
  onSomething?: DirectEventHandler<SomethingEvent>,
}


type ComponentType = HostComponent<MovingLightProps>;

interface NativeCommands {
  setLightOn: (viewRef: React.ElementRef<ComponentType>, value: boolean) => void;
}

export const Commands: NativeCommands = codegenNativeCommands<NativeCommands>({
  supportedCommands: ['setLightOn'],
});

export default codegenNativeComponent<MovingLightProps>('MovingLight');
