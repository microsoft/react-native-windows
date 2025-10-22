/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import React from 'react';
import {
  FlatList,
  Text,
  TouchableHighlight,
  View,
  StyleSheet,
} from 'react-native';

import {AppTheme, IHighContrastChangedEvent} from 'react-native-windows';

class AccessibilityBaseExample extends React.Component {
  public render() {
    return (
      <View>
        <Text>The following has accessibilityLabel and accessibilityHint:</Text>
        <View
          style={{width: 50, height: 50, backgroundColor: 'blue'}}
          accessible={true}
          accessibilityLabel="A blue box"
          role="heading"
          accessibilityHint="A hint for the blue box."
          accessibilityLevel={1}
          accessibilityItemType="comment"
          accessibilityAccessKey="accessKey"
          accessibilityDescription="Sample Description"
          accessibilityAnnotation={{
            typeID: 'Comment',
            typeName: 'Check Comment',
            author: 'Clint Westwood',
            dateTime: '3/19/2025 1:03 PM',
          }}
          testID="accessibility-base-view-1"
        />
        <Text>The following has accessible and accessibilityLabel:</Text>
        <View
          style={{width: 50, height: 50, backgroundColor: 'red'}}
          accessible={true}
          accessibilityLabel="A hint for the red box."
          accessibilityRole="header"
          accessibilityLevel={2}
          testID="accessibility-base-view-2"
        />
        <Text>
          The following has accessibilitySetSize, accessibilityPosInSet and
          accessibilityLabel:
        </Text>
        <View
          style={{width: 50, height: 50, backgroundColor: 'red'}}
          testID="accessibility-base-view-3"
          accessible={true}
          accessibilityRole="listitem"
          accessibilityLabel="This label should not be used"
          aria-label="Aria label takes precedence"
          accessibilitySetSize={5}
          accessibilityPosInSet={2}
          aria-level={9} //aria-level takes precedence over accessibilityLevel
          accessibilityLevel={5}
        />
      </View>
    );
  }
}

class HighContrastExample extends React.Component {
  state = {
    isHighContrast: AppTheme.isHighContrast,
    highContrastColorValues: AppTheme.currentHighContrastColors,
  };

  componentDidMount() {
    AppTheme.addListener('highContrastChanged', this.onHighContrastChanged);
  }

  componenetWillUnmount() {
    AppTheme.removeListener('highContrastChanged', this.onHighContrastChanged);
  }

  // TODO: Make args props
  onHighContrastChanged = (event: IHighContrastChangedEvent) => {
    this.setState({
      isHighContrast: event.isHighContrast,
      highContrastColorValues: event.highContrastColors,
    });
  };

