'use strict';
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
exports.examples = exports.description = exports.displayName = exports.title = void 0;
const react_1 = __importDefault(require("react"));
const react_native_1 = require("react-native");
const ViewWin32_1 = require("../ViewWin32");
const styles = react_native_1.StyleSheet.create({
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
class FocusMoverTestComponent extends react_1.default.Component {
    constructor(props) {
        super(props);
        this._focusTarget = null;
        this._setRef = (ref) => {
            this._focusTarget = ref;
        };
        this._onPress = () => {
            if (this._focusTarget !== undefined) {
                this._focusTarget.focus();
            }
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
        this.state = {
            hasFocus: false,
        };
        this._labeledBy = react_1.default.createRef();
    }
    render() {
        return (react_1.default.createElement(ViewWin32_1.ViewWin32, null,
            react_1.default.createElement(ViewWin32_1.ViewWin32, { ref: this._labeledBy, accessibilityLabel: "separate label for test" }),
            react_1.default.createElement(ViewWin32_1.ViewWin32, { style: { flexDirection: 'row', alignItems: 'center', justifyContent: 'space-around', marginVertical: 5 } },
                react_1.default.createElement(react_native_1.TouchableHighlight, { onPress: this._onPress },
                    react_1.default.createElement(ViewWin32_1.ViewWin32, { accessibilityLabeledBy: this._labeledBy, style: styles.blackbox })),
                react_1.default.createElement(ViewWin32_1.ViewWin32, { ref: this._setRef, focusable: true, style: this.state.hasFocus ? { backgroundColor: '#aee8fcff' } : { backgroundColor: '#00000000' }, onFocus: this._onFocus, onBlur: this._onBlur },
                    react_1.default.createElement(react_native_1.Text, null, this.state.hasFocus ? 'Focus: Yes' : 'Focus: No')))));
    }
}
const handledNativeKeyboardEvents = [
    { key: 'ArrowDown' },
    { key: 'ArrowUp' },
    { key: 'ArrowLeft' },
    { key: 'ArrowRight' },
    { key: 'Tab' },
];
class KeyboardTestComponent extends react_1.default.Component {
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
        this._onKeyUp = (ev) => {
            this.setState({ lastKeyUp: ev.nativeEvent.key, lastKeyDown: null });
        };
        this._onKeyDown = (ev) => {
            this.setState({ lastKeyDown: ev.nativeEvent.key, lastKeyUp: null });
        };
        this.state = {
            hasFocus: false,
            lastKeyDown: null,
            lastKeyUp: null,
        };
    }
    render() {
        return (react_1.default.createElement(ViewWin32_1.ViewWin32, { keyDownEvents: handledNativeKeyboardEvents, keyUpEvents: handledNativeKeyboardEvents },
            react_1.default.createElement(ViewWin32_1.ViewWin32, { style: this.state.hasFocus ? [styles.keyComponentRoot, styles.border] : styles.keyComponentRoot, focusable: true, onKeyUp: this._onKeyUp, onKeyDown: this._onKeyDown, onFocus: this._onFocus, onBlur: this._onBlur },
                react_1.default.createElement(ViewWin32_1.ViewWin32, { style: styles.keyEnterVisualizer },
                    react_1.default.createElement(react_native_1.Text, null, "OnKeyDown"),
                    react_1.default.createElement(react_native_1.Text, null, "----"),
                    react_1.default.createElement(react_native_1.Text, null, this.state.lastKeyDown !== null ? this.state.lastKeyDown : ' ')),
                react_1.default.createElement(ViewWin32_1.ViewWin32, { style: styles.keyEnterVisualizer },
                    react_1.default.createElement(react_native_1.Text, null, "OnKeyUp"),
                    react_1.default.createElement(react_native_1.Text, null, "----"),
                    react_1.default.createElement(react_native_1.Text, null, this.state.lastKeyUp !== null ? this.state.lastKeyUp : ' ')))));
    }
}
class HoverTestComponent extends react_1.default.Component {
    constructor(props) {
        super(props);
        this._onMouseLeave = () => {
            this.setState({ hasFocus: false });
        };
        this._onMouseEnter = () => {
            this.setState({ hasFocus: true });
        };
        this.state = {
            hasFocus: false,
        };
    }
    render() {
        return (react_1.default.createElement(ViewWin32_1.ViewWin32, { onMouseEnter: this._onMouseEnter, onMouseLeave: this._onMouseLeave, style: this.state.hasFocus ? [styles.blackbox, { backgroundColor: this.props.color }] : styles.blackbox }));
    }
}
class HoverExample extends react_1.default.Component {
    render() {
        return (react_1.default.createElement(ViewWin32_1.ViewWin32, { style: { flexDirection: 'row', marginHorizontal: 75, justifyContent: 'space-around' } },
            react_1.default.createElement(HoverTestComponent, { color: "red" }),
            react_1.default.createElement(HoverTestComponent, { color: "orange" }),
            react_1.default.createElement(HoverTestComponent, { color: "yellow" }),
            react_1.default.createElement(HoverTestComponent, { color: "green" }),
            react_1.default.createElement(HoverTestComponent, { color: "blue" }),
            react_1.default.createElement(HoverTestComponent, { color: "indigo" }),
            react_1.default.createElement(HoverTestComponent, { color: "violet" })));
    }
}
const ToolTipExample = () => {
    return (react_1.default.createElement(ViewWin32_1.ViewWin32, { style: {
            backgroundColor: 'red',
            height: 100,
            width: 100,
        }, tooltip: "Example tooltip", cursor: "pointer" }));
};
const CursorTestComponent = (props) => {
    return (react_1.default.createElement(ViewWin32_1.ViewWin32, { style: { flexDirection: 'column' } },
        react_1.default.createElement(react_native_1.Text, null, props.cursor),
        react_1.default.createElement(ViewWin32_1.ViewWin32, { cursor: props.cursor, style: styles.blackbox })));
};
const CursorExample = () => {
    return (react_1.default.createElement(ViewWin32_1.ViewWin32, { style: { flexDirection: 'row', justifyContent: 'space-around' } },
        react_1.default.createElement(CursorTestComponent, { cursor: 'auto' }),
        react_1.default.createElement(CursorTestComponent, { cursor: 'default' }),
        react_1.default.createElement(CursorTestComponent, { cursor: 'help' }),
        react_1.default.createElement(CursorTestComponent, { cursor: 'nesw-resize' }),
        react_1.default.createElement(CursorTestComponent, { cursor: 'not-allowed' }),
        react_1.default.createElement(CursorTestComponent, { cursor: 'ns-resize' }),
        react_1.default.createElement(CursorTestComponent, { cursor: 'nwse-resize' }),
        react_1.default.createElement(CursorTestComponent, { cursor: 'pointer' }),
        react_1.default.createElement(CursorTestComponent, { cursor: 'wait' }),
        react_1.default.createElement(CursorTestComponent, { cursor: 'move' }),
        react_1.default.createElement(CursorTestComponent, { cursor: 'text' }),
        react_1.default.createElement(CursorTestComponent, { cursor: 'we-resize' })));
};
exports.title = 'ViewWin32';
exports.displayName = 'ViewWin32 Example';
exports.description = 'All the stock View props plus Win32 specific ones';
exports.examples = [
    {
        title: 'focus() method example',
        description: 'Each of these black boxes moves focus to the ViewWin32 on the right',
        render() {
            return (react_1.default.createElement(ViewWin32_1.ViewWin32, null,
                react_1.default.createElement(FocusMoverTestComponent, null),
                react_1.default.createElement(FocusMoverTestComponent, null),
                react_1.default.createElement(FocusMoverTestComponent, null)));
        },
    },
    {
        title: 'KeyboardEvents example',
        description: 'Native keyboarding has been prevented',
        render() {
            return react_1.default.createElement(KeyboardTestComponent, null);
        },
    },
    {
        title: 'Hover example',
        description: 'Hover a rainbow',
        render() {
            return react_1.default.createElement(HoverExample, null);
        },
    },
    {
        title: 'Tooltip example',
        description: 'Displays a tooltip on hover',
        render() {
            return react_1.default.createElement(ToolTipExample, null);
        },
    },
    {
        title: 'Cursor example',
        description: 'Each of these boxes should display a different cursor',
        render() {
            return react_1.default.createElement(CursorExample, null);
        },
    },
];
//# sourceMappingURL=ViewWin32Test.js.map