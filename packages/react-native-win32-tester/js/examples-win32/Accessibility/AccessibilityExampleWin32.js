'use strict';
Object.defineProperty(exports, "__esModule", { value: true });
exports.examples = exports.description = exports.displayName = exports.title = void 0;
const React = require("react");
const react_native_1 = require("react-native");
const react_native_win32_1 = require("@office-iss/react-native-win32");
const styles = react_native_1.StyleSheet.create({
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
});
const AnnotationExample = () => {
    return (React.createElement(react_native_win32_1.ViewWin32, { accessible: true, accessibilityAnnotation: { typeID: 'Comment', author: 'Krystal Siler', dateTime: '7/19/2019 1:03 PM' }, accessibilityLabel: "Test accessibility label", accessibilityHint: "Test accessibility hint", focusable: true, style: styles.box },
        React.createElement(react_native_1.Text, null, "Comment.")));
};
class ButtonExample extends React.Component {
    constructor(props) {
        super(props);
        this._expand = () => {
            this.setState({
                expanded: true,
            });
        };
        this._collapse = () => {
            this.setState({
                expanded: false,
            });
        };
        this._onFocus = () => {
            this.setState({
                hasFocus: true,
            });
        };
        this._onBlur = () => {
            this.setState({
                hasFocus: false,
            });
        };
        this._onPress = () => {
            this.state.expanded ? this._collapse() : this._expand();
        };
        this.state = {
            hasFocus: false,
            expanded: false,
        };
    }
    render() {
        return (React.createElement(react_native_win32_1.ViewWin32, null,
            React.createElement(react_native_1.TouchableHighlight, { onPress: this._onPress, underlayColor: 'transparent' },
                React.createElement(react_native_win32_1.ViewWin32, { accessible: true, accessibilityLabel: 'Test accessibility label', accessibilityRole: "button", accessibilityActions: [{ name: 'Expand' }, { name: 'Collapse' }], accessibilityState: { expanded: this.state.expanded }, focusable: true, onAccessibilityAction: this._onAccessibilityAction, style: this.state.hasFocus ? [styles.box, styles.border] : styles.box, onFocus: this._onFocus, onBlur: this._onBlur, onAccessibilityTap: this._onPress },
                    React.createElement(react_native_1.Text, null, this.state.expanded ? 'Hide Text' : 'Show Text'))),
            this.state.expanded ? React.createElement(react_native_1.Text, null, "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor.") : null));
    }
    _onAccessibilityAction(event) {
        switch (event.nativeEvent.actionName) {
            case 'Expand':
                this._expand();
                break;
            case 'Collapse':
                this._collapse();
                break;
        }
    }
}
class MultiSelectionExample extends React.Component {
    constructor(props) {
        super(props);
        this.handleAdd = item => {
            if (!this.state.selectedItems.includes(item)) {
                this.setState({
                    selectedItems: this.state.selectedItems.concat([item]),
                });
            }
        };
        this.handleRemove = item => {
            if (this.state.selectedItems.includes(item)) {
                const array = [...this.state.selectedItems];
                const index = array.indexOf(item);
                array.splice(index, 1);
                this.setState({
                    selectedItems: array,
                });
            }
        };
        this.state = {
            selectedItems: [],
        };
    }
    render() {
        return (React.createElement(react_native_win32_1.ViewWin32, { accessible: true, accessibilityRole: "tablist", accessibilityState: { multiselectable: true } },
            React.createElement(SelectionItemComponent, { value: 1, color: "#aee8fcff", level: 1, position: 1, size: 3, addHandler: this.handleAdd, removeHandler: this.handleRemove, isSelected: this.state.selectedItems.includes(1) ? true : false }),
            React.createElement(SelectionItemComponent, { value: 2, color: "#aee8fcff", level: 1, position: 2, size: 3, addHandler: this.handleAdd, removeHandler: this.handleRemove, isSelected: this.state.selectedItems.includes(2) ? true : false }),
            React.createElement(SelectionItemComponent, { value: 3, color: "#aee8fcff", level: 1, position: 3, size: 3, addHandler: this.handleAdd, removeHandler: this.handleRemove, isSelected: this.state.selectedItems.includes(3) ? true : false }),
            React.createElement(react_native_1.Text, null,
                "Selected Items: ",
                this.state.selectedItems.toString())));
    }
}
const SelectionItemComponent = props => {
    return (React.createElement(react_native_1.TouchableHighlight, { onPress: _onPress, underlayColor: 'transparent' },
        React.createElement(react_native_win32_1.ViewWin32, { accessible: true, accessibilityLevel: props.level, accessibilityPositionInSet: props.position, accessibilitySetSize: props.size, accessibilityActions: [{ name: 'AddToSelection' }, { name: 'RemoveFromSelection' }], accessibilityRole: "tab", accessibilityState: { selected: props.isSelected }, focusable: true, style: props.isSelected ? [styles.box, { backgroundColor: props.color }] : styles.box, onAccessibilityAction: _onAccessibilityAction, onAccessibilityTap: _onPress },
            React.createElement(react_native_1.Text, null,
                "Item ",
                props.value.toString()))));
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
class ListItem extends React.PureComponent {
    constructor(props) {
        super(props);
        this._onFocus = () => {
            this.setState({
                hasFocus: true,
            });
        };
        this._onBlur = () => {
            this.setState({
                hasFocus: false,
            });
        };
        this.state = {
            hasFocus: false,
        };
    }
    render() {
        return (React.createElement(react_native_win32_1.ViewWin32, { accessible: true, accessibilityLabel: this.props.label, accessibilityRole: "treeitem", accessibilityLevel: this.props.level, accessibilitySetSize: this.props.setSize, accessibilityPositionInSet: this.props.positionInSet, focusable: true, style: this.state.hasFocus ? [styles.box, styles.border] : styles.box, onFocus: this._onFocus, onBlur: this._onBlur },
            React.createElement(react_native_1.Text, null, this.props.label)));
    }
}
const renderItem = (item) => (React.createElement(ListItem, { label: item.item.label, level: item.item.level, setSize: item.item.setSize, positionInSet: item.item.positionInSet }));
const getItemLayout = (data, index) => ({ length: 30, offset: 30 * index, index });
const keyExtractor = (item) => item.label.toString();
const FlatListExample = props => {
    return (React.createElement(react_native_win32_1.ViewWin32, { accessible: true, accessibilityRole: "tree", focusable: true },
        React.createElement(react_native_1.FlatList, { data: generateList(4), renderItem: props.renderItem, keyExtractor: props.keyExtractor })));
};
const VirtualizedFlatListExample = props => {
    return (React.createElement(react_native_win32_1.ViewWin32, { style: styles.listContainer, accessibilityRole: "tree" },
        React.createElement(react_native_1.FlatList, { data: generateList(30), renderItem: props.renderItem, getItemLayout: props.getItemLayout, initialNumToRender: 6, maxToRenderPerBatch: 5, updateCellsBatchingPeriod: 1000, keyExtractor: props.keyExtractor })));
};
function generateList(size) {
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
const AccessibilityInfoExample = () => {
    const onClick = React.useCallback(() => {
        react_native_1.AccessibilityInfo.announceForAccessibility('AccessibilityInfo announcement succeeded!');
    }, []);
    const ref = React.useRef();
    const onClickTag = React.useCallback(() => {
        // @ts-ignore (We changed the API for announceForAccessibility)
        react_native_1.AccessibilityInfo.announceForAccessibility('AccessibilityInfo announcement from tag', react_native_1.findNodeHandle(ref.current));
    }, [ref]);
    return (React.createElement(react_native_1.View, { style: styles.box },
        React.createElement(react_native_1.TouchableHighlight, { onPress: onClick },
            React.createElement(react_native_1.Text, null, "AccessibilityInfo.announceForAccessibility")),
        React.createElement(react_native_1.TouchableHighlight, { ref: ref, onPress: onClickTag },
            React.createElement(react_native_1.Text, null, "AccessibilityInfo.announceForAccessibility with target"))));
};
exports.title = 'Accessibility Examples';
exports.displayName = 'Accessibility Examples';
exports.description = 'Demonstrates accessibility props';
exports.examples = [
    {
        title: 'Annotation Example',
        description: 'A comment that exposes annotation properties.',
        render: () => React.createElement(AnnotationExample, null),
    },
    {
        title: 'Button Example',
        description: 'A button with some basic accessibility props and expand/collapse',
        render: () => React.createElement(ButtonExample, null),
    },
    {
        title: 'MultiSelection Example',
        description: 'A list of items that can be selected',
        render: () => React.createElement(MultiSelectionExample, null),
    },
    {
        title: 'FlatList Example',
        description: 'A flat list of headers with n of m support',
        render: () => React.createElement(FlatListExample, { renderItem: renderItem, keyExtractor: keyExtractor }),
    },
    {
        title: 'Virtualized FlatList Example',
        description: 'A virtualized flat list of 30 items with n of m support',
        render: () => React.createElement(VirtualizedFlatListExample, { renderItem: renderItem, getItemLayout: getItemLayout, keyExtractor: keyExtractor }),
    },
    {
        title: 'AccessibilityInfo Example',
        description: 'AccessibilityInfo Native Module announcement',
        render: () => React.createElement(AccessibilityInfoExample, null),
    },
];
//# sourceMappingURL=AccessibilityExampleWin32.js.map