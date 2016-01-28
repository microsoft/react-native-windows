'use strict';

var React = require('react-native');
var {AppRegistry, View, requireNativeComponent, Text, TextInput } = React;
var TextInput = requireNativeComponent('TextInput', null);

    var ReactRoot = React.createClass({
		
	getInitialState: function(){
		var text = "You can see me!";
		return {value: text};
	},

    render: function() {
        return (
              <View>
		         <Text>Hello!</Text>
				{(this.state && this.state.value) 
			     ? <TextInput text={this.state.value}></TextInput> 
				 : undefined}
              </View>
        );
    }
 });

AppRegistry.registerComponent('ReactRoot', () => ReactRoot);
