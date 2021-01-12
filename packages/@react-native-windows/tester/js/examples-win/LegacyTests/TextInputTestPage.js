"use strict";
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */
Object.defineProperty(exports, "__esModule", { value: true });
exports.examples = exports.description = exports.title = exports.displayName = exports.TextInputTestPage = void 0;
const React = require("react");
const react_native_1 = require("react-native");
const Consts_1 = require("./Consts");
class TextInputTestPage extends React.Component {
    constructor() {
        super(...arguments);
        this.state = {
            log: '<Log Start>',
        };
        this.updateText = (text) => {
            this.setState(state => {
                return {
                    log: text + '\n' + state.log,
                };
            });
        };
    }
    render() {
        return (React.createElement(react_native_1.View, null,
            React.createElement(react_native_1.TextInput, { style: { height: 32 }, testID: Consts_1.TEXTINPUT_ON_TEXTINPUT, placeholder: "Enter text to see events", onFocus: () => this.updateText('onFocus'), onBlur: () => this.updateText('onBlur'), onChange: event => this.updateText('onChange text: ' + event.nativeEvent.text), onEndEditing: event => this.updateText('onEndEditing text: ' + event.nativeEvent.text), onSubmitEditing: event => this.updateText('onSubmitEditing text: ' + event.nativeEvent.text), onSelectionChange: event => {
                    this.updateText('onSelectionChange range: ' +
                        event.nativeEvent.selection.start +
                        ',' +
                        event.nativeEvent.selection.end);
                }, onKeyPress: event => {
                    this.updateText('onKeyPress key: ' + event.nativeEvent.key);
                } }),
            React.createElement(react_native_1.TextInput, { testID: Consts_1.ML_TEXTINPUT_ON_TEXTINPUT, style: { height: 80 }, placeholder: "MultiLine", multiline: true }),
            React.createElement(react_native_1.TextInput, { testID: Consts_1.CAP_TEXTINPUT_ON_TEXTINPUT, style: { height: 80 }, placeholder: "autoCapitalize", autoCapitalize: "characters" }),
            React.createElement(react_native_1.Text, { testID: Consts_1.CURTEXT_ON_TEXTINPUT }, this.state.log)));
    }
}
exports.TextInputTestPage = TextInputTestPage;
exports.displayName = (_undefined) => { };
exports.title = 'LegacyTextInputTest';
exports.description = 'Legacy e2e test for TextInput';
exports.examples = [
    {
        render: function () {
            return React.createElement(TextInputTestPage, null);
        },
    },
];
//# sourceMappingURL=TextInputTestPage.js.map