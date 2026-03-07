import type { ColorValue, ViewProps } from 'react-native';
import * as React from 'react';
export interface MovingLightHandle {
    setLightOn(value: boolean): void;
}
interface MovingLightProps extends ViewProps {
    size?: number;
    color?: ColorValue;
    eventParam?: string;
    objectProp?: {
        number: number;
        string: string;
    };
    onSomething?: (value: string) => Promise<void> | void;
}
declare const MovingLightWithForwardedRef: React.ForwardRefExoticComponent<MovingLightProps & React.RefAttributes<MovingLightHandle>>;
export default MovingLightWithForwardedRef;
