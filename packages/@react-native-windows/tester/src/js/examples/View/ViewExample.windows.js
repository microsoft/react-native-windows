/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @format
 * @flow strict-local
 */

'use strict';

import type {RNTesterModule} from '../../types/RNTesterTypes';

import RNTesterText from '../../components/RNTesterText';
import * as React from 'react';
import {
  Platform,
  PlatformColor,
  Pressable,
  StyleSheet,
  View,
} from 'react-native';

class ViewBorderStyleExample extends React.Component<
  $ReadOnly<{}>,
  {showBorder: boolean},
> {
  state: {showBorder: boolean} = {
    showBorder: true,
  };

  render(): React.Node {
    return (
      <Pressable testID="border-style-button" onPress={this._handlePress}>
        <View>
          <View
            style={[
              {
                borderWidth: 1,
                padding: 5,
              },
              this.state.showBorder
                ? {
                    borderStyle: 'dashed',
                  }
                : null,
            ]}>
            <RNTesterText style={{fontSize: 11}}>
              Dashed border style
            </RNTesterText>
          </View>
          <View
            style={[
              {
                marginTop: 5,
                borderWidth: 1,
                borderRadius: 5,
                padding: 5,
              },
              this.state.showBorder
                ? {
                    borderStyle: 'dotted',
                  }
                : null,
            ]}>
            <RNTesterText style={{fontSize: 11}}>
              Dotted border style
            </RNTesterText>
          </View>
        </View>
      </Pressable>
    );
  }

  _handlePress = () => {
    this.setState({showBorder: !this.state.showBorder});
  };
}

const offscreenAlphaCompositingStyles = StyleSheet.create({
  alphaCompositing: {
    justifyContent: 'space-around',
    width: 100,
    height: 50,
    borderRadius: 100,
  },
});

class OffscreenAlphaCompositing extends React.Component<
  $ReadOnly<{testID?: ?string}>,
  {
    active: boolean,
  },
> {
  state: {active: boolean} = {
    active: false,
  };

  render(): React.Node {
    return (
      <Pressable
        testID="offscreen-alpha-compositing-button"
        onPress={this._handlePress}>
        <View>
          <RNTesterText style={{paddingBottom: 10}}>Blobs</RNTesterText>
          <View
            style={{opacity: 1.0, paddingBottom: 30}}
            needsOffscreenAlphaCompositing={this.state.active}>
            <View
              style={[
                offscreenAlphaCompositingStyles.alphaCompositing,
                {marginTop: 0, marginLeft: 0, backgroundColor: '#FF6F59'},
              ]}
            />
            <View
              style={[
                offscreenAlphaCompositingStyles.alphaCompositing,
                {
                  marginTop: -50,
                  marginLeft: 50,
                  backgroundColor: '#F7CB15',
                },
              ]}
            />
          </View>
          <RNTesterText style={{paddingBottom: 10}}>
            Same blobs, but their shared container have 0.5 opacity
          </RNTesterText>
          <RNTesterText style={{paddingBottom: 10}}>
            Tap to {this.state.active ? 'activate' : 'deactivate'}{' '}
            needsOffscreenAlphaCompositing
          </RNTesterText>
          <View
            style={{opacity: 0.8}}
            needsOffscreenAlphaCompositing={this.state.active}>
            <View
              style={[
                offscreenAlphaCompositingStyles.alphaCompositing,
                {marginTop: 0, marginLeft: 0, backgroundColor: '#FF6F59'},
              ]}
            />
            <View
              style={[
                offscreenAlphaCompositingStyles.alphaCompositing,
                {
                  marginTop: -50,
                  marginLeft: 50,
                  backgroundColor: '#F7CB15',
                },
              ]}
            />
          </View>
        </View>
      </Pressable>
    );
  }

  _handlePress = () => {
    this.setState({active: !this.state.active});
  };
}

const ZIndexExampleStyles = StyleSheet.create({
  zIndex: {
    justifyContent: 'space-around',
    width: 100,
    height: 50,
    marginTop: -10,
    position: 'relative',
  },
});

class ZIndexExample extends React.Component<
  $ReadOnly<{}>,
  {
    flipped: boolean,
  },
