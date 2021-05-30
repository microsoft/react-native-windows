'use strict';
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
exports.examples = exports.description = exports.displayName = exports.title = void 0;
const react_1 = __importDefault(require("react"));
const react_native_1 = require("react-native");
// Disabling no-jsx-lambda so functional components are more convenient to use
const AutoFocusingTextInputTest = () => {
    return (react_1.default.createElement(react_native_1.View, null,
        react_1.default.createElement(react_native_1.Text, null, "This TextInput auto focuses"),
        react_1.default.createElement(react_native_1.TextInput, { autoFocus: true, maxLength: 140, multiline: true, style: styles.input })));
};
const PlaceholderTextInputTest = () => {
    return (react_1.default.createElement(react_native_1.View, null,
        react_1.default.createElement(react_native_1.Text, null, "This TextInput demonstrates placeholders"),
        react_1.default.createElement(react_native_1.TextInput, { maxLength: 140, multiline: true, placeholder: "I am a blue placeholder, type in me for something magical", placeholderTextColor: "blue", style: styles.green })));
};
const ControllingTextInputTest = () => {
    const [value, setValue] = react_1.default.useState('');
    return (react_1.default.createElement(react_native_1.View, null,
        react_1.default.createElement(react_native_1.Text, null, "This TextInput inserts spaces between characters"),
        react_1.default.createElement(react_native_1.TextInput, { multiline: true, onChangeText: (text) => setValue(text.endsWith(' ') ? value : (text + ' ')), style: styles.blue, value: value })));
};
const BlurringAndFocusingTextInputTest = () => {
    const [isFocused, setIsFocused] = react_1.default.useState(false);
    return (react_1.default.createElement(react_native_1.View, null,
        react_1.default.createElement(react_native_1.Text, null,
            "This TextInput is currently focused: + ",
            isFocused ? 'true' : 'false'),
        react_1.default.createElement(react_native_1.TextInput, { multiline: true, onBlur: () => setIsFocused(false), onFocus: () => setIsFocused(true), style: styles.input })));
};
const LayoutListeningTextInputTest = () => {
    const [eventCounter, setEventCounter] = react_1.default.useState(0);
    return (react_1.default.createElement(react_native_1.View, null,
        react_1.default.createElement(react_native_1.Text, null,
            "The number of onContentSizeChanges is listed here + ",
            eventCounter),
        react_1.default.createElement(react_native_1.TextInput, { multiline: true, onContentSizeChange: () => setEventCounter(eventCounter + 1), style: styles.input })));
};
const KeyPressListeningTextInputTest = () => {
    const [value, setValue] = react_1.default.useState('');
    return (react_1.default.createElement(react_native_1.View, null,
        react_1.default.createElement(react_native_1.Text, null, "This TextInput uses onKeyPress to maintain state: it is slow"),
        react_1.default.createElement(react_native_1.TextInput, { multiline: true, onKeyPress: (e) => setValue(value + e.nativeEvent.key), style: styles.input, value: value })));
};
const StyleTextInputTest = () => {
    return (react_1.default.createElement(react_native_1.View, null,
        react_1.default.createElement(react_native_1.Text, null, "This TextInput is styled differently"),
        react_1.default.createElement(react_native_1.TextInput, { multiline: true, style: styles.superInput })));
};
exports.title = 'TextInput';
exports.displayName = 'TextInput';
exports.description = 'TextInput Examples and Tests';
exports.examples = [
    {
        title: 'Autofocus Example',
        description: 'autoFocus in action',
        render() {
            return (react_1.default.createElement(AutoFocusingTextInputTest, null));
        },
    },
    {
        title: 'Placeholders Example',
        description: 'placeholder in action',
        render() {
            return (react_1.default.createElement(PlaceholderTextInputTest, null));
        },
    },
    {
        title: 'Controlled Example',
        description: 'Controlling inputs in action',
        render() {
            return (react_1.default.createElement(ControllingTextInputTest, null));
        },
    },
    {
        title: 'Focus and Blur Example',
        description: 'onFocus/onBlur in action',
        render() {
            return (react_1.default.createElement(BlurringAndFocusingTextInputTest, null));
        },
    },
    {
        title: 'ContentSizeChange Example',
        description: 'onContentSizeChange in action',
        render() {
            return (react_1.default.createElement(LayoutListeningTextInputTest, null));
        },
    },
    {
        title: 'Control via onKeyPress Example',
        description: 'onKeyPress in action',
        render() {
            return (react_1.default.createElement(KeyPressListeningTextInputTest, null));
        },
    },
    {
        title: 'Super Styling Example',
        description: 'Styling in action',
        render() {
            return (react_1.default.createElement(StyleTextInputTest, null));
        },
    },
];
const styles = react_native_1.StyleSheet.create({
    input: {
        height: 20,
    },
    blue: {
        color: 'blue',
        height: 20,
    },
    green: {
        color: 'green',
        height: 20,
    },
    superInput: {
        color: 'purple',
        fontSize: 24,
        fontStyle: 'italic',
        fontWeight: 'bold',
        fontFamily: 'times-new-roman',
        height: 100,
    },
});
//# sourceMappingURL=TextInputTest.js.map