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
  TouchableWithoutFeedback,
  View,
} = ReactNative;

class TitleBarWindows extends React.Component {
  render(): ?ReactElement {
    return (
      <View style={[styles.header, this.props.style]}>
        <TouchableWithoutFeedback
          onPress={this.props.onPress}>
          <Image
            style={styles.image}
            source={require('./windows/back_white.png')} />
        </TouchableWithoutFeedback>
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
    alignItems: 'center',
  },
  button: {
    marginHorizontal: 8,
    alignItems: 'center',
  },
  image: {
    marginHorizontal: 8,
    height: 24,
    width: 24,
  },
  titleContainer: {
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

module.exports = TitleBarWindows;