/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */

import * as React from 'react';
import {
  AppRegistry,
  Button,
  StyleSheet,
  Text,
  View,
  StyleProp,
  ViewStyle,
} from 'react-native';

type IState = {
  currentAlignStyle: StyleProp<ViewStyle>;
  currentAbsoluteStyle: StyleProp<ViewStyle>;
  currentWidthStyle: StyleProp<ViewStyle>;
};

export default class FlexboxLayoutPlayground extends React.Component<
  {},
  IState
> {
  constructor(props: {}) {
    super(props);

    this._onClick = this._onClick.bind(this);
    this.state = {
      currentAlignStyle: styles.endAlignStyle,
      currentAbsoluteStyle: styles.absoluteTest1,
      currentWidthStyle: styles.widthTest1,
    };
  }

  _onClick() {
    this.setState(s => {
      let state = {...s};

      if (state.currentAlignStyle === styles.endAlignStyle) {
        state.currentAlignStyle = styles.emptyStyle;
      } else {
        state.currentAlignStyle = styles.endAlignStyle;
      }

      if (state.currentAbsoluteStyle === styles.absoluteTest1) {
        state.currentAbsoluteStyle = styles.absoluteTest2;
      } else {
        state.currentAbsoluteStyle = styles.absoluteTest1;
      }

      if (state.currentWidthStyle === styles.widthTest1) {
        state.currentWidthStyle = styles.widthTest2;
      } else {
        state.currentWidthStyle = styles.widthTest1;
      }

      return state;
    });
  }

  render() {
    return (
      // TODO: Should be ScrollView
      <View style={styles.root}>
        <View style={styles.mainContainer}>
          <View style={styles.section}>
            <Text>Direction: Row</Text>
            <View style={styles.containerRow}>
              <View style={styles.box} />
              <View style={styles.box} />
              <View style={styles.box} />
            </View>
          </View>

          <View style={styles.section}>
            <Text>Direction: Column</Text>
            <View style={styles.containerColumn}>
              <View style={styles.box} />
              <View style={styles.box} />
              <View style={styles.box} />
            </View>
          </View>

          <View style={styles.section}>
            <Text>Justify: FlexStart</Text>
            <View style={styles.containerJustifyFlexStart}>
              <View style={styles.box} />
              <View style={styles.box} />
            </View>
          </View>

          <View style={styles.section}>
            <Text>Justify: FlexEnd</Text>
            <View style={styles.containerJustifyFlexEnd}>
              <View style={styles.box} />
              <View style={styles.box} />
            </View>
          </View>

          <View style={styles.section}>
            <Text>Justify: Center</Text>
            <View style={styles.containerJustifyCenter}>
              <View style={styles.box} />
              <View style={styles.box} />
            </View>
          </View>
        </View>
        <View style={styles.mainContainer}>
          <View style={styles.section}>
            <Text>Justify: SpaceBetween</Text>
            <View style={styles.containerJustifySpaceBetween}>
              <View style={styles.box} />
              <View style={styles.box} />
            </View>
          </View>

          <View style={styles.section}>
            <Text>Justify: SpaceAround</Text>
            <View style={styles.containerJustifySpaceAround}>
              <View style={styles.box} />
              <View style={styles.box} />
            </View>
          </View>

          <View style={styles.section}>
            <Text>FlexWrap: NoWrap</Text>
            <View style={styles.containerFlexWrapNoWrap}>
              <View style={styles.box} />
              <View style={styles.box} />
              <View style={styles.box} />
              <View style={styles.box} />
              <View style={styles.box} />
            </View>
          </View>

          <View style={styles.section}>
            <Text>FlexWrap: Wrap</Text>
            <View style={styles.containerFlexWrapWrap}>
              <View style={styles.box} />
              <View style={styles.box} />
              <View style={styles.box} />
              <View style={styles.box} />
              <View style={styles.box} />
            </View>
          </View>

          <View style={styles.section}>
            <Text>AlignItems: Stretch</Text>
            <View style={styles.containerAlignItemsStretch}>
              <View style={styles.box} />
              <View style={styles.box} />
              <View style={styles.box} />
            </View>
          </View>
        </View>
        <View style={styles.mainContainer}>
          <View style={styles.section}>
            <Text>AlignItems: FlexStart</Text>
            <View style={styles.containerAlignItemsFlexStart}>
              <View style={styles.box} />
              <View style={styles.box} />
              <View style={styles.box} />
            </View>
          </View>

          <View style={styles.section}>
            <Text>AlignItems: FlexEnd</Text>
            <View style={styles.containerAlignItemsFlexEnd}>
              <View style={styles.box} />
              <View style={styles.box} />
              <View style={styles.box} />
            </View>
          </View>

          <View style={styles.section}>
            <Text>AlignItems: Center</Text>
            <View style={styles.containerAlignItemsCenter}>
              <View style={styles.box} />
              <View style={styles.box} />
              <View style={styles.box} />
            </View>
          </View>

          <View style={styles.section}>
            <Text>AlignSelf: FlexStart</Text>
            <View style={styles.containerAlignItemsFlexEnd}>
              <View style={styles.box} />
              <View style={styles.boxAlignSelfStart} />
              <View style={styles.box} />
            </View>
          </View>

          <View style={styles.section}>
            <Text style={styles.todoLabel}>TODO: FlexEnd, Stretch, Center</Text>
          </View>
        </View>
        <View style={styles.mainContainer}>
          <View style={styles.section}>
            <Text>AlignContent: FlexEnd</Text>
            <View style={styles.containerAlignContentFlexEnd}>
              <View style={styles.box} />
              <View style={styles.box} />
              <View style={styles.box} />
              <View style={styles.box} />
              <View style={styles.box} />
            </View>
          </View>

          <View style={styles.section}>
            <Text style={styles.todoLabel}>
              TODO: FlexStart, Center, Stretch, SpaceBetween, SpaceAround
            </Text>
          </View>

          <View style={styles.section}>
            <Text>FlexGrow: Right Box</Text>
            <View style={styles.containerRow}>
              <View style={styles.box} />
              <View style={styles.boxWithGrow} />
            </View>
          </View>

          <View style={styles.section}>
            <Text>FlexGrow: Both Boxes</Text>
            <View style={styles.containerRow}>
              <View style={styles.boxWithGrow} />
              <View style={styles.boxWithGrow} />
            </View>
          </View>

          <View style={styles.section}>
            <Text>FlexGrow: Right Box2</Text>
            <View style={styles.containerRow}>
              <View style={styles.boxWithGrow} />
              <View style={styles.boxWithGrow2} />
            </View>
          </View>
        </View>
        <View style={styles.mainContainer}>
          <View style={styles.section}>
            <Text>FlexShrink: Right Box</Text>
            <View style={styles.containerRow}>
              <View style={styles.wideBox} />
              <View style={styles.wideBoxWithShrink} />
            </View>
          </View>

          <View style={styles.section}>
            <Text>FlexBasis: Auto</Text>
            <View style={styles.containerRow}>
              <View style={styles.boxWithFlexBasisAuto} />
              <View style={styles.boxWithFlexBasisAuto} />
              <View style={styles.boxWithFlexBasisAuto} />
            </View>
          </View>

          <View style={styles.section}>
            <Text>FlexBasis: Point</Text>
            <View style={styles.containerRow}>
              <View style={styles.boxWithFlexBasisPoint} />
              <View style={styles.boxWithFlexBasisPoint} />
              <View style={styles.boxWithFlexBasisPoint} />
            </View>
          </View>

          <View style={styles.section}>
            <Text>FlexBasis: Percent</Text>
            <View style={styles.containerRow}>
              <View style={styles.boxWithFlexBasisPercent} />
              <View style={styles.boxWithFlexBasisPercent} />
              <View style={styles.boxWithFlexBasisPercent} />
            </View>
          </View>

          <View style={styles.section}>
            <Text>Position: Absolute</Text>
            <View style={styles.containerRow}>
              <View style={styles.absoluteBox} />
            </View>
          </View>
        </View>
        <View style={styles.mainContainer}>
          <View style={styles.section}>
            <Text>Align: Stretch With Right Box MaxHeight</Text>
            <View style={styles.containerAlignItemsStretch}>
              <View style={styles.box} />
              <View style={styles.boxWithMaxHeight} />
            </View>
          </View>

          <View style={styles.section}>
            <Text>Margin: Start</Text>
            <View style={styles.containerMarginStart}>
              <View style={styles.boxNoMargin} />
            </View>
          </View>

          <View style={styles.section}>
            <Text>Margin: End</Text>
            <View style={styles.containerMarginEnd}>
              <View style={styles.boxNoMargin} />
            </View>
          </View>

          <View style={styles.section}>
            <Text>Margin Border Padding</Text>
            <View style={styles.containerMarginBorderPadding}>
              <View style={styles.boxNoMargin} />
            </View>
          </View>
        </View>
        <View style={styles.mainContainer}>
          <View>
            <Text style={{flexWrap: 'wrap'}}>
              Native control under React view, make sure margin is not double
              counted. It is being double counted if the button's right and
              bottom edges touch the edge of the rect behind it.
            </Text>

            <View style={{padding: 5}}>
              <Text>Click to switch styles</Text>
              <Button
                title="Click"
                onPress={() => {
                  this._onClick();
                }}
              />
            </View>

            <View style={{backgroundColor: '#EEEEFF', alignItems: 'flex-end'}}>
              <Button title="Text" onPress={this._onClick} />
            </View>

            <View style={{flex: 1, flexDirection: 'column'}}>
              <View style={this.state.currentAlignStyle}>
                <View style={styles.box} />
              </View>

              <View>
                <View style={this.state.currentWidthStyle} />
              </View>

              <View>
                <View style={this.state.currentAbsoluteStyle} />
              </View>
            </View>
          </View>
        </View>
      </View>
    );
  }
}