  public render() {
    return (
      <View>
        <Text>The following has HighContrast Event awareness:</Text>
        <View>
          <Text>
            isHighContrast: {this.state.isHighContrast ? 'True' : 'False'}
          </Text>
        </View>
        <View
          style={
            this.state.isHighContrast
              ? [
                  this.styles.enabled,
                  {
                    backgroundColor:
                      this.state.highContrastColorValues.ButtonFaceColor,
                  },
                ]
              : this.styles.disabled
          }>
          <Text>
            ButtonFace High Contrast Hex Value:{' '}
            {this.state.highContrastColorValues.ButtonFaceColor}
          </Text>
        </View>
        <View
          style={
            this.state.isHighContrast
              ? [
                  this.styles.enabled,
                  {
                    backgroundColor:
                      this.state.highContrastColorValues.ButtonTextColor,
                  },
                ]
              : this.styles.disabled
          }>
          <Text>
            ButtonText High Contrast Color Hex Value:{' '}
            {this.state.highContrastColorValues.ButtonTextColor}
          </Text>
        </View>
        <View
          style={
            this.state.isHighContrast
              ? [
                  this.styles.enabled,
                  {
                    backgroundColor:
                      this.state.highContrastColorValues.GrayTextColor,
                  },
                ]
              : this.styles.disabled
          }>
          <Text>
            GrayText High Contrast Color Hex Value:{' '}
            {this.state.highContrastColorValues.GrayTextColor}
          </Text>
        </View>
        <View
          style={
            this.state.isHighContrast
              ? [
                  this.styles.enabled,
                  {
                    backgroundColor:
                      this.state.highContrastColorValues.HighlightColor,
                  },
                ]
              : this.styles.disabled
          }>
          <Text>
            Highlight High Contrast Color Hex Value:{' '}
            {this.state.highContrastColorValues.HighlightColor}
          </Text>
        </View>
        <View
          style={
            this.state.isHighContrast
              ? [
                  this.styles.enabled,
                  {
                    backgroundColor:
                      this.state.highContrastColorValues.HighlightTextColor,
                  },
                ]
              : this.styles.disabled
          }>
          <Text>
            HighlightText High Contrast Color Hex Value:{' '}
            {this.state.highContrastColorValues.HighlightTextColor}
          </Text>
        </View>
        <View
          style={
            this.state.isHighContrast
              ? [
                  this.styles.enabled,
                  {
                    backgroundColor:
                      this.state.highContrastColorValues.HotlightColor,
                  },
                ]
              : this.styles.disabled
          }>
          <Text>
            Hotlight High Contrast Color Hex Value:{' '}
            {this.state.highContrastColorValues.HotlightColor}
          </Text>
        </View>
        <View
          style={
            this.state.isHighContrast
              ? [
                  this.styles.enabled,
                  {
                    backgroundColor:
                      this.state.highContrastColorValues.WindowColor,
                  },
                ]
              : this.styles.disabled
          }>
          <Text>
            Window High Contrast Color Hex Value:{' '}
            {this.state.highContrastColorValues.WindowColor}
          </Text>
        </View>
        <View
          style={
            this.state.isHighContrast
              ? [
                  this.styles.enabled,
                  {
                    backgroundColor:
                      this.state.highContrastColorValues.WindowTextColor,
                  },
                ]
              : this.styles.disabled
          }>
          <Text>
            WindowText High Contrast Color Hex Value:{' '}
            {this.state.highContrastColorValues.WindowTextColor}
          </Text>
        </View>
      </View>
    );
  }

  styles = StyleSheet.create({
    enabled: {
      width: 250,
      height: 50,
    },
    disabled: {
      width: 250,
      height: 50,
      backgroundColor: '#808080',
    },
  });
}

class TouchableExamples extends React.Component<{}, any> {
  public state = {
    pressedCount: 0,
  };

  public render() {
    return (
      <View>
        <Text>
          The following TouchableHighlight has accessibilityLabel,
          accessibilityHint, accessibilityRole, toolip:
        </Text>
        <TouchableHighlight
          style={{width: 50, height: 50, backgroundColor: 'blue'}}
          accessibilityLabel="A blue box"
          accessibilityHint="A hint for the blue box."
          accessibilityRole="button"
          onPress={this.press}
          {...{tooltip: 'a blue tooltip'}}>
          <Text>Blue</Text>
        </TouchableHighlight>
        <Text
          accessibilityLiveRegion="polite"
          accessibilityLabel={'Pressed ' + this.state.pressedCount + ' times'}>
          Pressed {this.state.pressedCount} times
        </Text>
      </View>
    );
  }

  private readonly press = () => {
    this.setState({pressedCount: this.state.pressedCount + 1});
  };
}

class AccessibilityStateExamples extends React.Component {
  public state = {
    viewDisabled: false,
    itemsSelected: [false, false, false],
    viewChecked: 0,
    viewBusy: false,
    viewCollapsed: false,

    viewRangeNow: 10,
    viewRangeMax: 125,
    viewRangeMin: 5,
    viewValueText: 'testText',
  };

