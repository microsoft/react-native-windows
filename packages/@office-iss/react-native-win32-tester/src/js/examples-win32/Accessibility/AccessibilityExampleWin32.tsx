'use strict';
import React from 'react'
import {
  View,
  AccessibilityInfo,
  FlatList,
  StyleSheet,
  Text,
  TouchableHighlight,
  ListRenderItemInfo,
} from 'react-native';
import { ViewWin32 } from '@office-iss/react-native-win32';
import { IHandledKeyboardEvent, Pressable } from '@office-iss/react-native-win32';

const styles = StyleSheet.create({
  border: {
    borderStyle: 'dotted',
    borderColor: 'black',
  },
  box: {
    borderWidth: 2,
    flexDirection: 'row',
    marginVertical: 5,
    height: 20,
    backgroundColor: 'whitesmoke',
    justifyContent: 'space-around',
  },
  listContainer: {
    height: 150,
  },
  heading: {
    fontSize: 16,
    fontWeight: '600'
  },
});

interface IFocusableComponentState {
  hasFocus: boolean;
}

interface IExpandCollapseState {
  expanded: boolean;
}

const AnnotationExample: React.FunctionComponent = () => {
  return (
    <ViewWin32
      accessible
      accessibilityAnnotation={{ typeID: 'Comment', author: 'Krystal Siler', dateTime: '7/19/2019 1:03 PM' }}
      accessibilityLabel="Test accessibility label"
      accessibilityHint="Test accessibility hint"
      accessibilityPositionInSet={1}
      focusable
      style={styles.box}
    >
      <Text>Comment.</Text>
    </ViewWin32>
  );
};

class ButtonExample extends React.Component<{}, IFocusableComponentState & IExpandCollapseState> {
  public constructor(props) {
    super(props);
    this.state = {
      hasFocus: false,
      expanded: false,
    };
  }

  public render() {
    return (
      <ViewWin32>
        <TouchableHighlight onPress={this._onPress} underlayColor={'transparent'}>
          <ViewWin32
            accessible
            accessibilityLabel={'Test accessibility label'}
            accessibilityRole="button"
            accessibilityActions={[{ name: 'Expand' }, { name: 'Collapse' }]}
            accessibilityState={{expanded: this.state.expanded}}
            focusable
            onAccessibilityAction={this._onAccessibilityAction}
            style={this.state.hasFocus ? [styles.box, styles.border] : styles.box}
            onFocus={this._onFocus}
            onBlur={this._onBlur}
            onAccessibilityTap={this._onPress}
          >
            <Text>{this.state.expanded ? 'Hide Text' : 'Show Text'}</Text>
          </ViewWin32>
        </TouchableHighlight>
        {this.state.expanded ? <Text>Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor.</Text> : null}
      </ViewWin32>
    );
  }

  private _onAccessibilityAction(event) {
    switch (event.nativeEvent.actionName) {
      case 'Expand':
        this._expand();
        break;
      case 'Collapse':
        this._collapse();
        break;
    }
  }

  private readonly _expand = () => {
    this.setState({
      expanded: true,
    });
  };

  private readonly _collapse = () => {
    this.setState({
      expanded: false,
    });
  };

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

  private readonly _onPress = () => {
    this.state.expanded ? this._collapse() : this._expand();
  };
}

const HeadingLevelExample: React.FunctionComponent = () => {
  return (
    <ViewWin32>
      <Text accessible accessibilityRole="header" accessibilityLevel={1} style={styles.heading}>Paragraph Title</Text>
      <Text>The above heading level should be heading level 1.</Text>
      <Text accessible accessibilityRole="header" style={styles.heading}>Second Paragraph Title</Text>
      <Text>The above heading has no level set. It should default to heading level 2.</Text>
      <Text accessible accessibilityLevel={1} style={styles.heading}>Third Paragraph Title</Text>
      <Text>The above heading does not use the "header" role but has a level set. Since the "header" role 
        is not set, the heading level should be set to none and it will not be read as a header.
      </Text>
    </ViewWin32>
  );
};

