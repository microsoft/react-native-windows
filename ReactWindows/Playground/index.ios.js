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
        borderRadius: 10,
        tintColor: '#bf2d32'
    },
    textInput: {
        height: 40,
        borderWidth: 5, 
        borderColor: '#015d87'
    },
    longTextInput: {
        height: 80,
        borderWidth: 3,
        borderColor: '#ccc'
    }
};

var {AppRegistry, View, Text, TextInput, Image, LayoutAnimation } = React;
var animations = {
    layout: {
        spring: {
            duration: 750,
            create: {
                duration: 500,
                type: LayoutAnimation.Types.easeInEaseOut,
                property: LayoutAnimation.Properties.opacity,
            },
            update: {
                type: LayoutAnimation.Types.spring,
                springDamping: 0.4,
            },
        },
        easeInEaseOut: {
            duration: 750,
            create: {
                type: LayoutAnimation.Types.easeInEaseOut,
                property: LayoutAnimation.Properties.scaleXY,
            },
            update: {
                delay: 400,
                type: LayoutAnimation.Types.easeInEaseOut,
            },
        },
    },
};

var ReactRoot = React.createClass({
    componentDidMount() {
        // Animate creation
        LayoutAnimation.configureNext(animations.layout.spring);
    },
    getInitialState: function(){
        var text = "You can see me!";
        var longText = "This is such a long text that it needs to go into a new lineThis is such a long text that it needs to go into a new lineThis is such a long text that it needs to go into a new lineThis is such a long text that it needs to go into a new lineThis is such a long text that it needs to go into a new line";
        
        return {value: text, 
            longText: longText,
            w: 100, h: 100};
    },

    render: function() {
        let imageURL = "http://facebook.github.io/react-native/img/opengraph.png?2";
        
        return (
            <View>
              <View style={styles.views}>
		         <Text>Hello World!</Text>
{(this.state && this.state.value) 
			     ? <TextInput value={this.state.value} style={styles.textInput}></TextInput> 
				 : undefined}
                
{(this.state && this.state.longText) 
 ? <TextInput value={this.state.longText} multiline={true} style={styles.longTextInput}></TextInput> 
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