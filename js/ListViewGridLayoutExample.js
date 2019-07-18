/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @format
 * @flow strict-local
 */

'use strict';

const React = require('react');
const ReactNative = require('react-native');
const {
  Dimensions,
  Image,
  ListView,
  TouchableHighlight,
  StyleSheet,
  Text,
  View,
} = ReactNative;
const ListViewDataSource = require('ListViewDataSource');

import type {RNTesterProps} from 'RNTesterTypes';

const THUMB_URLS = [
  require('./Thumbnails/like.png'),
  require('./Thumbnails/dislike.png'),
  require('./Thumbnails/call.png'),
  require('./Thumbnails/fist.png'),
  require('./Thumbnails/bandaged.png'),
  require('./Thumbnails/flowers.png'),
  require('./Thumbnails/heart.png'),
  require('./Thumbnails/liking.png'),
  require('./Thumbnails/party.png'),
  require('./Thumbnails/poke.png'),
  require('./Thumbnails/superlike.png'),
  require('./Thumbnails/victory.png'),
];

const ITEM_WIDTH = 100;
const ITEM_HEIGHT = 100;
const ITEM_MARGIN = 3;
const ITEM_TOTAL_WIDTH = ITEM_WIDTH + ITEM_MARGIN * 2;
const ITEM_TOTAL_HEIGHT = ITEM_HEIGHT + ITEM_MARGIN * 2;

type State = {|
  dataSource: ListViewDataSource,
|};

class ListViewGridLayoutExample extends React.Component<RNTesterProps, State> {
  state = {
    dataSource: this.getInitialDataSource(),
    windowWidth: Dimensions.get("window").width,
    windowHeight: Dimensions.get("window").height,
  };

  getInitialDataSource() {
    const ds = new ListView.DataSource({rowHasChanged: (r1, r2) => r1 !== r2});
    return ds.cloneWithRows(this._genRows({}));
  }

  _pressData: {[key: number]: boolean} = {};

  UNSAFE_componentWillMount() {
    this._pressData = {};
    Dimensions.addEventListener("change", dims => {
      this.setState({
        windowWidth: dims.window.width,
        windowHeight: dims.window.height
      });
    });
  }

  render() {
    const pageSize = Math.floor(this.state.windowWidth / ITEM_TOTAL_WIDTH);
    const initialListSize = Math.ceil(this.state.windowHeight / ITEM_TOTAL_HEIGHT) * pageSize;

    return (
      // ListView wraps ScrollView and so takes on its properties.
      // With that in mind you can use the ScrollView's contentContainerStyle prop to style the items.
      <ListView
        contentContainerStyle={styles.list}
        dataSource={this.state.dataSource}
        initialListSize={initialListSize}
        pageSize={pageSize} // should be a multiple of the no. of visible cells per row
        scrollRenderAheadDistance={500}
        renderRow={this._renderRow}
      />
    );
  }

  _renderRow = (rowData: string, sectionID: number, rowID: number) => {
    const rowHash = Math.abs(hashCode(rowData));
    const imgSource = THUMB_URLS[rowHash % THUMB_URLS.length];
    return (
      <TouchableHighlight
        onPress={() => this._pressRow(rowID)}
        underlayColor="transparent">
        <View>
          <View style={styles.row}>
            <Image style={styles.thumb} source={imgSource} />
            <Text style={styles.text}>{rowData}</Text>
          </View>
        </View>
      </TouchableHighlight>
    );
  };

  _genRows(pressData: {[key: number]: boolean}): Array<string> {
    const dataBlob = [];
    for (let ii = 0; ii < 100; ii++) {
      const pressedText = pressData[ii] ? ' (X)' : '';
      dataBlob.push('Cell ' + ii + pressedText);
    }
    return dataBlob;
  }

  _pressRow = (rowID: number) => {
    this._pressData[rowID] = !this._pressData[rowID];
    this.setState({
      dataSource: this.state.dataSource.cloneWithRows(
        this._genRows(this._pressData),
      ),
    });
  };
}

/* eslint no-bitwise: 0 */
const hashCode = function(str) {
  let hash = 15;
  for (let ii = str.length - 1; ii >= 0; ii--) {
    hash = (hash << 5) - hash + str.charCodeAt(ii);
  }
  return hash;
};

const styles = StyleSheet.create({
  list: {
    justifyContent: 'space-around',
    flexDirection: 'row',
    flexWrap: 'wrap',
    alignItems: 'flex-start',
  },
  row: {
    justifyContent: 'center',
    padding: 5,
    margin: ITEM_MARGIN,
    width: ITEM_WIDTH,
    height: ITEM_HEIGHT,
    backgroundColor: '#F6F6F6',
    alignItems: 'center',
    borderWidth: 1,
    borderRadius: 5,
    borderColor: '#CCC',
  },
  thumb: {
    width: 64,
    height: 64,
  },
  text: {
    flex: 1,
    marginTop: 5,
    fontWeight: 'bold',
  },
});

exports.title = '<ListView> - Grid Layout';
exports.description = 'Flexbox grid layout.';
exports.examples = [
  {
    title: 'Simple list view with grid layout',
    render: function(): React.Element<typeof ListViewGridLayoutExample> {
      return <ListViewGridLayoutExample />;
    },
  },
];
