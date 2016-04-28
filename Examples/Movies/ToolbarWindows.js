/**
 * The examples provided by Facebook are for non-commercial testing and
 * evaluation purposes only.
 *
 * Facebook reserves all rights not expressly granted.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON INFRINGEMENT. IN NO EVENT SHALL
 * FACEBOOK BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
 * AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * @providesModule ToolbarWindows
 * @flow
 */
'use strict';

const React = require('react');
const ReactNative = require('react-native');
const {
  StyleSheet,
  Text,
  Image,
  TouchableHighlight,
  View,
} = ReactNative;

class ToolbarWindows extends React.Component {
  constuctor(props: {
    title: ?string,
    onPress: Function,
    style: ?any
  }) {
      
  }
  render(): ?ReactElement {
    return (
      <View style={[styles.header, this.props.style]}>
        <TouchableHighlight
          onPress={this.props.onPress}
          style={[styles.button]}>
          <Image
                style={styles.button}
                source={require('./android_back_white.png')}
          />
        </TouchableHighlight>
        <View style={[styles.titleContainer]}>
          <Text style={[styles.title]}>{this.props.title}</Text>
        </View>
      </View>
    );
  }
}

var styles = StyleSheet.create({
  header: {
    flexDirection: 'row',
  },
  button: {
    height: 25,
    width: 25,
    padding: 8,
  },
  titleContainer: {
    alignSelf: 'flex-start',
    marginTop: 8,
    paddingLeft:20,
    flex: 1,
    padding: 2,
  },
  title: {
    fontSize: 18,
    color: '#ffffff',
    paddingHorizontal: 10,
    textAlign: 'left',
    fontWeight: '400',
  },
});

module.exports = ToolbarWindows;
