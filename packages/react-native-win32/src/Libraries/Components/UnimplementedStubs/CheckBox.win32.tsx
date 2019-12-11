/**
 * @providesModule CheckBox
 */

'use strict';
import UnimplementedView from './UnimplementedViews';
import { ViewProps } from 'react-native';

// ViewProperties should be CheckBoxProperties, but it doesn't exist in the typings yet
class CheckBox extends UnimplementedView<ViewProps> {}

export = CheckBox;
