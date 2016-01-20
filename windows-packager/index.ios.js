'use strict';

var React = require('react-native');
var {AppRegistry, View, Text, TextInput } = React;

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
			     ? <TextInput value={this.state.value}></TextInput> 
				 : undefined}
              </View>
        );
    }
 });

AppRegistry.registerComponent('ReactRoot', () => ReactRoot);
