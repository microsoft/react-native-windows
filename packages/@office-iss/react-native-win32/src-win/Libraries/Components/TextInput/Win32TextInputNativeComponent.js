/**
 * @flow
 * @format
 */

import type {HostComponent} from '../../../src/private/types/HostComponent';

import requireNativeComponent from '../../ReactNative/requireNativeComponent';
import codegenNativeCommands from '../../Utilities/codegenNativeCommands';
import type {TextInputNativeCommands} from './TextInputNativeCommands';

type NativeType = HostComponent<{...}>;

type NativeCommands = TextInputNativeCommands<NativeType>;

export const Commands: NativeCommands = codegenNativeCommands<NativeCommands>({
  supportedCommands: ['focus', 'blur', 'setTextAndSelection'],
});

const WindowsTextInputComponent: NativeType =
  // $FlowFixMe[incompatible-call]
  requireNativeComponent<{...}>('RCTTextInput');

export default WindowsTextInputComponent;
// [Windows]