  public render() {
    const selectableItems = [{}, {}, {}];
    return (
      <View>
        <Text>
          The following TouchableHighlight toggles accessibilityState.disabled
          for the View under it:
        </Text>
        <TouchableHighlight
          style={{width: 100, height: 50, backgroundColor: 'blue'}}
          accessibilityRole="button"
          onPress={this.disablePress}>
          <Text>Toggle</Text>
        </TouchableHighlight>
        <View
          style={{
            backgroundColor: this.state.viewDisabled ? 'gray' : 'lightskyblue',
          }}
          accessibilityRole="none"
          accessibilityState={{disabled: this.state.viewDisabled}}>
          <Text>
            This View should be{' '}
            {this.state.viewDisabled ? 'disabled' : 'enabled'} according to UIA
          </Text>
        </View>
        <Text>
          The following list of TouchableHighlights toggles
          accessibilityState.selected when touched:
        </Text>
        <View
          accessible
          accessibilityLabel="Selection Container"
          accessibilityState={{multiselectable: true, required: true}}
          testID="selection-container">
          <FlatList
            accessibilityLabel="List of selectable items"
            data={selectableItems}
            renderItem={item => (
              <TouchableHighlight
                style={{
                  width: 100,
                  height: 50,
                  backgroundColor: this.state.itemsSelected[item.index]
                    ? 'gray'
                    : 'lightskyblue',
                }}
                accessibilityRole="button"
                accessibilityLabel={'Selectable item ' + (item.index + 1)}
                testID={'Selectable item ' + (item.index + 1)}
                accessibilityState={{
                  selected: this.state.itemsSelected[item.index],
                }}
                onPress={() => this.selectPress(item.index)}>
                <Text>
                  {this.state.itemsSelected[item.index]
                    ? 'Selected'
                    : 'Unselected'}
                </Text>
              </TouchableHighlight>
            )}
            keyExtractor={(item, index) => index.toString()}
          />
        </View>
        <Text>
          The following TouchableHighlight cycles accessibilityState.checked
          through unchecked/checked/mixed for the View under it:
        </Text>
        <TouchableHighlight
          style={{width: 100, height: 50, backgroundColor: 'blue'}}
          accessibilityRole="button"
          onPress={this.checkedPress}>
          <Text>Toggle</Text>
        </TouchableHighlight>
        <View
          style={{
            backgroundColor:
              this.state.viewChecked === 0
                ? 'green'
                : this.state.viewChecked === 1
                ? 'gray'
                : 'lightskyblue',
          }}
          accessibilityRole="checkbox"
          accessibilityState={{
            checked:
              this.state.viewChecked === 0
                ? false
                : this.state.viewChecked === 1
                ? true
                : 'mixed',
          }}>
          <Text>
            This View should be{' '}
            {this.state.viewChecked === 0
              ? 'Unchecked'
              : this.state.viewChecked === 1
              ? 'Checked'
              : 'Mixed'}{' '}
            according to UIA
          </Text>
        </View>
        <Text>
          The following TouchableHighlight toggles the acessibilityState.busy
          for the View under it:
        </Text>
        <TouchableHighlight
          style={{width: 100, height: 50, backgroundColor: 'blue'}}
          accessibilityRole="button"
          onPress={this.busyPress}>
          <Text>Toggle</Text>
        </TouchableHighlight>
        <View
          style={{
            backgroundColor: this.state.viewBusy ? 'gray' : 'lightskyblue',
          }}
          accessibilityRole="none"
          accessibilityState={{busy: this.state.viewBusy}}>
          <Text>
            This View should be {this.state.viewBusy ? 'Busy' : 'Not Busy'}{' '}
            according to UIA
          </Text>
        </View>
        <Text>
          The following TouchableHighlight toggles accessibilityState.expanded
          and accessibilityState.collapsed for the View under it:
        </Text>
        <TouchableHighlight
          style={{width: 100, height: 50, backgroundColor: 'blue'}}
          accessibilityRole="button"
          onPress={this.collapsePress}>
          <Text>Toggle</Text>
        </TouchableHighlight>
        <View
          style={{
            backgroundColor: this.state.viewCollapsed ? 'gray' : 'lightskyblue',
            height: this.state.viewCollapsed ? 25 : 50,
          }}
          accessibilityRole="none"
          accessibilityState={{
            expanded: !this.state.viewCollapsed,
          }}>
          <Text>
            This View should be{' '}
            {this.state.viewCollapsed ? 'Collapsed' : 'Expanded'} according to
            UIA
          </Text>
        </View>

        <Text>
          The following View exposes accessibilityValue fields (min, max, now)
        </Text>
        <TouchableHighlight
          style={{width: 100, height: 50, backgroundColor: 'blue'}}
          onPress={this.rangePress}
          testID="accessibilityValue-increment">
          <Text>Range value increment</Text>
        </TouchableHighlight>
        <View
          style={{
            backgroundColor: 'gray',
            height: 50,
          }}
          focusable={true}
          accessible={true}
          accessibilityRole="adjustable"
          accessibilityValue={{
            min: this.state.viewRangeMin,
            max: this.state.viewRangeMax,
            now: this.state.viewRangeNow,
          }}
          testID="accessibilityValue-number"
          accessibilityState={{readOnly: true}}>
          <Text>
            The View's (accessibilityRole == adjustable, ie. Slider) properties
            should be the following according to UIA: Min-{' '}
            {this.state.viewRangeMin}
            Max- {this.state.viewRangeMax}
            Now- {this.state.viewRangeNow}
          </Text>
        </View>

        <Text>
          The following View exposes the accessibilityValue.Text field
        </Text>
        <View
          style={{
            backgroundColor: 'gray',
            height: 50,
          }}
          accessibilityValue={{
            text: this.state.viewValueText,
          }}
          role="combobox"
          testID="accessibilityValue-text"
          accessible
          aria-readonly>
          <Text>
            The View's properties should be the following according to UIA:
            Text- {this.state.viewValueText}
          </Text>
        </View>
      </View>
    );
  }

