"use strict";
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */
Object.defineProperty(exports, "__esModule", { value: true });
exports.examples = exports.description = exports.category = exports.title = exports.displayName = void 0;
const React = require("react");
const react_native_1 = require("react-native");
const styles = react_native_1.StyleSheet.create({
    withBorder: {
        borderRadius: 5,
    },
    withoutBorder: {
        direction: 'rtl',
    },
    zIndex: {
        justifyContent: 'space-around',
        width: 100,
        height: 50,
        marginTop: -10,
    },
    blankStyle: {},
    border: {
        borderRadius: 5,
    },
});
class FlowDirectionChange extends React.Component {
    constructor() {
        super(...arguments);
        this.state = {
            useBorder: false,
        };
        this.handleClick = () => {
            this.setState({ useBorder: !this.state.useBorder });
        };
    }
    render() {
        const currentStyle = this.state.useBorder
            ? styles.withBorder
            : styles.withoutBorder;
        return (React.createElement(react_native_1.View, { style: { flex: 1 } },
            React.createElement(react_native_1.View, { style: [styles.withoutBorder, currentStyle] },
                React.createElement(react_native_1.Text, { style: { fontSize: 11 } }, "This text should remain on the right after changing the radius")),
            React.createElement(react_native_1.Button, { onPress: this.handleClick, title: "Change Border Radius" })));
    }
}
class ZIndexChange extends React.Component {
    constructor() {
        super(...arguments);
        this.state = {
            cornerRadius: false,
            flipped: false,
        };
        this.handleClick = () => {
            this.setState({ cornerRadius: !this.state.cornerRadius });
        };
        this.handlePress = () => {
            this.setState({ flipped: !this.state.flipped });
        };
    }
    render() {
        const hasRadius = this.state.cornerRadius
            ? styles.border
            : styles.blankStyle;
        const indices = this.state.flipped ? [-1, 0, 1, 2] : [2, 1, 0, -1];
        return (React.createElement(react_native_1.View, null,
            React.createElement(react_native_1.View, null,
                React.createElement(react_native_1.Text, { style: { paddingBottom: 10 } }, "The zIndex should remain the same after changing the border radius"),
                React.createElement(react_native_1.View, { testID: "automationID", style: [
                        styles.zIndex,
                        {
                            marginTop: 0,
                            backgroundColor: '#E57373',
                            zIndex: indices[0],
                        },
                        hasRadius,
                    ] },
                    React.createElement(react_native_1.Text, null,
                        "ZIndex ",
                        indices[0])),
                React.createElement(react_native_1.View, { style: [
                        styles.zIndex,
                        {
                            marginLeft: 50,
                            backgroundColor: '#FFF176',
                            zIndex: indices[1],
                        },
                        hasRadius,
                    ] },
                    React.createElement(react_native_1.Text, null,
                        "ZIndex ",
                        indices[1])),
                React.createElement(react_native_1.View, { style: [
                        styles.zIndex,
                        {
                            marginLeft: 100,
                            backgroundColor: '#81C784',
                            zIndex: indices[2],
                        },
                        hasRadius,
                    ] },
                    React.createElement(react_native_1.Text, null,
                        "ZIndex ",
                        indices[2])),
                React.createElement(react_native_1.View, { style: [
                        styles.zIndex,
                        {
                            marginLeft: 150,
                            backgroundColor: '#64B5F6',
                            zIndex: indices[3],
                        },
                        hasRadius,
                    ] },
                    React.createElement(react_native_1.Text, null,
                        "ZIndex ",
                        indices[3]))),
            React.createElement(react_native_1.Button, { onPress: this.handleClick, title: "Change Border Radius" }),
            React.createElement(react_native_1.Button, { onPress: this.handlePress, title: "Change Sorting Order" })));
    }
}
exports.displayName = (_undefined) => { };
exports.title = 'TransferProperties';
exports.category = 'Basic';
exports.description = 'Some tests that change the backing XAML element ' +
    'to see if transfer properties is working.';
exports.examples = [
    {
        title: 'Flow Direction Change',
        render: function () {
            return React.createElement(FlowDirectionChange, null);
        },
    },
    {
        title: 'zIndex Change',
        render: function () {
            return React.createElement(ZIndexChange, null);
        },
    },
];
//# sourceMappingURL=TransferPropertiesExample.js.map