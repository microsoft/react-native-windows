'use strict';

const React = require('react-native');
const styles={
   views:{
       flexDirection: 'column'
   },
   images: {
       height:200,
       width:250,
       borderWidth: 5, 
       borderColor: '#f099f0',
       borderRadius: 10
   }
};

var {AppRegistry, View, Text, TextInput, Image } = React;

    var ReactRoot = React.createClass({
		
	getInitialState: function(){
		var text = "You can see me!";
		return {value: text};
	},

    render: function() {
        let imageURL = "http://facebook.github.io/react-native/img/opengraph.png?2";
        
        return (
            <View>
              <View style={styles.views}>
		         <Text>Hello!</Text>
				{(this.state && this.state.value) 
			     ? <TextInput value={this.state.value}></TextInput> 
				 : undefined}
              </View>
              <View style={styles.views}>
                 <Image source={{uri: 'http://facebook.github.io/origami/public/images/blog-hero.jpg?r=1'}} style={styles.images}/>
              </View>
            </View>
        );
    }
 });

AppRegistry.registerComponent('ReactRoot', () => ReactRoot);
