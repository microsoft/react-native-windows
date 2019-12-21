/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 *
 * @flow strict-local
 * @format
 */

'use strict';

const Dimensions = require('../../Utilities/Dimensions');
const React = require('react');
const FlatList = require('../../Lists/FlatList');
const SafeAreaView = require('../../Components/SafeAreaView/SafeAreaView');
const StyleSheet = require('../../StyleSheet/StyleSheet');
const View = require('../../Components/View/View');
const YellowBoxButton = require('./YellowBoxButton');
const YellowBoxInspector = require('./YellowBoxInspector');
const YellowBoxListRow = require('./YellowBoxListRow');
const YellowBoxStyle = require('./YellowBoxStyle');
const Platform = require('../../Utilities/Platform'); // TODO(windows ISS)

import type {Category} from '../Data/YellowBoxCategory';
import type {Registry} from '../Data/YellowBoxRegistry';

type Props = $ReadOnly<{|
  onDismiss: (category: Category) => void,
  onDismissAll: () => void,
  registry: Registry,
|}>;

type State = {|
  selectedCategory: ?Category,
|};

const VIEWPORT_RATIO = 0.5;
const MAX_ITEMS =
  Platform.OS === 'win32' // [Win32 - Avoid calls to Dimensions since we need multi window support
    ? 3
    : Math.floor(
        (Dimensions.get('window').height * VIEWPORT_RATIO) /
          (YellowBoxListRow.GUTTER + YellowBoxListRow.HEIGHT),
      ); // ]Win32

class YellowBoxList extends React.Component<Props, State> {
  state = {
    selectedCategory: null,
  };

  render(): React.Node {
    const selectedWarnings =
      this.state.selectedCategory == null
        ? null
        : this.props.registry.get(this.state.selectedCategory);

    if (selectedWarnings != null) {
      return (
        <View style={StyleSheet.absoluteFill}>
          <YellowBoxInspector
            onDismiss={this._handleInspectorDismiss}
            onMinimize={this._handleInspectorMinimize}
            warnings={selectedWarnings}
          />
        </View>
      );
    }

    const items = [];
    for (const [category, warnings] of this.props.registry) {
      items.unshift({category, warnings});
    }

    const listStyle = {
      width: Platform.OS === 'win32' ? '85%' : undefined, // [Win32 - Percentage not currently supported on win32]
      height:
        // Additional `0.5` so the (N + 1)th row can peek into view.
        Math.min(items.length, MAX_ITEMS + 0.5) *
        (YellowBoxListRow.GUTTER + YellowBoxListRow.HEIGHT),
    };

    return items.length === 0 ? null : (
      <View style={styles.list}>
        <View pointerEvents="box-none" style={styles.dismissAll}>
          <YellowBoxButton
            hitSlop={{bottom: 4, left: 4, right: 4, top: 4}}
            label="Dismiss All"
            onPress={this.props.onDismissAll}
          />
        </View>
        <FlatList
          data={items}
          keyExtractor={item => item.category}
          renderItem={({item}) => (
            <YellowBoxListRow {...item} onPress={this._handleRowPress} />
          )}
          scrollEnabled={items.length > MAX_ITEMS}
          scrollsToTop={false}
          style={listStyle}
        />
        <SafeAreaView style={styles.safeArea} />
      </View>
    );
  }

  _handleInspectorDismiss = () => {
    const category = this.state.selectedCategory;
    if (category == null) {
      return;
    }
    this.setState({selectedCategory: null}, () => {
      this.props.onDismiss(category);
    });
  };

  _handleInspectorMinimize = () => {
    this.setState({selectedCategory: null});
  };

  _handleRowPress = (category: Category) => {
    this.setState({selectedCategory: category});
  };
}

const styles = StyleSheet.create({
  list: {
    bottom: 0,
    position: 'absolute',
    width: '100%',
  },
  dismissAll: {
    bottom: Platform.OS === 'win32' ? 0 : '100%', // [Win32 - Percentage not currently supported on win32]
    flexDirection: 'row',
    justifyContent: 'flex-end',
    paddingBottom: 4,
    paddingEnd: 4,
    position: 'absolute',
    width: '100%',
  },
  safeArea: {
    backgroundColor: YellowBoxStyle.getBackgroundColor(0.95),
    marginTop: StyleSheet.hairlineWidth,
  },
});

module.exports = YellowBoxList;
