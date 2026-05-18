import { codegenNativeComponent } from 'react-native';
import type { ViewProps } from 'react-native';

export interface CustomAccessibilityProps extends ViewProps {
}

export default codegenNativeComponent<CustomAccessibilityProps>('CustomAccessibility');
