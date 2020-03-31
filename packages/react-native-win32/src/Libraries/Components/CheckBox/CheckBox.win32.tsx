/**
 * @providesModule CheckBox
 */

'use strict';
// @ts-ignore This file is present after the JS build
import * as UnimplementedView from '../UnimplementedViews/UnimplementedView';
import { ViewProps } from 'react-native';

// ViewProperties should be CheckBoxProperties, but it doesn't exist in the typings yet
class CheckBox extends UnimplementedView<ViewProps> {}

export = CheckBox;
