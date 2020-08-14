import React from 'react';
import DeprecatedTextPropTypes from '../DeprecatedPropTypes/DeprecatedTextPropTypes';
import StyleSheet from '../../StyleSheet/StyleSheet';
import Text from '../../Text/Text.js';
import View from '../../Components/View/View';

// [Windows]
// Forking Text as a workaround in order to be able to render borders around it.
function BorderedText(props) {
  let { margin, padding, ...rest } = props.style != undefined
    ? (Array.isArray(props.style)
      ? StyleSheet.flatten(props.style)
      : props.style)
    : {};

  let { style, ...textPropsLessStyle } = props;

  if (props.style && props.style.borderWidth &&
    props.style.borderColor) {
    return (
      <>
        <View style={props.style}>
          <Text style={rest} {...textPropsLessStyle} />
        </View>
      </>
    );
  } else {
    return (
      <>
        <Text {...props} />
      </>);
  }
}

BorderedText.propTypes = DeprecatedTextPropTypes;
module.exports = BorderedText;