const styles = StyleSheet.create({
  endAlignStyle: {
    alignItems: 'flex-end',
  },

  absoluteTest1: {
    position: 'absolute',
    left: 30,
    minWidth: 50,
    minHeight: 50,
    backgroundColor: '#660066',
  },

  absoluteTest2: {
    position: 'absolute',
    minWidth: 50,
    minHeight: 50,
    backgroundColor: '#660066',
  },

  widthTest1: {
    width: 20,
    minHeight: 50,
    backgroundColor: '#660000',
  },

  widthTest2: {
    minHeight: 50,
    backgroundColor: '#660000',
  },

  emptyStyle: {},

  nativeStackPanel: {
    borderWidth: 5,
    borderColor: '#000000',
  },

  nativeButton: {
    margin: 2,
  },

  todoLabel: {
    backgroundColor: '#660066',
    flexWrap: 'wrap',
  },

  root: {
    flex: 1,
    flexDirection: 'row',
    flexWrap: 'wrap',
    alignContent: 'flex-start',
  },

  mainContainer: {
    flex: 1,
    flexDirection: 'column',
    width: 200,
    height: 800,
    borderWidth: 1,
    borderColor: '#000000',
    margin: 10,
  },
  section: {
    flex: 1,
    flexDirection: 'column',
  },
  containerRow: {
    flex: 1,
    backgroundColor: '#999999',
    borderWidth: 1,
    borderColor: '#000000',
    flexDirection: 'row',
  },
  containerColumn: {
    flex: 1,
    backgroundColor: '#999999',
    borderWidth: 1,
    borderColor: '#000000',
    flexDirection: 'column',
  },

  containerJustifyFlexStart: {
    flex: 1,
    backgroundColor: '#999999',
    borderWidth: 1,
    borderColor: '#000000',
    flexDirection: 'row',
    justifyContent: 'flex-start',
  },

  containerJustifyFlexEnd: {
    flex: 1,
    backgroundColor: '#999999',
    borderWidth: 1,
    borderColor: '#000000',
    flexDirection: 'row',
    justifyContent: 'flex-end',
  },

  containerJustifyCenter: {
    flex: 1,
    backgroundColor: '#999999',
    borderWidth: 1,
    borderColor: '#000000',
    flexDirection: 'row',
    justifyContent: 'center',
  },

  containerJustifySpaceBetween: {
    flex: 1,
    backgroundColor: '#999999',
    borderWidth: 1,
    borderColor: '#000000',
    flexDirection: 'row',
    justifyContent: 'space-between',
  },

  containerJustifySpaceAround: {
    flex: 1,
    backgroundColor: '#999999',
    borderWidth: 1,
    borderColor: '#000000',
    flexDirection: 'row',
    justifyContent: 'space-around',
  },

  containerFlexWrapNoWrap: {
    flex: 1,
    backgroundColor: '#999999',
    borderWidth: 1,
    borderColor: '#000000',
    flexDirection: 'row',
    flexWrap: 'nowrap',
  },

  containerFlexWrapWrap: {
    flex: 1,
    backgroundColor: '#999999',
    borderWidth: 1,
    borderColor: '#000000',
    flexDirection: 'row',
    flexWrap: 'wrap',
  },

  containerAlignItemsStretch: {
    flex: 1,
    backgroundColor: '#999999',
    borderWidth: 1,
    borderColor: '#000000',
    flexDirection: 'row',
    alignItems: 'stretch',
    height: 100,
  },

  containerAlignItemsFlexStart: {
    flex: 1,
    backgroundColor: '#999999',
    borderWidth: 1,
    borderColor: '#000000',
    flexDirection: 'row',
    alignItems: 'flex-start',
    height: 100,
  },

  containerAlignItemsFlexEnd: {
    flex: 1,
    backgroundColor: '#999999',
    borderWidth: 1,
    borderColor: '#000000',
    flexDirection: 'row',
    alignItems: 'flex-end',
    height: 100,
  },

  containerAlignItemsCenter: {
    flex: 1,
    backgroundColor: '#999999',
    borderWidth: 1,
    borderColor: '#000000',
    flexDirection: 'row',
    alignItems: 'center',
    height: 100,
  },

  containerAlignContentFlexEnd: {
    flex: 1,
    backgroundColor: '#999999',
    borderWidth: 1,
    borderColor: '#000000',
    flexDirection: 'row',
    flexWrap: 'wrap',
    alignContent: 'flex-end',
    height: 200,
  },

  containerMarginStart: {
    flex: 1,
    backgroundColor: '#999999',
    flexDirection: 'row',
    height: 200,
    marginLeft: 50,
  },

  containerMarginEnd: {
    flex: 1,
    backgroundColor: '#999999',
    flexDirection: 'row',
    height: 200,
    marginRight: 50,
  },

  containerMarginBorderPadding: {
    flex: 1,
    backgroundColor: '#999999',
    borderColor: '#000000',
    flexDirection: 'row',
    height: 200,
    margin: 20,
    padding: 20,
    borderWidth: 20,
  },

  box: {
    minWidth: 50,
    minHeight: 50,
    backgroundColor: '#333333',
    margin: 5,
  },

  boxAlignSelfStart: {
    minWidth: 50,
    minHeight: 50,
    backgroundColor: '#333333',
    margin: 5,
    alignSelf: 'flex-start',
  },

  boxWithGrow: {
    minWidth: 50,
    minHeight: 50,
    backgroundColor: '#333333',
    margin: 5,
    flexGrow: 1.0,
  },

  boxWithGrow2: {
    minWidth: 50,
    minHeight: 50,
    backgroundColor: '#333333',
    margin: 5,
    flexGrow: 2.0,
  },

  boxWithFlexBasisAuto: {
    minWidth: 50,
    minHeight: 50,
    backgroundColor: '#333333',
    margin: 5,
    flexBasis: 'auto',
  },

  boxWithFlexBasisPoint: {
    minWidth: 50,
    minHeight: 50,
    backgroundColor: '#333333',
    margin: 5,
    flexBasis: 60,
  },

  boxWithFlexBasisPercent: {
    minWidth: 50,
    minHeight: 50,
    backgroundColor: '#333333',
    margin: 5,
    flexBasis: '25%',
  },

  wideBox: {
    width: 150,
    minHeight: 50,
    backgroundColor: '#333333',
    margin: 5,
  },

  wideBoxWithShrink: {
    width: 150,
    minHeight: 50,
    backgroundColor: '#333333',
    margin: 5,
    flexShrink: 1.0,
  },

  absoluteBox: {
    minWidth: 50,
    minHeight: 50,
    backgroundColor: '#333333',
    margin: 5,
    left: 150,
    top: -20,
  },

  boxWithMaxHeight: {
    minWidth: 50,
    minHeight: 50,
    maxHeight: 60,
    backgroundColor: '#333333',
    margin: 5,
  },

  boxNoMargin: {
    width: 50,
    height: 50,
    backgroundColor: '#333333',
  },
});

AppRegistry.registerComponent('Bootstrap', () => FlexboxLayoutPlayground);
