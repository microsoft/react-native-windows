import {codegenNativeComponent} from 'react-native';
import type { ViewProps } from 'react-native';

export interface DrawingIslandProps extends ViewProps {
}

export default codegenNativeComponent<DrawingIslandProps>('DrawingIsland');
