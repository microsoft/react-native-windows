/**
 * @flow
 * @format
 */

import type {HostComponent} from '../../Renderer/shims/ReactNativeTypes';

const requireNativeComponent = require('../../ReactNative/requireNativeComponent');
import codegenNativeCommands from '../../Utilities/codegenNativeCommands';
import type {TextInputNativeCommands} from './TextInputNativeCommands';
type NativeType = HostComponent<mixed>;

type NativeCommands = TextInputNativeCommands<NativeType>;

export const Commands: NativeCommands = codegenNativeCommands<NativeCommands>({
  supportedCommands: ['focus', 'blur', 'setTextAndSelection'],
});

const WindowsTextInputComponent: NativeType =
  requireNativeComponent<mixed>('WindowsTextInput');

export default WindowsTextInputComponent;
// [Windows]
