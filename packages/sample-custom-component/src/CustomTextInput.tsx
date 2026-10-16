import React from 'react';
import CustomTextInputSpec, { CustomTextInputProps} from './CustomTextInputSpec';
import {unstable_TextAncestorContext} from 'react-native';


const CustomTextInput = React.forwardRef<any, CustomTextInputProps>((props: CustomTextInputProps, ref) => {
  const { children, ...rest } = props;
  return (
    <CustomTextInputSpec ref={ref} {...rest} focusable={props.focusable !== false}>
      <unstable_TextAncestorContext.Provider value={true}>
        {children}
      </unstable_TextAncestorContext.Provider>
    </CustomTextInputSpec>
  );
});

export default CustomTextInput;