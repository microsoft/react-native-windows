import type { ColorValue, NativeSyntheticEvent, ViewProps } from 'react-native';
import MovingLightNativeComponent, {Commands, SomethingEvent} from './MovingLightNativeComponent';
import * as React from 'react';

export interface MovingLightHandle {
  setLightOn(value: boolean): void,
};

interface MovingLightProps extends ViewProps {
  // Props
  size?: number;
  color?: ColorValue
  eventParam?: string;
  objectProp?: { number: number, string: string};

  // Events
  onSomething?: (value: string) => Promise<void> | void,
}
  
function MovingLight(
  props: MovingLightProps,
  ref: React.Ref<MovingLightHandle>,
) {
  const {onSomething, ...restProps} = props;

  const handleSomething = (event: NativeSyntheticEvent<SomethingEvent>) => {
    void onSomething?.(event.nativeEvent.value);
  };

  React.useImperativeHandle(
    ref,
    () => ({
      setLightOn(value: boolean) {
          if (nativeComponentRef.current != null) {
              Commands.setLightOn(
                nativeComponentRef.current,
                value,
              );
            }
      }
    }),
    [],
  );

  const nativeComponentRef = React.useRef<React.ElementRef<
    typeof MovingLightNativeComponent
  > | null>(null);

  return (
        <MovingLightNativeComponent
          ref={nativeComponentRef}
          onSomething={handleSomething}
          {...restProps}
        />
  );
}
  
const MovingLightWithForwardedRef = React.forwardRef(MovingLight);

export default MovingLightWithForwardedRef;
