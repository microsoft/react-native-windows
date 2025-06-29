'use strict';
import React from 'react'
import { StyleSheet, Text, TouchableHighlight } from 'react-native';
import { Cursor, IKeyboardEvent, IHandledKeyboardEvent, ViewWin32 } from '@office-iss/react-native-win32';

const styles = StyleSheet.create({
  border: {
    borderStyle: 'dotted',
    borderColor: 'black',
  },
  keyComponentRoot: {
    borderWidth: 2,
    flexDirection: 'row',
    marginVertical: 5,
    backgroundColor: 'whitesmoke',
    justifyContent: 'space-around',
  },
  keyEnterVisualizer: {
    margin: 5,
    alignItems: 'center',
    minWidth: 100,
    minHeight: 30,
  },
  blackbox: { height: 30, width: 30, borderColor: 'black', borderWidth: 3 },
});

interface IFocusableComponentState {
  hasFocus: boolean;
}

function FocusMoverTestComponent() {
  const [hasFocus, setHasFocus] = React.useState<boolean>(false);

  const labelId = React.useId();

  const ref = React.useRef<ViewWin32>(null);
  const onBtnPress = () => {
    if (ref.current) {
      ref.current.focus();
    }
  };

  return (
    <ViewWin32>
      <ViewWin32 nativeID={labelId} accessibilityLabel="separate label for test" accessibilityItemType="Comment" />
    <ViewWin32 style={{ flexDirection: 'row', alignItems: 'center', justifyContent: 'space-around', marginVertical: 5 }}>
      <TouchableHighlight onPress={onBtnPress}>
        <ViewWin32 accessibilityLabelledBy={labelId} style={styles.blackbox} />
      </TouchableHighlight>
      <ViewWin32
        ref={ref}
        focusable
        style={hasFocus ? { backgroundColor: '#aee8fcff' } : { backgroundColor: '#00000000' }}
        onFocus={() => setHasFocus(true)}
        onBlur={() => setHasFocus(false)}
        enableFocusRing={false}
      >
        <Text>{hasFocus ? 'Focus: Yes' : 'Focus: No'}</Text>
      </ViewWin32>
    </ViewWin32>
    </ViewWin32>
  );
}

interface IKeyboardableComponentState {
  lastKeyDown: string;
  lastKeyUp: string;
}

const handledNativeKeyboardEvents: IHandledKeyboardEvent[] = [
  { key: 'ArrowDown' },
  { key: 'ArrowUp' },
  { key: 'ArrowLeft' },
  { key: 'ArrowRight' },
  { key: 'Tab' },
];

class KeyboardTestComponent extends React.Component<{}, IFocusableComponentState & IKeyboardableComponentState> {
  public constructor(props) {
    super(props);
    this.state = {
      hasFocus: false,
      lastKeyDown: null,
      lastKeyUp: null,
    };
  }

  public render() {
    return (
      <ViewWin32 keyDownEvents={handledNativeKeyboardEvents} keyUpEvents={handledNativeKeyboardEvents}>
        <ViewWin32
          style={this.state.hasFocus ? [styles.keyComponentRoot, styles.border] : styles.keyComponentRoot}
          focusable
          onKeyUp={this._onKeyUp}
          onKeyDown={this._onKeyDown}
          onFocus={this._onFocus}
          onBlur={this._onBlur}
          enableFocusRing={false}
        >
          <ViewWin32 style={styles.keyEnterVisualizer}>
            <Text>OnKeyDown</Text>
            <Text>----</Text>
            <Text>{this.state.lastKeyDown !== null ? this.state.lastKeyDown : ' '}</Text>
          </ViewWin32>
          <ViewWin32 style={styles.keyEnterVisualizer}>
            <Text>OnKeyUp</Text>
            <Text>----</Text>
            <Text>{this.state.lastKeyUp !== null ? this.state.lastKeyUp : ' '}</Text>
          </ViewWin32>
        </ViewWin32>
      </ViewWin32>
    );
  }

  private readonly _onFocus = () => {
    this.setState({
      hasFocus: true,
    });
  };

  private readonly _onBlur = () => {
    this.setState({
      hasFocus: false,
    });
  };

  private readonly _onKeyUp = (ev: IKeyboardEvent) => {
    this.setState({ lastKeyUp: ev.nativeEvent.key, lastKeyDown: null });
  };

  private readonly _onKeyDown = (ev: IKeyboardEvent) => {
    this.setState({ lastKeyDown: ev.nativeEvent.key, lastKeyUp: null });
  };
}

interface IHoverComponentProps {
  color: string;
}
class HoverTestComponent extends React.Component<IHoverComponentProps, IFocusableComponentState> {
  public constructor(props) {
    super(props);
    this.state = {
      hasFocus: false,
    };
  }

