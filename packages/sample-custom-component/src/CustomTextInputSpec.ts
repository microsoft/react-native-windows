// Manual spec implementation for CustomTextInput component
// Since it needs to include all the textinput related props and events


// @ts-ignore
import {TextInputViewSpec, TextInputProps, HostComponent} from 'react-native';

// @ts-ignore
const NativeComponentRegistry = require('react-native/Libraries/NativeComponent/NativeComponentRegistry');
const dispatchCommand = require("react-native/Libraries/ReactNative/RendererProxy").dispatchCommand;

export type SomethingEvent = {
  value: string,
  target: number,
};

const nativeComponentName = 'CustomTextInput';
const spec = 
{
    ...TextInputViewSpec,
    uiViewClassName: nativeComponentName,
    directEventTypes: {
        ...TextInputViewSpec.directEventTypes,

        topSomething: { registrationName: "onSomething" },
    },
    validAttributes: Object.assign({...TextInputViewSpec.validAttributes, test: true, onSomething: true })
};

export interface CustomTextInputProps extends TextInputProps {
      test?: boolean;
      onSomething?: (event: SomethingEvent) => void;
}

export default NativeComponentRegistry.get(nativeComponentName, () => {
    return spec;
}) as HostComponent<CustomTextInputProps>;

export const Commands = {
    setLightOn: function setLightOn(ref : any, value: boolean) { dispatchCommand(ref, "setLightOn", [value]); }
};