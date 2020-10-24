/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

'use strict';

const React = require('react');

const {
  StyleSheet,
  View,
  Text,
  GestureResponderEvent,
  TouchableHighlight,
  BackHandler,
} = require('react-native');

exports.displayName = 'MouseExample';
exports.title = 'Mouse Events';
exports.category = 'Basic';
exports.description = 'Tests that mouse events can be observed';
exports.examples = [
  {
    title: 'onMouseEnter and onMouseLeave affect style\n',
    render: function(): React.Node {
      return <ExampleComponent />;
    },
  },
];

const styles = StyleSheet.create({
  page: {
    flex: 1,
    justifyContent: 'center',
    alignItems: 'center',
    backgroundColor: '#FFFFFF',
  },
  mainContainer: {
    width: 400,
    height: 400,
    backgroundColor: '#CCCCCC',
  },
  contentContainer: {
    flex: 1,
    backgroundColor: '#F2E2C4',
  },
  contentContainerHovered: {
    backgroundColor: '#B3A791',
  },
  contentChild: {
    position: 'absolute',
    top: 0,
    right: 0,
    width: 200,
    height: 200,
    backgroundColor: '#C2D2F2',
  },
  contentChildHovered: {
    backgroundColor: '#8F9BB3',
  },
  overlayContainer: {
    position: 'absolute',
    right: -100,
    top: 100,
    width: 200,
    height: 200,
    backgroundColor: '#FF9C65',
  },
  overlayContainerHovered: {
    backgroundColor: '#BF754D',
  },
  overlayChild: {
    position: 'absolute',
    top: 0,
    right: 0,
    width: 100,
    height: 100,
    backgroundColor: '#99A4BF',
  },
  overlayChildHovered: {
    backgroundColor: '#666D80',
  },
});

export default class ExampleComponent extends React.Component<
  {},
  {
    clicked: number,
    pageHover: boolean,
    contentHover: boolean,
    contentChildHover: boolean,
    overlayHover: boolean,
    overlayChildHover: boolean,
  },
> {
  constructor(props: {}) {
    super(props);
    this.state = {
      clicked: 0,
      pageHover: false,
      contentHover: false,
      contentChildHover: false,
      overlayHover: false,
      overlayChildHover: false,
    };
  }

  componentDidMount() {
    BackHandler.addEventListener('hardwareBackPress', this.back);
  }

  render() {
    const pageProps: any = {
      style: styles.page,
      onMouseEnter: this.mouseEnterPage,
      onMouseLeave: this.mouseLeavePage,
    };
    return (
      <View {...pageProps}>
        <View style={styles.mainContainer}>
          {this.renderContent()}
          {this.renderOverlay()}
        </View>
        <View>
          <Text>{this.state.pageHover ? 'Mouse over page' : ''}</Text>
        </View>
      </View>
    );
  }

  renderContent(): JSX.Element {
    const containerProps: any = {
      style: [
        styles.contentContainer,
        this.state.contentHover ? styles.contentContainerHovered : undefined,
      ],
      onMouseEnter: this.mouseEnterContentContainer,
      onMouseLeave: this.mouseLeaveContentContainer,
    };
    const childProps: any = {
      style: [
        styles.contentChild,
        this.state.contentChildHover ? styles.contentChildHovered : undefined,
      ],
      onMouseEnter: this.mouseEnterContentChild,
      onMouseLeave: this.mouseLeaveContentChild,
      onPress: this.press,
      onPressIn: this.pressIn,
      onPressOut: this.pressOut,
    };

    return (
      <View {...containerProps}>
        <TouchableHighlight {...childProps}>
          <Text>This is a TouchableHighlight</Text>
        </TouchableHighlight>
      </View>
    );
  }

  renderOverlay(): JSX.Element {
    const containerProps: any = {
      style: [
        styles.overlayContainer,
        this.state.overlayHover ? styles.overlayContainerHovered : undefined,
      ],
      onMouseEnter: this.mouseEnterOverlay,
      onMouseLeave: this.mouseLeaveOverlay,
    };
    const childProps: any = {
      style: [
        styles.overlayChild,
        this.state.overlayChildHover ? styles.overlayChildHovered : undefined,
      ],
      onMouseEnter: this.mouseEnterOverlayChild,
      onMouseLeave: this.mouseLeaveOverlayChild,
    };

    return (
      <View {...containerProps}>
        <View {...childProps}>
          <Text>This is an overlay view</Text>
        </View>
      </View>
    );
  }

  mouseEnterPage = () => {
    this.setState({pageHover: true});
  };
  mouseLeavePage = () => {
    this.setState({pageHover: false});
  };

  mouseEnterContentContainer = () => {
    this.setState({contentHover: true});
  };
  mouseLeaveContentContainer = () => {
    this.setState({contentHover: false});
  };

  mouseEnterContentChild = () => {
    this.setState({contentChildHover: true});
  };
  mouseLeaveContentChild = () => {
    this.setState({contentChildHover: false});
  };

  mouseEnterOverlay = () => {
    this.setState({overlayHover: true});
  };
  mouseLeaveOverlay = () => {
    this.setState({overlayHover: false});
  };
  mouseEnterOverlayChild = () => {
    this.setState({overlayChildHover: true});
  };
  mouseLeaveOverlayChild = () => {
    this.setState({overlayChildHover: false});
  };

  click = () => {
    this.setState({clicked: this.state.clicked + 1});
  };
  press = (event: GestureResponderEvent) => {
    console.log('press');
    console.log(event);
  };
  pressIn = (event: GestureResponderEvent) => {
    console.log('pressin');
    console.log(event);
  };
  pressOut = (event: GestureResponderEvent) => {
    console.log('pressout');
    console.log(event);
  };
  back = () => {
    console.log('hardwareBackPress');
    return true;
  };
}
