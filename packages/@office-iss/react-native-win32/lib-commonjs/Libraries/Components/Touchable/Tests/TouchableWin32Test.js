'use strict';
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
exports.examples = exports.description = exports.title = exports.displayName = void 0;
const react_1 = __importDefault(require("react"));
const react_native_1 = require("react-native");
const TextWin32_1 = require("../../Text/TextWin32");
const ViewWin32_1 = require("../../View/ViewWin32");
const TouchableWin32_1 = require("../TouchableWin32");
/**
 * Styles used across both examples
 */
const styles = react_native_1.StyleSheet.create({
    largeContainer: {
        height: 200,
        width: 600,
        justifyContent: 'space-between',
        alignItems: 'center',
        flexDirection: 'row',
    },
    smallContainer: {
        height: 90,
        width: 90,
        justifyContent: 'center',
        alignContent: 'center',
    },
    highlight: {
        height: 150,
        width: 150,
        justifyContent: 'space-around',
        alignContent: 'center',
        alignItems: 'center',
        flexDirection: 'row',
    },
    outerTouch: {
        height: 150,
        width: 150,
        justifyContent: 'space-around',
        alignContent: 'center',
        alignItems: 'center',
        flexDirection: 'row',
    },
    innerTouch: {
        height: 90,
        width: 90,
        alignItems: 'center',
        justifyContent: 'center',
    },
});
/**
 * Constant press rect offset, used to demonstrate press geometry
 */
const PRESS_RETENTION_OFFSET = {
    top: 100,
    left: 100,
    right: 100,
    bottom: 100,
};
/**
 * Example implementation of TouchableWithoutFeedback - not meant for use outside these examples
 * This is mainly there to show the ability to respond to touches without providing visual feedback,
 * though we do show a red border on focus to know where starts start and end and whether press rect
 * offsets are respected.
 */
class TouchableWin32WithoutFeedback extends react_1.default.Component {
    constructor(props) {
        super(props);
        this._touchableHandlePress = (e) => {
            this.props.onPress && this.props.onPress(e);
        };
        this._touchableHandleActivePressIn = (e) => {
            this.props.onPressIn && this.props.onPressIn(e);
        };
        this._touchableHandleActivePressOut = (e) => {
            this.props.onPressOut && this.props.onPressOut(e);
        };
        this._touchableHandleLongPress = (e) => {
            this.props.onLongPress && this.props.onLongPress(e);
        };
        this._touchableGetPressRectOffset = () => {
            return this.props.pressRetentionOffset || PRESS_RETENTION_OFFSET;
        };
        this._touchableGetHitSlop = () => {
            return this.props.hitSlop;
        };
        this._touchableGetHighlightDelayMS = () => {
            return this.props.delayPressIn || 0;
        };
        this._touchableGetLongPressDelayMS = () => {
            return this.props.delayLongPress === 0 ? 0 : this.props.delayLongPress || 500;
        };
        this._touchableGetPressOutDelayMS = () => {
            return this.props.delayPressOut || 0;
        };
        this._onFocus = () => {
            this.setState({ isFocused: true });
        };
        this._onBlur = () => {
            this.setState({ isFocused: false });
        };
        /**
         * The generated style uses hard-coded border width values
         */
        this._generateStyle = (state) => {
            const finalStyle = {
                borderWidth: state.isFocused ? 5 : 0,
                borderColor: 'red',
            };
            return Object.assign({}, this.props.style, finalStyle);
        };
        this.state = { isFocused: false };
    }
    render() {
        return (react_1.default.createElement(TouchableWin32_1.TouchableWin32, { focusable: true, rejectResponderTermination: true, disabled: false, touchableHandleActivePressIn: this._touchableHandleActivePressIn, touchableHandleActivePressOut: this._touchableHandleActivePressOut, touchableHandlePress: this._touchableHandlePress, touchableHandleLongPress: this._touchableHandleLongPress, touchableGetHighlightDelayMS: this._touchableGetHighlightDelayMS, touchableGetPressRectOffset: this._touchableGetPressRectOffset, touchableGetLongPressDelayMS: this._touchableGetLongPressDelayMS, touchableGetPressOutDelayMS: this._touchableGetPressOutDelayMS, touchableGetHitSlop: this._touchableGetHitSlop, onFocus: this._onFocus, onBlur: this._onBlur, renderStyle: this._generateStyle }, react_1.default.Children.only(this.props.children)));
    }
}
/**
 * Example implementation of TouchableHighlight - not meant for use outside these examples
 * The main difference between a ful TouchableHiglight implementation and this example is that
 * TouchableHighlight should manipulate the opacity of the wrapped view to display the underlay color.
 * This example merely uses hard coded color values to distinguish between different control states
 */
