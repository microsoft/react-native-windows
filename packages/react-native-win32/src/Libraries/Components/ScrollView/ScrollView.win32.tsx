/**
 * @providesModule ScrollView
 */
'use strict';

import * as React from 'react';
import { requireNativeComponent, NativeModules, StyleSheet, findNodeHandle, View, StyleProp, ViewStyle } from 'react-native';
import { IScrollViewProps, IScrollViewState } from './ScrollViewTypes';

const RCTScrollView = requireNativeComponent('RCTScrollView');

class ScrollView extends React.Component<IScrollViewProps, IScrollViewState> {
  private styles = StyleSheet.create({
    contentContainerStyleHorizontal: {
      flexDirection: 'row'
    }
  });

  constructor(props: IScrollViewProps) {
    super(props);
    this._handleContentOnLayout = this._handleContentOnLayout.bind(this);
  }

  // to properly handle contentContainerStyle and onContentSizeChange
  // all children will get thrown inside a View that is a child of the RCTScrollView
  // this is more in line with the React Native ScrollView implementation for Android and IOS
  public render() {
    const containerStyle: StyleProp<ViewStyle> = [
      this.props.contentContainerStyle,
      this.props.horizontal && this.styles.contentContainerStyleHorizontal
    ];
    return (
      <RCTScrollView { ...this.props }>
        <View style={ containerStyle } onLayout={ this._handleContentOnLayout }>
          { this.props.children }
        </View>
      </RCTScrollView>
    );
  }

  public scrollToEnd() {
    NativeModules.UIManager.dispatchViewManagerCommand(
      findNodeHandle(this),
      NativeModules.UIManager.RCTScrollView.Commands.ScrollToEnd,
      null
    );
  }

  public scrollTo(offsets) {
    NativeModules.UIManager.dispatchViewManagerCommand(findNodeHandle(this), NativeModules.UIManager.RCTScrollView.Commands.ScrollTo, [
      offsets.x || 0,
      offsets.y || 0,
      false /*animation not supported*/
    ]);
  }

  private _handleContentOnLayout(e: INativeContainerOnLayoutEventArgs) {
    const { width, height } = e.nativeEvent.layout;
    this.props.onContentSizeChange && this.props.onContentSizeChange(width, height);
  }
}

/* subset of what will actually be passed back from native side */
interface INativeContainerOnLayoutEventArgs {
  nativeEvent: { layout: { width: number; height: number; x: number; y: number } };
}

export = ScrollView;