interface IMultiSelectionExampleState {
  selectedItems: number[];
}

class MultiSelectionExample extends React.Component<{}, IMultiSelectionExampleState> {
  public constructor(props) {
    super(props);

    this.state = {
      selectedItems: [],
    };
  }

  public handleAdd = item => {
    if (!this.state.selectedItems.includes(item)) {
      this.setState({
        selectedItems: this.state.selectedItems.concat([item]),
      });
    }
  };

  public handleRemove = item => {
    if (this.state.selectedItems.includes(item)) {
      const array = [...this.state.selectedItems];
      const index = array.indexOf(item);
      array.splice(index, 1);
      this.setState({
        selectedItems: array,
      });
    }
  };

  public render() {
    return (
      <ViewWin32 accessible accessibilityRole="tablist" aria-required aria-multiselectable>
        <SelectionItemComponent
          value={1}
          color="#aee8fcff"
          level={1}
          position={1}
          size={3}
          addHandler={this.handleAdd}
          removeHandler={this.handleRemove}
          isSelected={this.state.selectedItems.includes(1) ? true : false}
        />
        <SelectionItemComponent
          value={2}
          color="#aee8fcff"
          level={1}
          position={2}
          size={3}
          addHandler={this.handleAdd}
          removeHandler={this.handleRemove}
          isSelected={this.state.selectedItems.includes(2) ? true : false}
        />
        <SelectionItemComponent
          value={3}
          color="#aee8fcff"
          level={1}
          position={3}
          size={3}
          addHandler={this.handleAdd}
          removeHandler={this.handleRemove}
          isSelected={this.state.selectedItems.includes(3) ? true : false}
        />
        <Text>Selected Items: {this.state.selectedItems.toString()}</Text>
      </ViewWin32>
    );
  }
}
interface ISelectionItemComponentProps {
  value: any;
  color: string;
  level?: number;
  position?: number;
  size?: number;
  addHandler?: (item: number) => void;
  removeHandler?: (item: number) => void;
  isSelected: boolean;
}

const SelectionItemComponent: React.FunctionComponent<ISelectionItemComponentProps> = props => {
  return (
    <Pressable focusable={false} onPress={_onPress}>
      <ViewWin32
        accessible
        accessibilityLevel={props.level}
        accessibilityPositionInSet={props.position}
        accessibilitySetSize={props.size}
        accessibilityActions={[{ name: 'AddToSelection' }, { name: 'RemoveFromSelection' }]}
        accessibilityRole="tab"
        accessibilityState={{selected: props.isSelected}}
        focusable
        style={props.isSelected ? [styles.box, { backgroundColor: props.color }] : styles.box}
        onAccessibilityAction={_onAccessibilityAction}
        onAccessibilityTap={_onPress}
      >
        <Text>{props.value.toString()}</Text>
      </ViewWin32>
    </Pressable>
  );

  function _onAccessibilityAction(event) {
    switch (event.nativeEvent.actionName) {
      case 'AddToSelection':
        addToSelection();
        break;
      case 'RemoveFromSelection':
        removeFromSelection();
        break;
    }
  }

  function _onPress() {
    props.isSelected ? removeFromSelection() : addToSelection();
  }

  function addToSelection() {
    props.addHandler(props.value);
  }

  function removeFromSelection() {
    props.removeHandler(props.value);
  }
};

interface IListProps {
  label: string;
  level: number;
  setSize: number;
  positionInSet: number;
}

class ListItem extends React.PureComponent<IListProps, IFocusableComponentState> {
  public constructor(props) {
    super(props);
    this.state = {
      hasFocus: false,
    };
  }