> {
  state: {flipped: boolean} = {
    flipped: false,
  };

  render(): React.Node {
    const indices = this.state.flipped ? [-1, 0, 1, 2] : [2, 1, 0, -1];
    return (
      <Pressable testID="z-index-button" onPress={this._handlePress}>
        <View>
          <RNTesterText style={{paddingBottom: 10}}>
            Tap to flip sorting order
          </RNTesterText>
          <View
            style={[
              ZIndexExampleStyles.zIndex,
              {
                marginTop: 0,
                backgroundColor: '#E57373',
                zIndex: indices[0],
              },
            ]}>
            <RNTesterText>ZIndex {indices[0]}</RNTesterText>
          </View>
          <View
            style={[
              ZIndexExampleStyles.zIndex,
              {
                marginLeft: 50,
                backgroundColor: '#FFF176',
                zIndex: indices[1],
              },
            ]}>
            <RNTesterText>ZIndex {indices[1]}</RNTesterText>
          </View>
          <View
            style={[
              ZIndexExampleStyles.zIndex,
              {
                marginLeft: 100,
                backgroundColor: '#81C784',
                zIndex: indices[2],
              },
            ]}>
            <RNTesterText>ZIndex {indices[2]}</RNTesterText>
          </View>
          <View
            style={[
              ZIndexExampleStyles.zIndex,
              {
                marginLeft: 150,
                backgroundColor: '#64B5F6',
                zIndex: indices[3],
              },
            ]}>
            <RNTesterText>ZIndex {indices[3]}</RNTesterText>
          </View>
        </View>
      </Pressable>
    );
  }

  _handlePress = () => {
    this.setState({flipped: !this.state.flipped});
  };
}

function PositionStaticZIndexExample(): React.Node {
  return (
    <View
      testID="view-test-zindex-with-static"
      style={{backgroundColor: 'yellow', flexDirection: 'row'}}>
      <View
        style={{
          backgroundColor: 'red',
          width: 100,
          height: 100,
          position: 'relative',
          left: 10,
        }}>
        <View
          style={{
            backgroundColor: 'purple',
            width: 50,
            height: 50,
            top: 30,
            left: 65,
            position: 'relative',
          }}
        />
      </View>
      <View
        style={{
          backgroundColor: 'blue',
          width: 100,
          height: 100,
          zIndex: 100,
          position: 'static',
        }}>
        <View
          style={{
            backgroundColor: 'orange',
            width: 50,
            height: 50,
            top: 10,
            position: 'relative',
          }}
        />
        <View
          style={{
            backgroundColor: 'brown',
            width: 50,
            height: 50,
            position: 'static',
          }}>
          <View
            style={{
              backgroundColor: 'black',
              width: 25,
              height: 25,
              top: -10,
              position: 'relative',
            }}
          />
        </View>
      </View>
      <View
        style={{
          backgroundColor: 'green',
          width: 100,
          height: 100,
          position: 'relative',
          left: -20,
        }}
      />
    </View>
  );
}

class DisplayNoneStyle extends React.Component<
  $ReadOnly<{}>,
  {
    index: number,
  },
> {
  state: {index: number} = {
    index: 0,
  };

  render(): React.Node {
    return (
      <Pressable testID="display-none-button" onPress={this._handlePress}>
        <View>
          <RNTesterText style={{paddingBottom: 10}}>
            Press to toggle `display: none`
          </RNTesterText>
          <View
            style={{
              height: 50,
              width: 50,
              backgroundColor: 'red',
              display: this.state.index % 2 === 0 ? 'none' : 'flex',
            }}
          />
          <View
            style={{
              height: 50,
              width: 50,
              backgroundColor: 'blue',
              display: this.state.index % 3 === 0 ? 'none' : 'flex',
            }}
          />
          <View
            style={{
              height: 50,
              width: 50,
              backgroundColor: 'yellow',
              display: this.state.index % 5 === 0 ? 'none' : 'flex',
            }}>
            <View
              style={{
                height: 30,
                width: 30,
                backgroundColor: 'salmon',
                display: this.state.index % 11 === 0 ? 'none' : 'flex',
              }}
            />
          </View>
          <View
            style={{
              height: 50,
              width: 50,
              backgroundColor: 'magenta',
              display: this.state.index % 7 === 0 ? 'none' : 'flex',
            }}
          />
        </View>
      </Pressable>
    );
  }

  _handlePress = () => {
    this.setState({index: this.state.index + 1});
  };
}

class FlexGapExample extends React.Component<$ReadOnly<{testID?: ?string}>> {
  render(): React.Node {
    return (
      <View
        testID={this.props.testID}
        style={{
          flexDirection: 'row',
          flexWrap: 'wrap',
          borderWidth: 1,
          rowGap: 20,
          columnGap: 30,
        }}
        accessible
        accessibilityLabel="Flex Gap Example">
        <View style={{backgroundColor: 'black', height: 30, width: 30}} />
        <View style={{backgroundColor: 'black', height: 30, width: 30}} />
        <View
          style={{
            backgroundColor: 'pink',
            height: 30,
            flexBasis: 30,
          }}
        />
        <View style={{backgroundColor: 'black', height: 30, width: 30}} />
        <View style={{backgroundColor: 'black', height: 30, width: 30}} />
        <View style={{backgroundColor: 'black', height: 30, width: 30}} />
        <View style={{backgroundColor: 'black', height: 30, width: 30}} />
        <View style={{backgroundColor: 'pink', height: 30, width: 30}} />
        <View style={{backgroundColor: 'pink', height: 30, width: 30}} />
        <View style={{backgroundColor: 'pink', height: 30, width: 30}} />
        <View style={{backgroundColor: 'pink', height: 30, width: 30}} />
      </View>
    );
  }
}

