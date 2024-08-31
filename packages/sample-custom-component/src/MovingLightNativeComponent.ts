import codegenNativeComponent from 'react-native/Libraries/Utilities/codegenNativeComponent';
import type { ColorValue, ViewProps } from 'react-native';

import type {
  DirectEventHandler,
  Float,
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

  // Events
  onSomething?: DirectEventHandler<SomethingEvent>,
}

export default codegenNativeComponent<MovingLightProps>('MovingLight');
