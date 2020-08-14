import React from 'react';
import DeprecatedTextPropTypes from '../DeprecatedPropTypes/DeprecatedTextPropTypes';
import StyleSheet from '../../StyleSheet/StyleSheet';
import Text from '../../Text/Text.js';
import TextAncestorBordered from './TextAncestorBordered.windows.js';
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

  // Due to XAML limitations, wrapping  Text with a View in order to display borders.
  // Like other platforms, ignoring borders for nested Text (using the Context API to detect nesting).
  return (
    <TextAncestorBordered.Consumer>
      {
        (hasTextAncestor) => hasTextAncestor ?
          (<Text {...props} ref={props.forwardedRef} />) :

          (props.style && props.style.borderWidth && props.style.borderColor) ?
            (<TextAncestorBordered.Provider value={true}>
              <View style={props.style}>
                <Text style={rest} {...textPropsLessStyle} ref={props.forwardedRef} />
              </View>
            </TextAncestorBordered.Provider>) :

            (<TextAncestorBordered.Provider value={true}>
              <Text {...props} ref={props.forwardedRef} />
            </TextAncestorBordered.Provider>)
      }
    </TextAncestorBordered.Consumer>);
}

BorderedText.propTypes = DeprecatedTextPropTypes;
module.exports = BorderedText;
