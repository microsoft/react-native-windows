import type { ViewProps } from 'react-native';
import DrawingIslandNativeComponent from './DrawingIslandNativeComponent';
import * as React from 'react';

export interface DrawingIslandHandle {
};

interface DrawingIslandProps extends ViewProps {
}
  
function DrawingIsland(
  props: DrawingIslandProps,
  ref: React.Ref<DrawingIslandHandle>,
) {
  
  React.useImperativeHandle(
    ref,
    () => ({}),
    [],
  );

  const nativeComponentRef = React.useRef<React.ElementRef<
    typeof DrawingIslandNativeComponent
  > | null>(null);

  return (
        <DrawingIslandNativeComponent
          ref={nativeComponentRef}
          {...props}
        />
  );
}
  
const DrawingIslandWithForwardedRef = React.forwardRef(DrawingIsland);

export default DrawingIslandWithForwardedRef;
