"use strict";
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 */
Object.defineProperty(exports, "__esModule", { value: true });
exports.examples = exports.description = exports.title = exports.displayName = exports.ControlStyleTestPage = void 0;
const react_native_1 = require("react-native");
const react_native_windows_1 = require("react-native-windows");
const React = require("react");
const react_1 = require("react");
const Consts_1 = require("./Consts");
const TreeDumpControl_1 = require("./TreeDumpControl");
const styles = react_native_1.StyleSheet.create({
    container: {
        padding: 20,
    },
    regularBorder: {
        height: 50,
        backgroundColor: 'rgba(225,225,225,0.2)',
        borderWidth: 1,
        borderColor: '#ff00ff55',
        marginBottom: 10,
        padding: 10,
        color: '#fff',
    },
    roundBorder: {
        height: 50,
        backgroundColor: 'rgba(0,0,0,0.2)',
        marginBottom: 10,
        padding: 10,
        color: '#000',
        borderRadius: 10.0,
        borderWidth: 10,
        borderColor: '#00ff0055',
    },
    buttonText: {
        color: '#fff',
        textAlign: 'center',
        fontWeight: '700',
        borderRadius: 10.0,
    },
    treeDumpControl: {
        height: 150,
        width: 500,
        margin: 10,
    },
});
function ControlStyleTestPage() {
    const [showRoundBorder, setShowRoundBorder] = react_1.useState(false);
    const onPressShowRoundBorder = () => {
        const previousState = showRoundBorder;
        setShowRoundBorder(!previousState);
    };
    return (React.createElement(react_native_1.View, null,
        React.createElement(react_native_1.View, { testID: 'ControlStyleView' },
            React.createElement(react_native_1.Switch, { style: showRoundBorder ? styles.roundBorder : styles.regularBorder, thumbColor: "blue" }),
            React.createElement(react_native_1.TextInput, { style: showRoundBorder ? styles.roundBorder : styles.regularBorder, placeholder: "TextBox", placeholderTextColor: "rgba(225,225,225,0.7)", editable: false }),
            React.createElement(react_native_1.TextInput, { style: showRoundBorder ? styles.roundBorder : styles.regularBorder, placeholder: "Password", placeholderTextColor: "rgba(225,225,225,0.7)", secureTextEntry: true, editable: false }),
            React.createElement(react_native_windows_1.DatePicker, { style: showRoundBorder ? styles.roundBorder : styles.regularBorder }),
            React.createElement(react_native_windows_1.Picker, { style: showRoundBorder ? styles.roundBorder : styles.regularBorder })),
        React.createElement(react_native_1.Button, { title: showRoundBorder ? 'Hide Round Border' : 'Show Round Border', onPress: onPressShowRoundBorder, testID: Consts_1.SHOWBORDER_ON_CONTROLSTYLE }),
        React.createElement(TreeDumpControl_1.TreeDumpControl, { style: styles.treeDumpControl, dumpID: showRoundBorder
                ? 'ControlStyleRoundBorder'
                : 'ControlStyleRegularBorder', uiaID: 'ControlStyleView', testID: Consts_1.TREE_DUMP_RESULT })));
}
exports.ControlStyleTestPage = ControlStyleTestPage;
exports.displayName = (_undefined) => { };
exports.title = 'LegacyControlStyleTest';
exports.description = 'Legacy e2e test for Control Styles';
exports.examples = [
    {
        render: function () {
            return React.createElement(ControlStyleTestPage, null);
        },
    },
];
//# sourceMappingURL=ControlStyleTestPage.js.map