  public render() {
    return (
      <ViewWin32
        accessible
        accessibilityLabel={this.props.label}
        accessibilityRole="treeitem"
        accessibilityLevel={this.props.level}
        accessibilitySetSize={this.props.setSize}
        accessibilityPositionInSet={this.props.positionInSet}
        focusable
        style={this.state.hasFocus ? [styles.box, styles.border] : styles.box}
        onFocus={this._onFocus}
        onBlur={this._onBlur}
      >
        <Text>{this.props.label}</Text>
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

const renderItem = (item: ListRenderItemInfo<IListProps>) => (
  <ListItem label={item.item.label} level={item.item.level} setSize={item.item.setSize} positionInSet={item.item.positionInSet} />
);

const getItemLayout = (data: ArrayLike<IListProps>, index: number) => ({ length: 30, offset: 30 * index, index });

const keyExtractor = (item: IListProps) => item.label.toString();

interface IFlatListProps {
  renderItem: (item: ListRenderItemInfo<IListProps>) => React.JSX.Element;
  getItemLayout?: (
    data: ArrayLike<IListProps>,
    index: number
  ) => {
    length: number;
    offset: number;
    index: number;
  };
  keyExtractor?: (item: IListProps) => string;
}

const FlatListExample: React.FunctionComponent<IFlatListProps> = props => {
  return (
    <ViewWin32 accessible accessibilityRole="tree" focusable>
      <FlatList data={generateList(4)} renderItem={props.renderItem} keyExtractor={props.keyExtractor} />
    </ViewWin32>
  );
};

const handledNativeKeyboardEvents: IHandledKeyboardEvent[] = [
  { key: 'ArrowDown' },
  { key: 'ArrowUp' },
];

const SingleSelectionItemComponent: React.FunctionComponent<ISelectionItemComponentProps> = props => {
  const [hasFocus, setHasFocus] = React.useState(false);
  return (
      <ViewWin32
        accessible
        accessibilityLevel={props.level}
        accessibilityPositionInSet={props.position}
        accessibilitySetSize={props.size}
        accessibilityActions={[{ name: 'Select' }]}
        accessibilityRole="listitem"
        accessibilityState={{selected: props.isSelected}}
        onFocus={() => {setHasFocus(true)}}
        onBlur={() => {setHasFocus(false)}}
        focusable
        style={[styles.box, 
          (props.isSelected) ? {backgroundColor: props.color} : {},
          (hasFocus) ? styles.border : {}]}
      >
        <Text>{props.value.toString()}</Text>
      </ViewWin32>
  );

};

const AccessibilityControlsExample: React.FunctionComponent = _props => {
  const listLength = 3;
  const listNativeId = React.useId();
  const [selectedIndex, setSelectedIndex] = React.useState(0);
  const [keyTargetHasFocus, setKeyTargetHasFocus] = React.useState(false);
  return (
    <View>      
      <ViewWin32 accessible accessibilityRole="list" nativeID={listNativeId}>
      <SingleSelectionItemComponent
            value={"Label A"}
            color="#aee8fcff"
            level={1}
            position={1}
            size={3}
            isSelected={selectedIndex === 0 ? true : false}
          />
      <SingleSelectionItemComponent
          value={"Label B"}
          color="#aee8fcff"
          level={1}
          position={2}
          size={3}
          isSelected={selectedIndex === 1 ? true : false}
        />
      <SingleSelectionItemComponent
        value={"Label C"}
        color="#aee8fcff"
        level={1}
        position={3}
        size={3}
        isSelected={selectedIndex === 2 ? true : false}
      />
      </ViewWin32>
      
      <View style={{alignItems:'center'}}><Text>With focus on the FOCUS TARGET View below, press Arrow Key Up ⬆️ or Arrow Key Down ⬇️ to move selection</Text>
      <ViewWin32
        accessible
        focusable
        accessibilityActions={[{ name: 'Select' }]}
        accessibilityControls={listNativeId}
        style={[styles.box, {width:'50%'}, keyTargetHasFocus ? styles.border : {}]}
        keyDownEvents={handledNativeKeyboardEvents}
        onFocus={() => {setKeyTargetHasFocus(true)}}
        onBlur={() => {setKeyTargetHasFocus(false)}}
        onKeyDown={(event) => {
        if (event.nativeEvent.key === 'ArrowUp' || event.nativeEvent.key === 'ArrowDown')
        {
          const indexChange = (event.nativeEvent.key === 'ArrowUp') ? -1 : 1;
          
          setSelectedIndex(
              (
                ((selectedIndex === 0) ? listLength : selectedIndex) + indexChange)
              % listLength);
        }
      }}>
        <Text>FOCUS TARGET</Text>
      </ViewWin32>
    </View>
    </View>
  );
};

const VirtualizedFlatListExample: React.FunctionComponent<IFlatListProps> = props => {
  return (
    <ViewWin32 style={styles.listContainer} accessibilityRole="tree">
      <FlatList
        data={generateList(30)}
        renderItem={props.renderItem}
        getItemLayout={props.getItemLayout}
        initialNumToRender={6}
        maxToRenderPerBatch={5}
        updateCellsBatchingPeriod={1000}
        keyExtractor={props.keyExtractor}
      />
    </ViewWin32>
  );
};

function generateList(size: number): Array<IListProps> {
  const list = new Array(size);
  for (let i = 1; i <= size; i++) {
    const item = {
      label: i.toString(),
      level: 1,
      setSize: size,
      positionInSet: i,
    };
    list[i - 1] = item;
  }
  return list;
}

const AccessibilityInfoExample: React.FunctionComponent<{}> =() => {
  const onClick = React.useCallback(() => {
    AccessibilityInfo.announceForAccessibility('AccessibilityInfo announcement succeeded!');
  }, []);

  const onClickDelayed = React.useCallback(() => {
    setTimeout(() => {
      AccessibilityInfo.announceForAccessibilityWithOptions(
        'AccessibilityInfo announcement succeeded!',
        { nativeID: 'AnnouncementTarget' });
      }, 3000);
  }, []);

  return (
    <View>
      <TouchableHighlight onPress={onClick} underlayColor={'transparent'}>
        <ViewWin32 style={styles.box} accessible focusable>
            <Text>AccessibilityInfo.announceForAccessibility</Text>
        </ViewWin32>
      </TouchableHighlight>

      <TouchableHighlight onPress={onClickDelayed} underlayColor={'transparent'}>
        <ViewWin32 style={styles.box} accessible focusable nativeID={'AnnouncementTarget'}>
            <Text>AccessibilityInfo.announceForAccessibilityWithOptions</Text>
        </ViewWin32>
      </TouchableHighlight>
    </View>
  );
};

export const title = 'Accessibility Examples';
export const displayName = 'Accessibility Examples';
export const description = 'Demonstrates accessibility props';
export const examples = [
    {
      title: 'Annotation Example',
      description: 'A comment that exposes annotation properties.',
      render: () => <AnnotationExample />,
    },
    {
      title: 'Button Example',
      description: 'A button with some basic accessibility props and expand/collapse',
      render: () => <ButtonExample />,
    },
    {
      title: 'Heading Level Example',
      description: 'A few text headings with heading level set',
      render: () => <HeadingLevelExample />,
    },
    {
      title: 'MultiSelection Example',
      description: 'A list of items that can be selected',
      render: () => <MultiSelectionExample />,
    },
    {
      title: 'FlatList Example',
      description: 'A flat list of headers with n of m support',
      render: () => <FlatListExample renderItem={renderItem} keyExtractor={keyExtractor} />,
    },
    {
      title: 'AccessibilityControls Example',
      description: 'A flat list of headers with n of m support with accessibilityControls selection',
      render: () => <AccessibilityControlsExample />,
    },
    {
      title: 'Virtualized FlatList Example',
      description: 'A virtualized flat list of 30 items with n of m support',
      render: () => <VirtualizedFlatListExample renderItem={renderItem} getItemLayout={getItemLayout} keyExtractor={keyExtractor} />,
    },
    {
      title: 'AccessibilityInfo Example',
      description: 'AccessibilityInfo Native Module announcement',
      render: () => <AccessibilityInfoExample />,
    },
  ];
