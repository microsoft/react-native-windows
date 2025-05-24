/**
 * @flow
 * @format
 */

import type {HostComponent} from '../../../src/private/types/HostComponent';

import requireNativeComponent from '../../ReactNative/requireNativeComponent';
import codegenNativeCommands from '../../Utilities/codegenNativeCommands';
import type {TextInputNativeCommands} from './TextInputNativeCommands';

// $FlowFixMe[incompatible-type-arg]
type NativeType = HostComponent<mixed>;

type NativeCommands = TextInputNativeCommands<NativeType>;

export const Commands: NativeCommands = codegenNativeCommands<NativeCommands>({
  supportedCommands: ['focus', 'blur', 'setTextAndSelection'],
});

const WindowsTextInputComponent: NativeType =
  // $FlowFixMe[incompatible-call]
  requireNativeComponent<mixed>('RCTTextInput');

export default WindowsTextInputComponent;
// [Windows]
