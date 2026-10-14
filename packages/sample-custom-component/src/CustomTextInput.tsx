import React from 'react';
import CustomTextInputSpec, { CustomTextInputProps} from './CustomTextInputSpec';

const CustomTextInput = React.forwardRef<any, CustomTextInputProps>((props: CustomTextInputProps, ref) => {
  return <CustomTextInputSpec ref={ref} {...props} focusable={props.focusable !== false}  />;
});

export default CustomTextInput;