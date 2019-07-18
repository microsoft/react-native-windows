/**
 * Copyright (c) Microsoft Corporation and contributors. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */
'use strict';

var React = require('react');
var ReactNative = require('react-native');
var ReactWindows = require('react-native-windows');
var {
  Image,
  StyleSheet,
  Text,
  TouchableWithoutFeedback,
  TouchableOpacity,
  View,
} = ReactNative;
var {FlipViewWindows} = ReactWindows;

var PAGES = 5;
var BGCOLOR = ['#fdc08e', '#fff6b9', '#99d1b7', '#dde5fe', '#f79273'];
var IMAGE_URIS = [
  'http://apod.nasa.gov/apod/image/1410/20141008tleBaldridge001h990.jpg',
  'http://apod.nasa.gov/apod/image/1409/volcanicpillar_vetter_960.jpg',
  'http://apod.nasa.gov/apod/image/1409/m27_snyder_960.jpg',
  'http://apod.nasa.gov/apod/image/1409/PupAmulti_rot0.jpg',
  'http://apod.nasa.gov/apod/image/1510/lunareclipse_27Sep_beletskycrop4.jpg',
];

class LikeCount extends React.Component {
  state = {
    likes: 7,
  };

  onClick = () => {
    this.setState({likes: this.state.likes + 1});
  };

  render() {
    var thumbsUp = '\uD83D\uDC4D';
    return (
      <View style={styles.likeContainer}>
        <TouchableOpacity onPress={this.onClick} style={styles.likeButton}>
          <Text style={styles.likesText}>
            {thumbsUp + ' Like'}
          </Text>
        </TouchableOpacity>
        <Text style={styles.likesText}>
          {this.state.likes + ' likes'}
        </Text>
      </View>
    );
  }
}

class Button extends React.Component {
  _handlePress = () => {
    if (this.props.enabled && this.props.onPress) {
      this.props.onPress();
    }
  };

  render() {
    return (
      <TouchableWithoutFeedback onPress={this._handlePress}>
        <View style={[styles.button, this.props.enabled ? {} : styles.buttonDisabled]}>
          <Text style={styles.buttonText}>{this.props.text}</Text>
        </View>
      </TouchableWithoutFeedback>
    );
  }
}

class ProgressBar extends React.Component {
  render() {
    var fractionalPosition = (this.props.progress.position + this.props.progress.offset);
    var progressBarSize = (fractionalPosition / (PAGES - 1)) * this.props.size;
    return (
      <View style={[styles.progressBarContainer, {width: this.props.size}]}>
        <View style={[styles.progressBar, {width: progressBarSize}]}/>
      </View>
    );
  }
}

class FlipViewWindowsExample extends React.Component {
  
  state = {
    page: 0,
    animationsAreEnabled: true,
    scrollEnabled: true,
    progress: {
      position: 0,
      offset: 0,
    },
  };

  onSelectionChange = (e) => {
    this.setState({page: e.nativeEvent.position});
  };

  move = (delta) => {
    var page = this.state.page + delta;
    this.go(page);
  };

  go = (page) => {
    this.flipView.setPage(page);
    this.setState({page});
  };

  render() {
    var pages = [];
    for (var i = 0; i < PAGES; i++) {
      var pageStyle = {
        backgroundColor: BGCOLOR[i % BGCOLOR.length],
        alignItems: 'center',
        padding: 20,
      };
      pages.push(
        <View key={i} style={pageStyle} collapsable={false}>
          <Image
            style={styles.image}
            source={{uri: IMAGE_URIS[i % BGCOLOR.length]}}
          />
          <LikeCount />
       </View>
      );
    }
    var { page, animationsAreEnabled } = this.state;
    return (
      <View style={styles.container}>
        <FlipViewWindows
          style={styles.flipView}
          alwaysAnimate={this.state.animationsAreEnabled}
          initialPage={0}
          onSelectionChange={this.onSelectionChange}
          ref={flipView => { this.flipView = flipView; }}>
          {pages}
        </FlipViewWindows>
        <View style={styles.buttons}>
          { animationsAreEnabled ?
            <Button
              text="Turn off animations"
              enabled={true}
              onPress={() => this.setState({animationsAreEnabled: false})}
            /> :
            <Button
              text="Turn animations back on"
              enabled={true}
              onPress={() => this.setState({animationsAreEnabled: true})}
            /> }
        </View>
        <View style={styles.buttons}>
          <Button text="Start" enabled={page > 0} onPress={() => this.go(0)}/>
          <Button text="Prev" enabled={page > 0} onPress={() => this.move(-1)}/>
          <Text style={styles.buttonText}>Page {page + 1} / {PAGES}</Text>
          <ProgressBar size={100} progress={this.state.progress}/>
          <Button text="Next" enabled={page < PAGES - 1} onPress={() => this.move(1)}/>
          <Button text="Last" enabled={page < PAGES - 1} onPress={() => this.go(PAGES - 1)}/>
        </View>
      </View>
    );
  }
}

var styles = StyleSheet.create({
  buttons: {
    flexDirection: 'row',
    height: 30,
    backgroundColor: 'black',
    alignItems: 'center',
    justifyContent: 'space-between',
  },
  button: {
    flex: 1,
    width: 0,
    margin: 5,
    borderColor: 'gray',
    borderWidth: 1,
    backgroundColor: 'gray',
  },
  buttonDisabled: {
    backgroundColor: 'black',
    opacity: 0.5,
  },
  buttonText: {
    color: 'white',
  },
  container: {
    flex: 1,
    backgroundColor: 'white',
  },
  image: {
    width: 300,
    height: 200,
    padding: 20,
  },
  likeButton: {
    backgroundColor: 'rgba(0, 0, 0, 0.1)',
    borderColor: '#333333',
    borderWidth: 1,
    borderRadius: 5,
    flex: 1,
    margin: 8,
    padding: 8,
  },
  likeContainer: {
    flexDirection: 'row',
  },
  likesText: {
    flex: 1,
    fontSize: 18,
    alignSelf: 'center',
  },
  progressBarContainer: {
    height: 10,
    margin: 10,
    borderColor: '#eeeeee',
    borderWidth: 2,
  },
  progressBar: {
    alignSelf: 'flex-start',
    flex: 1,
    backgroundColor: '#eeeeee',
  },
  flipView: {
    flex: 1,
  },
});

exports.title = '<FlipViewWindows>';
exports.description =
  'Container that allows to flip left and right between child views.';

exports.examples = [
  {
    title: 'Basic FlipView',
    render(): React.Element<typeof FlipViewWindowsExample> {
      return <FlipViewWindowsExample />;
    },
  },
];