class TouchableWin32HighlightComponent extends react_1.default.Component {
    constructor() {
        super(...arguments);
        this._touchableHandlePress = (e) => {
            this.props.onPress && this.props.onPress(e);
            this.setState({ isPressed: false });
        };
        this._touchableHandleActivePressIn = (e) => {
            this.props.onPressIn && this.props.onPressIn(e);
            this.setState({ isPressed: true });
        };
        this._touchableHandleActivePressOut = (e) => {
            this.props.onPressOut && this.props.onPressOut(e);
            this.setState({ isPressed: false });
        };
        this._touchableHandleLongPress = (e) => {
            this.props.onLongPress && this.props.onLongPress(e);
        };
        this._touchableGetPressRectOffset = () => {
            return this.props.pressRetentionOffset || PRESS_RETENTION_OFFSET;
        };
        this._touchableGetHitSlop = () => {
            return { left: 100, right: 100, top: 100, bottom: 100 };
        };
        this._touchableGetHighlightDelayMS = () => {
            return this.props.delayLongPressIn || 0;
        };
        this._touchableGetLongPressDelayMS = () => {
            return this.props.delayLongPress || 500;
        };
        this._touchableGetPressOutDelayMS = () => {
            return this.props.delayPressOut || 0;
        };
        this._touchableHandleKeyPress = (ev) => {
            this.props.onKeyPress && this.props.onKeyPress(ev);
        };
        this._touchableHandleKeyPressDown = (ev) => {
            this.props.onKeyPressDown && this.props.onKeyPressDown(ev);
        };
        this._mouseEnter = () => {
            this.props.onMouseEnter && this.props.onMouseEnter();
            this.setState({ isMouseIn: true });
        };
        this._mouseLeave = () => {
            this.props.onMouseLeave && this.props.onMouseLeave();
            this.setState({ isMouseIn: false });
        };
        this._onFocus = (ev) => {
            this.props.onFocus && this.props.onFocus(ev);
            this.setState({ isFocused: true });
        };
        this._onBlur = (ev) => {
            this.props.onBlur && this.props.onBlur(ev);
            this.setState({ isFocused: false });
        };
        /**
         * The generated style uses hard-coded color values,
         * though it is trivial to pass these values as props.
         * This is an example of a render style (one that is a
         * funtion passed on to TouchableWin32 and resolved according
         * to the internal state of TouchableWin32).
         */
        this._generateStyle = (state) => {
            const finalStyle = {};
            finalStyle.borderColor = state.isFocused ? 'red' : 'blue';
            finalStyle.borderWidth = state.isFocused ? 10 : 5;
            if (state.isHovered) {
                if (state.isPressed) {
                    finalStyle.backgroundColor = 'black';
                }
                else {
                    finalStyle.backgroundColor = 'gray';
                }
            }
            else {
                if (state.isPressed) {
                    finalStyle.backgroundColor = 'green';
                }
                else {
                    finalStyle.backgroundColor = 'white';
                }
            }
            return Object.assign({}, this.props.style, finalStyle);
        };
    }
    render() {
        return (react_1.default.createElement(TouchableWin32_1.TouchableWin32, { focusable: true, rejectResponderTermination: this.props.rejectResponderTermination, disabled: false, touchableHandleActivePressIn: this._touchableHandleActivePressIn, touchableHandleActivePressOut: this._touchableHandleActivePressOut, touchableHandlePress: this._touchableHandlePress, touchableHandleLongPress: this._touchableHandleLongPress, touchableGetHighlightDelayMS: this._touchableGetHighlightDelayMS, touchableGetPressRectOffset: this._touchableGetPressRectOffset, touchableGetLongPressDelayMS: this._touchableGetLongPressDelayMS, touchableGetPressOutDelayMS: this._touchableGetPressOutDelayMS, touchableGetHitSlop: this._touchableGetHitSlop, touchableHandleKeyPress: this._touchableHandleKeyPress, touchableHandleKeyPressDown: this._touchableHandleKeyPressDown, onFocus: this._onFocus, onBlur: this._onBlur, onMouseEnter: this._mouseEnter, onMouseLeave: this._mouseLeave, renderStyle: this._generateStyle, children: this.props.children, ref: this.props.innerRef }));
    }
}
// Demonstrating ref forwarding - forwarding a ref using an innerRef prop on a class component
const TouchableWin32Highlight = react_1.default.forwardRef((props, ref) => {
    return (react_1.default.createElement(TouchableWin32HighlightComponent, Object.assign({ innerRef: ref }, props)));
});
/**
 * TouchableHighlightExamples
 */