function BoxShadowExample(): React.Node {
  const defaultStyleSize = {width: 50, height: 50};

  return (
    <View
      testID="view-test-box-shadow"
      style={{flexDirection: 'row', flexWrap: 'wrap', gap: 30, padding: 20}}>
      <View
        style={{
          ...defaultStyleSize,
          borderRadius: 10,
          borderWidth: 5,
          borderColor: 'red',
          boxShadow: '0 0 10px 0 black',
        }}
      />
      <View
        style={{
          ...defaultStyleSize,
          borderRadius: 30,
          borderWidth: 5,
          borderColor: 'red',
          boxShadow: 'inset 0 0 10px 0 black',
        }}
      />
      <View
        style={{
          ...defaultStyleSize,
          borderRadius: 30,
          borderWidth: 5,
          borderColor: 'red',
          boxShadow:
            'inset 15px -5px 5px 5px cyan, inset 15px -5px 20px 10px orange, -5px 5px 5px 0px green, 0px -10px 0px 5px black',
        }}
      />
      <View
        style={{
          ...defaultStyleSize,
          boxShadow: '0px 0px 5px 5px black',
        }}>
        <View
          style={{
            ...defaultStyleSize,
            backgroundColor: 'red',
            boxShadow: 'inset 0px 0px 5px 5px black',
          }}
        />
        <View
          style={{
            ...defaultStyleSize,
            boxShadow: ' 0px 0px 20px 5px black',
          }}
        />
      </View>
      <View
        style={{
          ...defaultStyleSize,
          backgroundColor: 'green',
          boxShadow: '0px 10px',
        }}
      />
      <View
        style={{
          ...defaultStyleSize,
          backgroundColor: 'orange',
          boxShadow: '5px 5px 5px 0px rgba(0, 0, 0, 0)',
        }}
      />
      <View
        style={[
          defaultStyleSize,
          {
            backgroundColor: 'brown',
            boxShadow: '5px 5px 5px 0px rgba(0, 0, 255, 1) inset',
            transform: [{scale: 1.2}],
          },
        ]}
      />
      <View
        style={{
          ...defaultStyleSize,
          backgroundColor: 'hotpink',
          boxShadow: [
            {
              offsetX: 5,
              offsetY: '5px',
              blurRadius: 5,
              spreadDistance: '1px',
              color: 'green',
            },
            {
              offsetX: '-5px',
              offsetY: '0',
              blurRadius: '5px',
              spreadDistance: 2,
              color: Platform.select({
                ios: PlatformColor('systemOrange'),
                android: PlatformColor('?attr/colorError'),
                default: 'orange',
              }),
            },
          ],
        }}
      />
      {/* Invalid, should not render */}
      <View
        style={{
          ...defaultStyleSize,
          backgroundColor: 'hotpink',
          boxShadow: [
            {
              offsetX: '-5px',
              offsetY: '',
              blurRadius: 5,
              spreadDistance: '1px',
              color: 'green',
            },
          ],
        }}
      />
    </View>
  );
}

class AccessibilityExample extends React.Component<
  $ReadOnly<{||}>,
  {|tap: number|},
> {
  state: {tap: number} = {
    tap: 0,
    expanded: true,
  };

  render(): React.Node {
    return (
      <View
        accessibilityLabel="A View with accessibility values"
        accessibilityHint="Accessibility Hint"
        accessibilityRole="button"
        accessibilityValue={{now: this.state.tap}}
        accessibilityActions={[
          {name: 'expand', label: 'expand'},
          {name: 'collapse', label: 'collapse'},
        ]}
        accessibilityState={{
          expanded: this.state.expanded,
          busy: true,
          checked: true,
        }}
        accessibilityPosInSet={1}
        accessibilitySetSize={1}
        accessibilityLiveRegion="polite"
        testID="accessibility"
        accessible
        focusable
        onAccessibilityAction={event => {
          switch (event.nativeEvent.actionName) {
            case 'expand':
              this.setState({expanded: true});
              break;
            case 'collapse':
              this.setState({expanded: false});
          }
        }}
        onAccessibilityTap={() => {
          this.setState({tap: this.state.tap + 1});
        }}
        onPress={() => {
          this.setState({expanded: !this.state.expanded});
          console.log('Pressed');
        }}>
        <RNTesterText>A View with accessibility values.</RNTesterText>
        <RNTesterText>
          Current Number of Accessibility Taps: {this.state.tap}
        </RNTesterText>
        <View importantForAccessibility="no-hide-descendants">
          <RNTesterText>
            This element should be hidden from accessibility.
          </RNTesterText>
        </View>
        <View accessible accessibilityValue={{now: this.state.tap}}>
          <RNTesterText>
            This sub-view should not have an accessibility value. It's control
            type does not support the value pattern.
          </RNTesterText>
        </View>
      </View>
    );
  }
}