  private readonly disablePress = () => {
    this.setState({viewDisabled: !this.state.viewDisabled});
  };

  private readonly selectPress = (index: number) => {
    const tmp = this.state.itemsSelected;
    tmp[index] = !tmp[index];
    this.setState({itemsSelected: tmp});
  };

  private readonly checkedPress = () => {
    let newChecked = this.state.viewChecked + 1;
    if (newChecked === 3) {
      newChecked = 0;
    }
    this.setState({viewChecked: newChecked});
  };

  private readonly busyPress = () => {
    this.setState({viewBusy: !this.state.viewBusy});
  };

  private readonly collapsePress = () => {
    this.setState({viewCollapsed: !this.state.viewCollapsed});
  };

  private readonly rangePress = () => {
    this.setState({viewRangeNow: this.state.viewRangeNow + 1});
  };
}

class AccessibilityListExamples extends React.Component {
  public render() {
    const items = [{}, {}, {}];
    return (
      <View>
        <Text>
          The following uses accessibilityRole: 'list', 'listitem',
          accessibilitySetSize, and accessibilityPosInSet.
        </Text>
        <View accessibilityRole="list">
          <FlatList
            data={items}
            renderItem={item => (
              <View
                style={{
                  width: 100,
                  height: 50,
                  backgroundColor: 'lightskyblue',
                }}
                accessibilityRole="listitem"
                accessibilitySetSize={items.length}
                accessibilityPosInSet={item.index + 1}>
                <Text>Item {item.index + 1}</Text>
              </View>
            )}
            keyExtractor={(item, index) => index.toString()}
          />
        </View>
        <View accessibilityRole="list">
          <Text>The following does the same, but with Touchables.</Text>
          <FlatList
            data={items}
            renderItem={item => (
              <TouchableHighlight
                style={{
                  width: 100,
                  height: 50,
                  backgroundColor: 'lightskyblue',
                }}
                accessibilityRole="listitem"
                accessibilitySetSize={items.length}
                accessibilityPosInSet={item.index + 1}>
                <Text>Touchable {item.index + 1}</Text>
              </TouchableHighlight>
            )}
            keyExtractor={(item, index) => index.toString()}
          />
        </View>
      </View>
    );
  }
}

export const displayName = (_undefined?: string) => {};
export const title = 'Accessibility Windows';
export const category = 'Basic';
export const description = 'Usage of accessibility properties.';
export const examples = [
  {
    title: 'Label, Hint',
    render: function () {
      return <AccessibilityBaseExample />;
    },
  },
  {
    title: 'Touchables',
    render: function () {
      return <TouchableExamples />;
    },
  },
  {
    title: 'HighContrast',
    render: function () {
      return <HighContrastExample />;
    },
  },
  {
    title: 'States',
    render: function () {
      return <AccessibilityStateExamples />;
    },
  },
  {
    title: 'Lists',
    render: function () {
      return <AccessibilityListExamples />;
    },
  },
];