  public render() {
    return (
      <ViewWin32
        onMouseEnter={this._onMouseEnter}
        onMouseLeave={this._onMouseLeave}
        style={this.state.hasFocus ? [styles.blackbox, { backgroundColor: this.props.color }] : styles.blackbox}
      />
    );
  }
  private readonly _onMouseLeave = () => {
    this.setState({ hasFocus: false });
  };
  private readonly _onMouseEnter = () => {
    this.setState({ hasFocus: true });
  };
}

class HoverExample extends React.Component {
  public render() {
    return (
      <ViewWin32 style={{ flexDirection: 'row', marginHorizontal: 75, justifyContent: 'space-around' }}>
        <HoverTestComponent color="red" />
        <HoverTestComponent color="orange" />
        <HoverTestComponent color="yellow" />
        <HoverTestComponent color="green" />
        <HoverTestComponent color="blue" />
        <HoverTestComponent color="indigo" />
        <HoverTestComponent color="violet" />
      </ViewWin32>
    );
  }
}

const ToolTipExample: React.FunctionComponent<{}> = () => {
  return (
    <ViewWin32
      style={{
        backgroundColor: 'red',
        height: 100,
        width: 100,
      }}
      tooltip="Example tooltip"
      cursor="pointer"
    />
  );
};

interface ICursorTestComponentProps {
  cursor: Cursor
}

const CursorTestComponent: React.FunctionComponent<ICursorTestComponentProps> = (props) => {
  return (
    <ViewWin32 style={{flexDirection: 'column'}}>
      <Text>{props.cursor}</Text>
      <ViewWin32 cursor={props.cursor} style={styles.blackbox} />
    </ViewWin32>
  )
}

const CursorExample: React.FunctionComponent = () => {
  return (
    <ViewWin32 style={{ flexDirection: 'row', justifyContent: 'space-around' }}>
      <CursorTestComponent cursor='auto' />
      <CursorTestComponent cursor='default' />
      <CursorTestComponent cursor='help' />
      <CursorTestComponent cursor='nesw-resize' />
      <CursorTestComponent cursor='not-allowed' />
      <CursorTestComponent cursor='ns-resize' />
      <CursorTestComponent cursor='nwse-resize' />
      <CursorTestComponent cursor='pointer' />
      <CursorTestComponent cursor='wait' />
      <CursorTestComponent cursor='move' />
      <CursorTestComponent cursor='text' />
      <CursorTestComponent cursor='we-resize' />
    </ViewWin32>
  );
}
class EnableFocusRingExample extends React.Component<{}, IFocusableComponentState> {
  public constructor(props) {
    super(props);
    this.state = {
      hasFocus: false,
    };
  }
  
  public render() {
    return (
      <ViewWin32 style={{ flexDirection: 'row', alignItems: 'center', justifyContent: 'space-around', marginVertical: 5 }}>
        <ViewWin32
          style={{
            backgroundColor: 'pink',
            height: 100,
            width: 100,
          }}
          enableFocusRing={true}
          focusable
        >
          <Text>enableFocusRing set to true</Text>
        </ViewWin32>
        <ViewWin32
          style={{
            backgroundColor: 'pink',
            height: 100,
            width: 100,
          }}
          enableFocusRing={false}
          focusable
          onFocus={this._onFocus}
          onBlur={this._onBlur}
        >
          <>
            <Text>enableFocusRing set to false</Text>
            <Text>{this.state.hasFocus ? 'Focus: Yes' : 'Focus: No'}</Text>
          </>
        </ViewWin32>
      </ViewWin32>
    );
  }

  private readonly _onFocus = () => {
    this.setState({
      hasFocus: true,
    });
  };

  private readonly _onBlur = () => {
    this.setState({
      hasFocus: false,
    });
  };
}


export const title = 'ViewWin32';
export const displayName = 'ViewWin32 Example';
export const description = 'All the stock View props plus Win32 specific ones';
export const examples = [
  {
    title: 'focus() method example',
    description: 'Each of these black boxes moves focus to the ViewWin32 on the right',
    render() {
      return (
        <ViewWin32>
          <FocusMoverTestComponent />
          <FocusMoverTestComponent />
          <FocusMoverTestComponent />
        </ViewWin32>
      );
    },
  },
  {
    title: 'KeyboardEvents example',
    description: 'Native keyboarding has been prevented',
    render() {
      return <KeyboardTestComponent />;
    },
  },
  {
    title: 'Hover example',
    description: 'Hover a rainbow',
    render() {
      return <HoverExample />;
    },
  },
  {
    title: 'Tooltip example',
    description: 'Displays a tooltip on hover',
    render() {
      return <ToolTipExample />;
    },
  },
  {
    title: 'Cursor example',
    description: 'Each of these boxes should display a different cursor',
    render() {
      return <CursorExample />;
    },
  },
  {
    title: 'EnableFocusRing example',
    description: 'Displays focus visuals that are driven by native',
    render() {
      return <EnableFocusRingExample />;
    },
  },
];