class HitSlopExample extends React.Component<
  $ReadOnly<{||}>,
  {|color: string|},
> {
  state: {color: string} = {
    color: 'grey',
  };

  render(): React.Node {
    return (
      <View
        style={{
          height: 200,
          width: 200,
          backgroundColor: this.state.color,
        }}
        hitSlop={{top: 100, bottom: 100, left: 0, right: 100}}
        onTouchStart={() => {
          this.setState({color: 'red'});
        }}
        onTouchEnd={() => {
          this.setState({color: 'grey'});
        }}
        testID="hitslop"
        accessible
        accessibilityLabel="HitSlop Example"
      />
    );
  }
}

function BoxSizingExample(): React.Node {
  const styles = StyleSheet.create({
    boxSizingBox: {
      padding: 5,
      backgroundColor: 'green',
      borderWidth: 5,
      margin: 10,
      width: 50,
      height: 25,
    },
    boxSizingChild: {
      backgroundColor: 'red',
      width: '100%',
      height: '100%',
    },
  });

  return (
    <View testID={'view-test-box-sizing'}>
      <RNTesterText>Content box 50x25</RNTesterText>
      <View style={[styles.boxSizingBox, {boxSizing: 'content-box'}]}>
        <View style={styles.boxSizingChild} />
      </View>
      <RNTesterText>Border box 50x25</RNTesterText>
      <View style={[styles.boxSizingBox, {boxSizing: 'border-box'}]}>
        <View style={styles.boxSizingChild} />
      </View>
    </View>
  );
}

class FastRefreshStyleExample extends React.Component<
  $ReadOnly<{}>,
  {currentStyle: number},
> {
  state: {currentStyle: number} = {
    currentStyle: 0,
  };

  styles = [
    {backgroundColor: '#ff6b6b', padding: 20, borderRadius: 5},
    {backgroundColor: '#4ecdc4', padding: 15, borderRadius: 10},
    {backgroundColor: '#45b7d1', padding: 25, borderRadius: 15},
    {backgroundColor: '#96ceb4', padding: 10, borderRadius: 20},
  ];

  render(): React.Node {
    return (
      <View testID="view-test-fast-refresh" accessible accessibilityLabel="Fast Refresh Example">
        <Pressable onPress={this._handlePress}>
          <View style={this.styles[this.state.currentStyle]}>
            <RNTesterText style={{color: 'white', fontSize: 14, textAlign: 'center'}}>
              Tap to change style (simulates fast refresh)
            </RNTesterText>
            <RNTesterText style={{color: 'white', fontSize: 12, textAlign: 'center', marginTop: 5}}>
              Style: {this.state.currentStyle + 1} of {this.styles.length}
            </RNTesterText>
          </View>
        </Pressable>
      </View>
    );
  }

  _handlePress = () => {
    this.setState({
      currentStyle: (this.state.currentStyle + 1) % this.styles.length,
    });
  };
}

