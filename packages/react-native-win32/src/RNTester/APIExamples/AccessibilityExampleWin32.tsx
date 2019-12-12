'use strict';
import * as React from 'react';
import { IRNTesterPage } from 'src/RNTester/RNTester.types';
import { FlatList, StyleSheet, Text, TouchableHighlight, ListRenderItemInfo } from 'react-native';
import { ViewWin32 } from '../../Libraries/Components/View/ViewWin32';

const styles = StyleSheet.create({
  border: {
    borderStyle: 'dotted',
    borderColor: 'black'
  },
  box: {
    borderWidth: 2,
    flexDirection: 'row',
    marginVertical: 5,
    height: 20,
    backgroundColor: 'whitesmoke',
    justifyContent: 'space-around'
  },
  listContainer: {
    height: 150
  }
});

interface IFocusableComponentState {
  hasFocus: boolean;
}

interface IExpandCollapseState {
  expanded: boolean;
}

const AnnotationExample: React.FunctionComponent = props => {
  return (
    <ViewWin32
      accessible
      acceptsKeyboardFocus
      accessibilityAnnotation={{ typeID: 'Comment', author: 'Krystal Siler', dateTime: '7/19/2019 1:03 PM' }}
      accessibilityLabel="Test accessibility label"
      accessibilityHint="Test accessibility hint"
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
      expanded: false
    };
  }

  public render() {
    return (
      <ViewWin32>
        <TouchableHighlight onPress={this._onPress} underlayColor={'transparent'}>
          <ViewWin32
            accessible
            acceptsKeyboardFocus
            accessibilityLabel={'Test accessibility label'}
            accessibilityRole="button"
            accessibilityActions={[{ name: 'Expand' }, { name: 'Collapse' }]}
            accessibilityStates={this.state.expanded ? ['expanded'] : []}
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

  private _expand = () => {
    this.setState({
      expanded: true
    });
  };

  private _collapse = () => {
    this.setState({
      expanded: false
    });
  };

  private _onFocus = () => {
    this.setState({
      hasFocus: true
    });
  };

  private _onBlur = () => {
    this.setState({
      hasFocus: false
    });
  };

  private _onPress = () => {
    this.state.expanded ? this._collapse() : this._expand();
  };
}

interface IMultiSelectionExampleState {
  selectedItems: number[];
}

class MultiSelectionExample extends React.Component<{}, IMultiSelectionExampleState> {
  public constructor(props) {
    super(props);

    this.state = {
      selectedItems: []
    };
  }

  public handleAdd = item => {
    if (this.state.selectedItems.indexOf(item) === -1) {
      this.setState({
        selectedItems: this.state.selectedItems.concat([item])
      });
    }
  };

  public handleRemove = item => {
    if (this.state.selectedItems.indexOf(item) > -1) {
      const array = [...this.state.selectedItems];
      const index = array.indexOf(item);
      array.splice(index, 1);
      this.setState({
        selectedItems: array
      });
    }
  };

  public render() {
    return (
      <ViewWin32 accessible accessibilityRole="tablist" accessibilityStates={['multiselectable']}>
        <SelectionItemComponent
          value={1}
          color="#aee8fcff"
          level={1}
          position={1}
          size={3}
          addHandler={this.handleAdd}
          removeHandler={this.handleRemove}
          isSelected={this.state.selectedItems.indexOf(1) > -1 ? true : false}
        />
        <SelectionItemComponent
          value={2}
          color="#aee8fcff"
          level={1}
          position={2}
          size={3}
          addHandler={this.handleAdd}
          removeHandler={this.handleRemove}
          isSelected={this.state.selectedItems.indexOf(2) > -1 ? true : false}
        />
        <SelectionItemComponent
          value={3}
          color="#aee8fcff"
          level={1}
          position={3}
          size={3}
          addHandler={this.handleAdd}
          removeHandler={this.handleRemove}
          isSelected={this.state.selectedItems.indexOf(3) > -1 ? true : false}
        />
        <Text>Selected Items: {this.state.selectedItems.toString()}</Text>
      </ViewWin32>
    );
  }
}
interface ISelectionItemComponentProps {
  value: number;
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
    <TouchableHighlight onPress={this._onPress} underlayColor={'transparent'}>
      <ViewWin32
        accessible
        acceptsKeyboardFocus
        accessibilityLevel={props.level}
        accessibilityPositionInSet={props.position}
        accessibilitySetSize={props.size}
        accessibilityActions={[{ name: 'AddToSelection' }, { name: 'RemoveFromSelection' }]}
        accessibilityRole="tab"
        accessibilityStates={props.isSelected ? ['selected'] : []}
        style={props.isSelected ? [styles.box, { backgroundColor: props.color }] : styles.box}
        onAccessibilityAction={_onAccessibilityAction}
        onAccessibilityTap={_onPress}
      >
        <Text>Item {props.value.toString()}</Text>
      </ViewWin32>
    </TouchableHighlight>
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
      hasFocus: false
    };
  }

  public render() {
    return (
      <ViewWin32
        accessible
        acceptsKeyboardFocus
        accessibilityLabel={this.props.label}
        accessibilityRole="treeitem"
        accessibilityLevel={this.props.level}
        accessibilitySetSize={this.props.setSize}
        accessibilityPositionInSet={this.props.positionInSet}
        style={this.state.hasFocus ? [styles.box, styles.border] : styles.box}
        onFocus={this._onFocus}
        onBlur={this._onBlur}
      >
        <Text>{this.props.label}</Text>
      </ViewWin32>
    );
  }

  private _onFocus = () => {
    this.setState({
      hasFocus: true
    });
  };

  private _onBlur = () => {
    this.setState({
      hasFocus: false
    });
  };
}

