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
 * @providesModule SearchBar
 * @flow
 */
'use strict';

var React = require('react');
var ReactNative = require('react-native');
var {
  Image,
  Platform,
  TextInput,
  ProgressRingWindows,
  StyleSheet,
  TouchableNativeFeedback,
  View,
} = ReactNative;

var SearchBar = React.createClass({
  render: function() {
    var loadingView;
    if (this.props.isLoading) {
      loadingView = (
        <View style={styles.spinnerView}>
         <ProgressRingWindows style={styles.spinner} />
        </View>
      );
    } else {
      loadingView = <View style={styles.spinnerView} />;
    }
    return (
      <View style={styles.searchBar}>
         <View>
            <Image
              source={require('./android_search_white.png')}
              style={styles.icon}
            />
        </View>
        <TextInput
          ref="input"
          autoCapitalize="none"
          autoCorrect={false}
          onChange={this.props.onSearchChange}
          placeholder="Search a movie..."
          onFocus={this.props.onFocus}
          style={styles.searchBarInput}
        />
        {loadingView}
      </View>
    );
  }
});

var styles = StyleSheet.create({
  searchBar: {
    flexDirection: 'row',
    alignItems: 'center',
    backgroundColor: '#a9a9a9',
    height: 40,
  },
  searchBarInput: {
    flex: 1,
    fontSize: 15,
    marginTop: 20,
    color: '#ffffff',
    height: 50,
    alignSelf: 'center',
    borderWidth: 0,
    padding: 0,
    backgroundColor: '#a9a9a9'
  },
  spinnerView: {
    width: 30,
    height: 30,
  },
  spinner: {
    color: 'lime',
  },
  icon: {
    width: 24,
    height: 24,
    marginHorizontal: 8,
  },
});

module.exports = SearchBar;