export default ({
  title: 'View',
  documentationURL: 'https://reactnative.dev/docs/view',
  category: 'Basic',
  description: ('Basic building block of all UI, examples that ' +
    'demonstrate some of the many styles available.': string),
  displayName: 'ViewExample',
  examples: [
    {
      title: 'Background Color',
      name: 'background-color',
      render(): React.Node {
        return (
          <View
            testID="view-test-background-color"
            style={{backgroundColor: '#527FE4', padding: 5}}
            accessible
            accessibilityLabel="Background Color Example">
            <RNTesterText style={{fontSize: 11}}>Blue background</RNTesterText>
          </View>
        );
      },
    },
    {
      title: 'Border',
      name: 'border',
      render(): React.Node {
        return (
          <View
            testID="view-test-border"
            style={{borderColor: '#527FE4', borderWidth: 5, padding: 10}}
            accessible
            accessibilityLabel="Border Example">
            <RNTesterText style={{fontSize: 11}}>5px blue border</RNTesterText>
          </View>
        );
      },
    },
    {
      title: 'Padding/Margin',
      name: 'padding-margin',
      render(): React.Node {
        const styles = StyleSheet.create({
          box: {
            backgroundColor: '#527FE4',
            borderColor: '#000033',
            borderWidth: 1,
          },
        });
        return (
          <View
            testID="view-test-padding-margin"
            style={{borderColor: '#bb0000', borderWidth: 0.5}}
            accessible
            accessibilityLabel="Padding/Margin Example">
            <View style={[styles.box, {padding: 5}]}>
              <RNTesterText style={{fontSize: 11}}>5px padding</RNTesterText>
            </View>
            <View style={[styles.box, {margin: 5}]}>
              <RNTesterText style={{fontSize: 11}}>5px margin</RNTesterText>
            </View>
            <View
              style={[
                styles.box,
                {margin: 5, padding: 5, alignSelf: 'flex-start'},
              ]}>
              <RNTesterText style={{fontSize: 11}}>
                5px margin and padding,
              </RNTesterText>
              <RNTesterText style={{fontSize: 11}}>
                widthAutonomous=true
              </RNTesterText>
            </View>
          </View>
        );
      },
    },
    {
      title: 'Border Radius',
      name: 'border-radius',
      render(): React.Node {
        return (
          <View
            testID="view-test-border-radius"
            accessible
            accessibilityLabel="Border Radius Example">
            <View style={{borderWidth: 0.5, borderRadius: 5, padding: 5}}>
              <RNTesterText style={{fontSize: 11}}>
                Too much use of `borderRadius` (especially large radii) on
                anything which is scrolling may result in dropped frames. Use
                sparingly.
              </RNTesterText>
            </View>
            {Platform.OS === 'ios' && (
              <View
                style={{
                  borderRadius: 20,
                  padding: 8,
                  marginTop: 12,
                  backgroundColor: '#527FE4',
                  borderCurve: 'continuous',
                }}>
                <RNTesterText style={{fontSize: 16, color: 'white'}}>
                  View with continuous border curve
                </RNTesterText>
              </View>
            )}
          </View>
        );
      },
    },
    {
      title: 'Border Style',
      name: 'border-style',
      render(): React.Node {
        return <ViewBorderStyleExample />;
      },
    },
    {
      title: 'Rounded Borders',
      name: 'rounded-borders',
      render(): React.Node {
        return (
          <View
            testID="view-test-rounded-borders"
            style={{flexDirection: 'row', flexWrap: 'wrap'}}
            accessible
            accessibilityLabel="Rounded Borders Example">
            <View
              style={{
                width: 50,
                height: 50,
                borderRadius: 25,
                borderWidth: 1,
                marginRight: 10,
              }}
            />
            <View
              style={{
                width: 50,
                height: 50,
                borderRadius: 25,
                borderWidth: 10,
                marginRight: 10,
              }}
            />
            <View
              style={{
                width: 50,
                height: 50,
                borderTopLeftRadius: 5,
                borderTopRightRadius: 10,
                borderBottomRightRadius: 25,
                borderBottomLeftRadius: 50,
                borderWidth: 1,
                marginRight: 10,
              }}
            />
            <View
              style={{
                width: 50,
                height: 50,
                borderTopLeftRadius: 5,
                borderTopRightRadius: 10,
                borderBottomRightRadius: 25,
                borderBottomLeftRadius: 50,
                borderWidth: 10,
                marginRight: 10,
              }}
            />
            <View
              style={{
                width: 50,
                height: 50,
                borderLeftWidth: 6,
                borderTopWidth: 6,
                borderTopLeftRadius: 20,
              }}
            />
            <View
              style={{
                width: 50,
                height: 50,
                borderRightWidth: 6,
                borderTopWidth: 6,
                borderTopRightRadius: 20,
              }}
            />
            <View
              style={{
                width: 50,
                height: 50,
                borderBottomWidth: 6,
                borderLeftWidth: 6,
                borderBottomLeftRadius: 20,
              }}
            />
            <View
              style={{
                width: 50,
                height: 50,
                borderBottomWidth: 6,
                borderRightWidth: 6,
                borderBottomRightRadius: 20,
              }}
            />
          </View>
        );
      },
    },
    {
      title: 'Rounded Borders (Percentages)',
      name: 'rounded-borders-percentages',
      render(): React.Node {
        return (
          <View
            testID="view-test-rounded-borders-percentages"
            style={{flexDirection: 'row', flexWrap: 'wrap'}}>
            <View
              style={{
                width: 50,
                height: 50,
                borderRadius: '50%',
                borderWidth: 1,
                marginRight: 10,
              }}
            />
            <View
              style={{
                width: 50,
                height: 50,
                borderRadius: '50%',
                borderWidth: 10,
                marginRight: 10,
              }}
            />
            <View
              style={{
                width: 50,
                height: 50,
                borderTopLeftRadius: '10%',
                borderTopRightRadius: '20%',
                borderBottomRightRadius: '50%',
                borderBottomLeftRadius: '100%',
                borderWidth: 1,
                marginRight: 10,
              }}
            />
            <View
              style={{
                width: 50,
                height: 50,
                borderTopLeftRadius: '10%',
                borderTopRightRadius: '20%',
                borderBottomRightRadius: '50%',
                borderBottomLeftRadius: '100%',
                borderWidth: 10,
                marginRight: 10,
              }}
            />
            <View
              style={{
                width: 50,
                height: 50,
                borderLeftWidth: 6,
                borderTopWidth: 6,
                borderTopLeftRadius: '40%',
              }}
            />
            <View
              style={{
                width: 50,
                height: 50,
                borderRightWidth: 6,
                borderTopWidth: 6,
                borderTopRightRadius: '40%',
              }}
            />
            <View
              style={{
                width: 50,
                height: 50,
                borderBottomWidth: 6,
                borderLeftWidth: 6,
                borderBottomLeftRadius: '40%',
              }}
            />
            <View
              style={{
                width: 50,
                height: 50,
                borderBottomWidth: 6,
                borderRightWidth: 6,
                borderBottomRightRadius: '40%',
              }}
            />
            <View
              style={{
                width: 100,
                height: 50,
                borderWidth: 6,
                borderRadius: '100%',
              }}
            />
            <View
              style={{
                width: 100,
                height: 50,
                borderWidth: 6,
                borderTopLeftRadius: '10%',
                borderTopRightRadius: '20%',
                borderBottomRightRadius: '50%',
                borderBottomLeftRadius: '100%',
              }}
            />
            <View
              style={{
                width: 100,
                height: 50,
                borderWidth: 6,
                borderRadius: '0%',
              }}
            />
          </View>
        );
      },
    },
    {
      title: 'Overflow',
      name: 'overflow',
      render(): React.Node {
        const styles = StyleSheet.create({
          container: {
            borderWidth: StyleSheet.hairlineWidth,
            height: 12,
            marginBottom: 8,
            marginEnd: 16,
            width: 95,
          },
          content: {
            height: 20,
            width: 200,
          },
        });

        // NOTE: The <View> that sets `overflow` should only have other layout
        // styles so that we can accurately test view flattening optimizations.
        return (
          <View
            testID="view-test-overflow"
            style={{flexDirection: 'row'}}
            accessible
            accessibilityLabel="Rounded Borders Example">
            <View style={styles.container}>
              <View style={[StyleSheet.absoluteFill]}>
                <RNTesterText style={styles.content}>undefined</RNTesterText>
              </View>
            </View>
            <View style={styles.container}>
              <View style={[StyleSheet.absoluteFill, {overflow: 'hidden'}]}>
                <RNTesterText style={styles.content}>hidden</RNTesterText>
              </View>
            </View>
            <View style={styles.container}>
              <View style={[StyleSheet.absoluteFill, {overflow: 'visible'}]}>
                <RNTesterText style={styles.content}>visible</RNTesterText>
              </View>
            </View>
          </View>
        );
      },
    },
    {
      title: 'Opacity',
      name: 'opacity',
      render(): React.Node {
        return (
          <View
            testID="view-test-opacity"
            accessible
            accessibilityLabel="Opacity Example">
            <View style={{opacity: 0}}>
              <RNTesterText>Opacity 0</RNTesterText>
            </View>
            <View style={{opacity: 0.1}}>
              <RNTesterText>Opacity 0.1</RNTesterText>
            </View>
            <View style={{opacity: 0.3}}>
              <RNTesterText>Opacity 0.3</RNTesterText>
            </View>
            <View style={{opacity: 0.5}}>
              <RNTesterText>Opacity 0.5</RNTesterText>
            </View>
            <View style={{opacity: 0.7}}>
              <RNTesterText>Opacity 0.7</RNTesterText>
            </View>
            <View style={{opacity: 0.9}}>
              <RNTesterText>Opacity 0.9</RNTesterText>
            </View>
            <View style={{opacity: 1}}>
              <RNTesterText>Opacity 1</RNTesterText>
            </View>
          </View>
        );
      },
    },
    {
      title: 'Offscreen Alpha Compositing',
      name: 'offscreen-alpha-compositing',
      render(): React.Node {
        return (
          <OffscreenAlphaCompositing testID="view-test-offscreen-alpha-compositing" />
        );
      },
    },
    {
      title: 'ZIndex',
      name: 'z-index',
      render(): React.Node {
        return <ZIndexExample />;
      },
    },
    {
      title: 'ZIndex With Static',
      name: 'zindex-with-static',
      render: PositionStaticZIndexExample,
    },
    {
      title: '`display: none` style',
      name: 'display-none',
      render(): React.Node {
        return <DisplayNoneStyle />;
      },
    },
    {
      title: 'BackfaceVisibility',
      name: 'backface-visibility',
      render(): React.Node {
        return (
          <View
            testID="view-test-backface-visibility"
            accessible
            accessibilityLabel="Backface Visibility Example">
            <RNTesterText style={{paddingBottom: 10}}>
              View #1, front is visible, back is hidden.
            </RNTesterText>
            <View style={{justifyContent: 'center', alignItems: 'center'}}>
              <View
                style={{
                  height: 150,
                  width: 150,
                  justifyContent: 'center',
                  alignItems: 'center',
                  backgroundColor: 'blue',
                  backfaceVisibility: 'hidden',
                }}>
                <RNTesterText>Front</RNTesterText>
              </View>
              <View
                style={{
                  height: 150,
                  width: 150,
                  justifyContent: 'center',
                  alignItems: 'center',
                  backgroundColor: 'red',
                  backfaceVisibility: 'hidden',
                  transform: [{rotateY: '180deg'}],
                  position: 'absolute',
                  top: 0,
                }}>
                <RNTesterText>Back (You should not see this)</RNTesterText>
              </View>
            </View>
            <RNTesterText style={{paddingVertical: 10}}>
              View #2, front is hidden, back is visible.
            </RNTesterText>
            <View style={{justifyContent: 'center', alignItems: 'center'}}>
              <View
                style={{
                  height: 150,
                  width: 150,
                  justifyContent: 'center',
                  alignItems: 'center',
                  backgroundColor: 'blue',
                  backfaceVisibility: 'hidden',
                }}>
                <RNTesterText>Front (You should not see this)</RNTesterText>
              </View>
              <View
                style={{
                  height: 150,
                  width: 150,
                  justifyContent: 'center',
                  alignItems: 'center',
                  backgroundColor: 'red',
                  backfaceVisibility: 'hidden',
                  position: 'absolute',
                  top: 0,
                }}>
                <RNTesterText>Back</RNTesterText>
              </View>
            </View>
          </View>
        );
      },
    },
    {
      title: 'View with aria-label="label"',
      name: 'aria-label',
      render(): React.Node {
        return (
          <View
            testID="view-test-aria-label"
            aria-label="Blue background View with Text"
            style={{backgroundColor: '#527FE4', padding: 5}}
            accessible
            accessibilityLabel="Aria-Label Example">
            <RNTesterText style={{fontSize: 11}}>Blue background</RNTesterText>
          </View>
        );
      },
    },
    {
      title: 'FlexGap',
      name: 'flexgap',
      render(): React.Node {
        return <FlexGapExample testID="view-test-flexgap" />;
      },
    },
    {
      title: 'Insets',
      name: 'insets',
      render(): React.Node {
        return (
          <View testID="view-test-insets" style={{rowGap: 5}} accessible>
            <View style={{position: 'relative', height: 40, borderWidth: 1}}>
              <View
                style={{
                  backgroundColor: '#527FE4',
                  padding: 5,
                  position: 'absolute',
                  inset: 5,
                }}>
                <RNTesterText style={{fontSize: 11}}>inset 5</RNTesterText>
              </View>
            </View>
            <View style={{position: 'relative', height: 40, borderWidth: 1}}>
              <View
                style={{
                  backgroundColor: '#527FE4',
                  padding: 5,
                  position: 'absolute',
                  insetBlock: 5,
                }}>
                <RNTesterText style={{fontSize: 11}}>insetBlock 5</RNTesterText>
              </View>
            </View>
            <View style={{position: 'relative', height: 40, borderWidth: 1}}>
              <View
                style={{
                  backgroundColor: '#527FE4',
                  padding: 5,
                  position: 'absolute',
                  insetBlockEnd: 5,
                }}>
                <RNTesterText style={{fontSize: 11}}>
                  insetBlockEnd 5
                </RNTesterText>
              </View>
            </View>
            <View style={{position: 'relative', height: 40, borderWidth: 1}}>
              <View
                style={{
                  backgroundColor: '#527FE4',
                  padding: 5,
                  position: 'absolute',
                  insetBlockStart: 5,
                }}>
                <RNTesterText style={{fontSize: 11}}>
                  insetBlockStart 5
                </RNTesterText>
              </View>
            </View>
            <View style={{position: 'relative', height: 40, borderWidth: 1}}>
              <View
                style={{
                  backgroundColor: '#527FE4',
                  padding: 5,
                  position: 'absolute',
                  insetInline: 5,
                }}>
                <RNTesterText style={{fontSize: 11}}>
                  insetInline 5
                </RNTesterText>
              </View>
            </View>
            <View style={{position: 'relative', height: 40, borderWidth: 1}}>
              <View
                style={{
                  backgroundColor: '#527FE4',
                  padding: 5,
                  position: 'absolute',
                  insetInlineEnd: 5,
                }}>
                <RNTesterText style={{fontSize: 11}}>
                  insetInlineEnd 5
                </RNTesterText>
              </View>
            </View>
            <View style={{position: 'relative', height: 40, borderWidth: 1}}>
              <View
                style={{
                  backgroundColor: '#527FE4',
                  padding: 5,
                  position: 'absolute',
                  insetInlineStart: 5,
                }}>
                <RNTesterText style={{fontSize: 11}}>
                  insetInlineStart 5
                </RNTesterText>
              </View>
            </View>
          </View>
        );
      },
    },
    {
      title: 'Logical Border Color',
      name: 'logical-border-color',
      render(): React.Node {
        return (
          <View
            testID="view-test-logical-border-color"
            style={{rowGap: 10}}
            accessible
            accessibilityLabel="Logical Border Color Example">
            <View style={{position: 'relative', height: 50, borderWidth: 1}}>
              <View
                style={{
                  borderBlockColor: 'orange',
                  borderWidth: 5,
                  position: 'absolute',
                  top: 10,
                  bottom: 10,
                  left: 10,
                  right: 10,
                }}>
                <RNTesterText style={{fontSize: 11}}>
                  borderBlockColor orange
                </RNTesterText>
              </View>
            </View>
            <View style={{position: 'relative', height: 65, borderWidth: 1}}>
              <View
                style={{
                  borderBlockEndColor: 'green',
                  borderBlockStartColor: 'purple',
                  borderWidth: 5,
                  position: 'absolute',
                  top: 10,
                  bottom: 10,
                  left: 10,
                  right: 10,
                }}>
                <RNTesterText style={{fontSize: 11}}>
                  borderBlockStartColor purple
                </RNTesterText>
                <RNTesterText style={{fontSize: 11}}>
                  borderBlockEndColor green
                </RNTesterText>
              </View>
            </View>
          </View>
        );
      },
    },
    {
      title: 'Box Shadow',
      name: 'box-shadow',
      render: BoxShadowExample,
    },
    {
      title: 'Box Sizing',
      name: 'box-sizing',
      render: BoxSizingExample,
    },
    // [Windows
    {
      title: 'NativeID',
      render(): React.Node {
        return (
          <View
            nativeID="native-id-view"
            testID="nativeid"
            accessible
            accessibilityLabel="NativeID Example">
            <RNTesterText>A View with a nativeID "native-id-view"</RNTesterText>
          </View>
        );
      },
    },
    {
      title: 'Accessibility',
      render(): React.Node {
        return <AccessibilityExample />;
      },
    },
    {
      title: 'Shadow',
      render(): React.Node {
        return (
          <View
            style={{
              height: 50,
              width: 50,
              shadowColor: 'pink',
              shadowOpacity: 80,
              shadowOffset: {width: 10, height: 10},
              shadowRadius: 10,
            }}
            testID="shadow"
            accessible
            accessibilityLabel="Shadow Example"
          />
        );
      },
    },
    {
      title: 'HitSlop',
      render(): React.Node {
        return <HitSlopExample />;
      },
    },
    {
      title: 'Fast Refresh Style Updates',
      name: 'fast-refresh',
      render(): React.Node {
        return <FastRefreshStyleExample />;
      },
    },
    {
      title: 'ToolTip',
      render(): React.Node {
        return (
          <View
            tooltip="Parent View"
            testID="tool-tip"
            accessible
            accessibilityLabel="Tooltip Example">
            <RNTesterText style={{fontSize: 11}}>
              This Parent View has tooltip "Parent View"
            </RNTesterText>
            <View tooltip="Child View 1">
              <RNTesterText style={{fontSize: 11}}>
                This view has tooltip "Child View 1"
              </RNTesterText>
            </View>
            <View tooltip="Child View 2">
              <RNTesterText style={{fontSize: 11}}>
                This view has tooltip "Child View 2"
              </RNTesterText>
            </View>
          </View>
        );
      },
    }, // ]TODO(macOS ISS#2323203)
    // Windows]
  ],
}: RNTesterModule);