const renderItem = (item: ListRenderItemInfo<IListProps>) => (
  <ListItem label={item.item.label} level={item.item.level} setSize={item.item.setSize} positionInSet={item.item.positionInSet} />
);

const getItemLayout = (data: Array<IListProps>, index: number) => ({ length: 30, offset: 30 * index, index });

const keyExtractor = (item: IListProps) => item.label.toString();

interface IFlatListProps {
  renderItem: (item: ListRenderItemInfo<IListProps>) => JSX.Element;
  getItemLayout?: (
    data: IListProps[],
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
    <ViewWin32 accessible acceptsKeyboardFocus accessibilityRole="tree">
      <FlatList data={generateList(4)} renderItem={props.renderItem} keyExtractor={props.keyExtractor} />
    </ViewWin32>
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
      positionInSet: i
    };
    list[i - 1] = item;
  }
  return list;
}

const AccessibilityTestPage: IRNTesterPage = {
  title: 'Accessibility Examples',
  displayName: 'Accessibility Examples',
  description: 'Demonstrates accessibility props',
  examples: [
    {
      title: 'Annotation Example',
      description: 'A comment that exposes annotation properties.',
      render: () => <AnnotationExample />
    },
    {
      title: 'Button Example',
      description: 'A button with some basic accessibility props and expand/collapse',
      render: () => <ButtonExample />
    },
    {
      title: 'MultiSelection Example',
      description: 'A list of items that can be selected',
      render: () => <MultiSelectionExample />
    },
    {
      title: 'FlatList Example',
      description: 'A flat list of headers with n of m support',
      render: () => <FlatListExample renderItem={renderItem} keyExtractor={keyExtractor} />
    },
    {
      title: 'Virtualized FlatList Example',
      description: 'A virtualized flat list of 30 items with n of m support',
      render: () => <VirtualizedFlatListExample renderItem={renderItem} getItemLayout={getItemLayout} keyExtractor={keyExtractor} />
    }
  ]
};

export = AccessibilityTestPage;
