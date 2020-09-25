/**
 * @flow
 * @format
 */

import type {HostComponent} from '../../Renderer/shims/ReactNativeTypes';

import codegenNativeCommands from '../../Utilities/codegenNativeCommands';
import type {TextInputNativeCommands} from './TextInputNativeCommands';

type NativeType = HostComponent<mixed>;

type NativeCommands = TextInputNativeCommands<NativeType>;

export const Commands: NativeCommands = codegenNativeCommands<NativeCommands>({
  supportedCommands: ['focus', 'blur', 'setTextAndSelection'],
}); // [Windows]
