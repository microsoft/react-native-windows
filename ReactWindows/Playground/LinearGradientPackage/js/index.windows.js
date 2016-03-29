/**
 * @providesModule LinearGradient
 * @flow
 */

var React = require('react-native');
var flattenStyle = require('flattenStyle');
var { requireNativeComponent, processColor, PropTypes, View } = React;

var LinearGradient = React.createClass({
  propTypes: {
    start: PropTypes.arrayOf(PropTypes.number),
    end: PropTypes.arrayOf(PropTypes.number),
    colors: PropTypes.arrayOf(PropTypes.string).isRequired,
    locations: PropTypes.arrayOf(PropTypes.number),
    ...View.propTypes,
  },

  render: function() {
    var { style, children, colors, locations, start, end, ...otherProps } = this.props;

    // inherit container borderRadius until this issue is resolved:
    // https://github.com/facebook/react-native/issues/3198
    var borderRadius = style && flattenStyle(style).borderRadius || 0;

    return (
      <View {...otherProps} style={style}>
        <NativeLinearGradient
          style={{position: 'absolute', top: 0, left: 0, bottom: 0, right: 0}}
          colors={colors.map(processColor)}
          start={start}
          end={end}
          locations={locations}
          borderRadius={borderRadius}
        />
        { children }
      </View>
    );
  }
})

 var NativeLinearGradient = requireNativeComponent('BVLinearGradient', null);

 module.exports = LinearGradient;