class TouchableWithoutFeedbackExample extends react_1.default.Component {
    constructor(props) {
        super(props);
        this._onPress = () => {
            this.setState({ numberOfPresses: this.state.numberOfPresses + 1 });
        };
        this.state = { numberOfPresses: 0 };
    }
    render() {
        return (react_1.default.createElement(ViewWin32_1.ViewWin32, { style: styles.largeContainer },
            react_1.default.createElement(TouchableWin32WithoutFeedback, { style: styles.outerTouch, onPress: this._onPress },
                react_1.default.createElement(ViewWin32_1.ViewWin32, null,
                    react_1.default.createElement(TextWin32_1.TextWin32, null, "Press me"))),
            react_1.default.createElement(TouchableWin32WithoutFeedback, { style: styles.outerTouch, onPress: this._onPress },
                react_1.default.createElement(ViewWin32_1.ViewWin32, { style: styles.smallContainer },
                    react_1.default.createElement(TouchableWin32WithoutFeedback, { style: styles.innerTouch, onPress: this._onPress },
                        react_1.default.createElement(ViewWin32_1.ViewWin32, null,
                            react_1.default.createElement(TextWin32_1.TextWin32, null, "You can also have nested Touchables"))))),
            react_1.default.createElement(TextWin32_1.TextWin32, null, 'Number of Recognized Presses: ' + this.state.numberOfPresses)));
    }
}
/**
 * TouchableHighlightExamples
 */
class TouchableHighlightExample extends react_1.default.Component {
    constructor(props) {
        super(props);
        /**
         * This is primarily to demonstrate render children as a function
         * of state, here we change text color depending on interaction state.
         */
        this._getChildrenOfInnerTouchable = (state) => {
            return (react_1.default.createElement(ViewWin32_1.ViewWin32, { style: {
                    borderColor: state.isFocused ? 'green' : 'pink',
                    borderWidth: state.isHovered ? 10 : 5,
                    height: 70,
                    width: 70,
                } },
                react_1.default.createElement(TextWin32_1.TextWin32, { style: {
                        color: this._pickColor(state.isHovered, state.isPressed),
                        fontSize: state.isFocused ? 8 : 10,
                    }, textStyle: "None" }, "Press Me!")));
        };
        this._onPress = () => {
            this.setState({ numberOfPresses: this.state.numberOfPresses + 1 });
        };
        this.state = { numberOfPresses: 0 };
    }
    render() {
        return (react_1.default.createElement(ViewWin32_1.ViewWin32, { style: styles.largeContainer },
            react_1.default.createElement(TouchableWin32Highlight, { style: styles.outerTouch, onPress: this._onPress },
                react_1.default.createElement(ViewWin32_1.ViewWin32, null,
                    react_1.default.createElement(TextWin32_1.TextWin32, null, "Press me"))),
            react_1.default.createElement(TouchableWin32Highlight, { style: styles.outerTouch, onPress: this._onPress },
                react_1.default.createElement(ViewWin32_1.ViewWin32, { style: styles.smallContainer },
                    react_1.default.createElement(TouchableWin32Highlight, { style: styles.innerTouch, onPress: this._onPress, children: this._getChildrenOfInnerTouchable }))),
            react_1.default.createElement(TextWin32_1.TextWin32, { style: { color: 'red' }, textStyle: "None" }, 'Number of Recognized Presses: ' + this.state.numberOfPresses)));
    }
    _pickColor(hovered, pressed) {
        if (hovered) {
            if (pressed) {
                return 'white';
            }
            else {
                return 'red';
            }
        }
        else {
            if (pressed) {
                return 'purple';
            }
            else {
                return 'black';
            }
        }
    }
}
const TouchableFocusExample = () => {
    const [focused, setFocused] = react_1.default.useState(false);
    const focusableRef = react_1.default.useRef(null);
    // onPress callback
    const focusOnPress = react_1.default.useCallback(() => {
        focusableRef.current && focusableRef.current.focus();
        focused || setFocused(true);
    }, [focused]);
    // onFocus and onBlur callbacks
    const onFocus = react_1.default.useCallback(() => {
        setFocused(true);
    }, []);
    const onBlur = react_1.default.useCallback(() => {
        setFocused(false);
    }, []);
    return (react_1.default.createElement(ViewWin32_1.ViewWin32, { style: styles.largeContainer },
        react_1.default.createElement(TouchableWin32Highlight, { onPress: focusOnPress },
            react_1.default.createElement(react_native_1.Text, null, "Press me to focus my friend")),
        react_1.default.createElement(TouchableWin32Highlight, { ref: focusableRef, onFocus: onFocus, onBlur: onBlur },
            react_1.default.createElement(react_native_1.Text, null, 'Focused: ' + focused))));
};
exports.displayName = 'TouchableWin32 Examples';
exports.title = 'TouchableWin32';
exports.description = 'Demonstration of touchable + focus + hover behavior all in one component';
exports.examples = [
    {
        title: 'TouchableWithoutFeedback Example',
        description: 'A simple example implementation of without feedback behavior',
        render() {
            return react_1.default.createElement(TouchableWithoutFeedbackExample, null);
        },
    },
    {
        title: 'TouchableHighlight Example',
        description: 'A simple example implementation of highlight behavior',
        render() {
            return react_1.default.createElement(TouchableHighlightExample, null);
        },
    },
    {
        title: 'Imperative Focus on TouchableWin32 Example',
        description: 'A simple example implementation of imperative focus behavior',
        render() {
            return react_1.default.createElement(TouchableFocusExample, null);
        },
    }
];
//# sourceMappingURL=TouchableWin32Test.js.map