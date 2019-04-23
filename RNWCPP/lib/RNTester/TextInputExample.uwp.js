"use strict";
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
var __extends = (this && this.__extends) || (function () {
    var extendStatics = function (d, b) {
        extendStatics = Object.setPrototypeOf ||
            ({ __proto__: [] } instanceof Array && function (d, b) { d.__proto__ = b; }) ||
            function (d, b) { for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p]; };
        return extendStatics(d, b);
    };
    return function (d, b) {
        extendStatics(d, b);
        function __() { this.constructor = d; }
        d.prototype = b === null ? Object.create(b) : (__.prototype = b.prototype, new __());
    };
})();
var __assign = (this && this.__assign) || function () {
    __assign = Object.assign || function(t) {
        for (var s, i = 1, n = arguments.length; i < n; i++) {
            s = arguments[i];
            for (var p in s) if (Object.prototype.hasOwnProperty.call(s, p))
                t[p] = s[p];
        }
        return t;
    };
    return __assign.apply(this, arguments);
};
var __rest = (this && this.__rest) || function (s, e) {
    var t = {};
    for (var p in s) if (Object.prototype.hasOwnProperty.call(s, p) && e.indexOf(p) < 0)
        t[p] = s[p];
    if (s != null && typeof Object.getOwnPropertySymbols === "function")
        for (var i = 0, p = Object.getOwnPropertySymbols(s); i < p.length; i++) if (e.indexOf(p[i]) < 0)
            t[p[i]] = s[p[i]];
    return t;
};
Object.defineProperty(exports, "__esModule", { value: true });
// This is a port of TextInputExample.ios.js
// Text children of TextInput examples are removed/edited
/* tslint:disable */
var react_native_1 = require("react-native");
// const InputAccessoryView = require('InputAccessoryView');
var React = require("react");
var WithLabel = /** @class */ (function (_super) {
    __extends(WithLabel, _super);
    function WithLabel() {
        return _super !== null && _super.apply(this, arguments) || this;
    }
    WithLabel.prototype.render = function () {
        return (React.createElement(react_native_1.View, { style: styles.labelContainer },
            React.createElement(react_native_1.View, { style: styles.label },
                React.createElement(react_native_1.Text, null, this.props.label)),
            this.props.children));
    };
    return WithLabel;
}(React.Component));
var TextEventsExample = /** @class */ (function (_super) {
    __extends(TextEventsExample, _super);
    function TextEventsExample() {
        var _this = _super !== null && _super.apply(this, arguments) || this;
        _this.state = {
            curText: '<No Event>',
            prevText: '<No Event>',
            prev2Text: '<No Event>',
            prev3Text: '<No Event>',
        };
        _this.updateText = function (text) {
            _this.setState(function (state) {
                return {
                    curText: text,
                    prevText: state.curText,
                    prev2Text: state.prevText,
                    prev3Text: state.prev2Text,
                };
            });
        };
        return _this;
    }
    TextEventsExample.prototype.render = function () {
        var _this = this;
        return (React.createElement(react_native_1.View, null,
            React.createElement(react_native_1.TextInput, { autoCapitalize: "none", placeholder: "Enter text to see events", autoCorrect: false, onFocus: function () { return _this.updateText('onFocus'); }, onBlur: function () { return _this.updateText('onBlur'); }, onChange: function (event) {
                    return _this.updateText('onChange text: ' + event.nativeEvent.text);
                }, onEndEditing: function (event) {
                    return _this.updateText('onEndEditing text: ' + event.nativeEvent.text);
                }, onSubmitEditing: function (event) {
                    return _this.updateText('onSubmitEditing text: ' + event.nativeEvent.text);
                }, onSelectionChange: function (event) {
                    return _this.updateText('onSelectionChange range: ' +
                        event.nativeEvent.selection.start +
                        ',' +
                        event.nativeEvent.selection.end);
                }, onKeyPress: function (event) {
                    _this.updateText('onKeyPress key: ' + event.nativeEvent.key);
                }, style: styles.default }),
            React.createElement(react_native_1.Text, { style: styles.eventLabel },
                this.state.curText,
                '\n',
                "(prev: ",
                this.state.prevText,
                ")",
                '\n',
                "(prev2: ",
                this.state.prev2Text,
                ")",
                '\n',
                "(prev3: ",
                this.state.prev3Text,
                ")")));
    };
    return TextEventsExample;
}(React.Component));
/*
class TextInputAccessoryViewExample extends React.Component<{}, *> {
  constructor(props) {
    super(props);
    this.state = {text: 'Placeholder Text'};
  }

  render() {
    const inputAccessoryViewID = 'inputAccessoryView1';
    return (
      <View>
        <TextInput
          style={styles.default}
          inputAccessoryViewID={inputAccessoryViewID}
          onChangeText={text => this.setState({text})}
          value={this.state.text}
        />
        <InputAccessoryView nativeID={inputAccessoryViewID}>
          <View style={{backgroundColor: 'white'}}>
            <Button
              onPress={() => this.setState({text: 'Placeholder Text'})}
              title="Reset Text"
            />
          </View>
        </InputAccessoryView>
      </View>
    );
  }
}
*/
var RewriteExample = /** @class */ (function (_super) {
    __extends(RewriteExample, _super);
    function RewriteExample(props) {
        var _this = _super.call(this, props) || this;
        _this.state = { text: '' };
        return _this;
    }
    RewriteExample.prototype.render = function () {
        var _this = this;
        var limit = 20;
        var remainder = limit - this.state.text.length;
        var remainderColor = remainder > 5 ? 'blue' : 'red';
        return (React.createElement(react_native_1.View, { style: styles.rewriteContainer },
            React.createElement(react_native_1.TextInput, { multiline: false, maxLength: limit, onChangeText: function (text) {
                    text = text.replace(/ /g, '_');
                    _this.setState({ text: text });
                }, style: styles.default, value: this.state.text }),
            React.createElement(react_native_1.Text, { style: [styles.remainder, { color: remainderColor }] }, remainder)));
    };
    return RewriteExample;
}(React.Component));
var RewriteExampleInvalidCharacters = /** @class */ (function (_super) {
    __extends(RewriteExampleInvalidCharacters, _super);
    function RewriteExampleInvalidCharacters(props) {
        var _this = _super.call(this, props) || this;
        _this.state = { text: '' };
        return _this;
    }
    RewriteExampleInvalidCharacters.prototype.render = function () {
        var _this = this;
        return (React.createElement(react_native_1.View, { style: styles.rewriteContainer },
            React.createElement(react_native_1.TextInput, { multiline: false, onChangeText: function (text) {
                    _this.setState({ text: text.replace(/\s/g, '') });
                }, style: styles.default, value: this.state.text })));
    };
    return RewriteExampleInvalidCharacters;
}(React.Component));
var RewriteExampleKana = /** @class */ (function (_super) {
    __extends(RewriteExampleKana, _super);
    function RewriteExampleKana(props) {
        var _this = _super.call(this, props) || this;
        _this.state = { text: '' };
        return _this;
    }
    RewriteExampleKana.prototype.render = function () {
        var _this = this;
        return (React.createElement(react_native_1.View, { style: styles.rewriteContainer },
            React.createElement(react_native_1.TextInput, { multiline: false, onChangeText: function (text) {
                    _this.setState({ text: text.replace(/ひ/g, '日') });
                }, style: styles.default, value: this.state.text })));
    };
    return RewriteExampleKana;
}(React.Component));
var SecureEntryExample = /** @class */ (function (_super) {
    __extends(SecureEntryExample, _super);
    function SecureEntryExample(props) {
        var _this = _super.call(this, props) || this;
        _this.state = { text: '' };
        return _this;
    }
    SecureEntryExample.prototype.render = function () {
        var _this = this;
        return (React.createElement(react_native_1.View, null,
            React.createElement(react_native_1.TextInput, { secureTextEntry: true, style: styles.default, defaultValue: "abc", onChangeText: function (text) { return _this.setState({ text: text }); }, value: this.state.text }),
            React.createElement(react_native_1.Text, null,
                "Current text is: ",
                this.state.text)));
    };
    return SecureEntryExample;
}(React.Component));
var TokenizedTextExample = /** @class */ (function (_super) {
    __extends(TokenizedTextExample, _super);
    function TokenizedTextExample(props) {
        var _this = _super.call(this, props) || this;
        _this.state = { text: 'Hello #World' };
        return _this;
    }
    TokenizedTextExample.prototype.render = function () {
        var _this = this;
        // define delimiter
        var delimiter = /\s+/;
        // split string
        var _text = this.state.text;
        var token, index, parts = [];
        while (_text) {
            delimiter.lastIndex = 0;
            token = delimiter.exec(_text);
            if (token === null) {
                break;
            }
            index = token.index;
            if (token[0].length === 0) {
                index = 1;
            }
            parts.push(_text.substr(0, index));
            parts.push(token[0]);
            index = index + token[0].length;
            _text = _text.slice(index);
        }
        parts.push(_text);
        // highlight hashtags
        parts = parts.map(function (text) {
            if (/^#/.test(text)) {
                return (React.createElement(react_native_1.Text, { key: text, style: styles.hashtag }, text));
            }
            else {
                return text;
            }
        });
        return (React.createElement(react_native_1.View, null,
            React.createElement(react_native_1.TextInput, { multiline: true, style: styles.multiline, onChangeText: function (text) {
                    _this.setState({ text: text });
                } })));
    };
    return TokenizedTextExample;
}(React.Component));
var BlurOnSubmitExample = /** @class */ (function (_super) {
    __extends(BlurOnSubmitExample, _super);
    function BlurOnSubmitExample() {
        var _this = _super !== null && _super.apply(this, arguments) || this;
        _this.focusNextField = function (nextField) {
            var ref = _this.refs[nextField];
            ref.focus();
        };
        return _this;
    }
    BlurOnSubmitExample.prototype.render = function () {
        var _this = this;
        return (React.createElement(react_native_1.View, null,
            React.createElement(react_native_1.TextInput, { ref: "1", style: styles.default, placeholder: "blurOnSubmit = false", returnKeyType: "next", blurOnSubmit: false, onSubmitEditing: function () { return _this.focusNextField('2'); } }),
            React.createElement(react_native_1.TextInput, { ref: "2", style: styles.default, keyboardType: "email-address", placeholder: "blurOnSubmit = false", returnKeyType: "next", blurOnSubmit: false, onSubmitEditing: function () { return _this.focusNextField('3'); } }),
            React.createElement(react_native_1.TextInput, { ref: "3", style: styles.default, keyboardType: "url", placeholder: "blurOnSubmit = false", returnKeyType: "next", blurOnSubmit: false, onSubmitEditing: function () { return _this.focusNextField('4'); } }),
            React.createElement(react_native_1.TextInput, { ref: "4", style: styles.default, keyboardType: "numeric", returnKeyType: "done", placeholder: "blurOnSubmit = false", blurOnSubmit: false, onSubmitEditing: function () { return _this.focusNextField('5'); } }),
            React.createElement(react_native_1.TextInput, { ref: "5", style: styles.default, keyboardType: "numbers-and-punctuation", placeholder: "blurOnSubmit = true", returnKeyType: "done" })));
    };
    return BlurOnSubmitExample;
}(React.Component));
var SelectionExample = /** @class */ (function (_super) {
    __extends(SelectionExample, _super);
    function SelectionExample(props) {
        var _this = _super.call(this, props) || this;
        _this.state = {
            selection: { start: 0, end: 0 },
            value: props.value,
        };
        return _this;
    }
    SelectionExample.prototype.onSelectionChange = function (_a) {
        var selection = _a.nativeEvent.selection;
        this.setState({ selection: selection });
    };
    SelectionExample.prototype.getRandomPosition = function () {
        var length = this.state.value.length;
        return Math.round(Math.random() * length);
    };
    SelectionExample.prototype.select = function (start, end) {
        this._textInput.focus();
        this.setState({ selection: { start: start, end: end } });
    };
    SelectionExample.prototype.selectRandom = function () {
        var positions = [this.getRandomPosition(), this.getRandomPosition()].sort(function (a, b) { return a - b; });
        this.select(positions[0], positions[1]);
    };
    SelectionExample.prototype.placeAt = function (position) {
        this.select(position, position);
    };
    SelectionExample.prototype.placeAtRandom = function () {
        this.placeAt(this.getRandomPosition());
    };
    SelectionExample.prototype.render = function () {
        var _this = this;
        var length = this.state.value.length;
        return (React.createElement(react_native_1.View, null,
            React.createElement(react_native_1.TextInput, { multiline: this.props.multiline, onChangeText: function (value) { return _this.setState({ value: value }); }, onSelectionChange: this.onSelectionChange.bind(this), ref: function (textInput) { return (_this._textInput = textInput); }, selection: this.state.selection, style: this.props.style, value: this.state.value }),
            React.createElement(react_native_1.View, null,
                React.createElement(react_native_1.Text, null,
                    "selection = ",
                    JSON.stringify(this.state.selection)),
                React.createElement(react_native_1.Text, { onPress: this.placeAt.bind(this, 0) }, "Place at Start (0, 0)"),
                React.createElement(react_native_1.Text, { onPress: this.placeAt.bind(this, length) },
                    "Place at End (",
                    length,
                    ", ",
                    length,
                    ")"),
                React.createElement(react_native_1.Text, { onPress: this.placeAtRandom.bind(this) }, "Place at Random"),
                React.createElement(react_native_1.Text, { onPress: this.select.bind(this, 0, length) }, "Select All"),
                React.createElement(react_native_1.Text, { onPress: this.selectRandom.bind(this) }, "Select Random"))));
    };
    return SelectionExample;
}(React.Component));
var AutogrowingTextInputExample = /** @class */ (function (_super) {
    __extends(AutogrowingTextInputExample, _super);
    function AutogrowingTextInputExample(props) {
        var _this = _super.call(this, props) || this;
        _this.state = {
            width: 100,
            multiline: true,
            text: '',
            contentSize: {
                width: 0,
                height: 0,
            },
        };
        return _this;
    }
    AutogrowingTextInputExample.prototype.UNSAFE_componentWillReceiveProps = function (props) {
        this.setState({
            multiline: props.multiline,
        });
    };
    AutogrowingTextInputExample.prototype.render = function () {
        var _this = this;
        var _a = this.props, style = _a.style, multiline = _a.multiline, props = __rest(_a, ["style", "multiline"]);
        return (React.createElement(react_native_1.View, null,
            React.createElement(react_native_1.Text, null, "Width:"),
            React.createElement(react_native_1.Text, null, "Multiline:"),
            React.createElement(react_native_1.Switch, { value: this.state.multiline, onValueChange: function (value) { return _this.setState({ multiline: value }); } }),
            React.createElement(react_native_1.Text, null, "TextInput:"),
            React.createElement(react_native_1.TextInput, __assign({ value: "prop", multiline: this.state.multiline, style: [style, { width: this.state.width + '%' }], onChangeText: function (value) { return _this.setState({ text: value }); }, onContentSizeChange: function (event) {
                    return _this.setState({ contentSize: event.nativeEvent.contentSize });
                } }, props)),
            React.createElement(react_native_1.Text, null, "Plain text value representation:"),
            React.createElement(react_native_1.Text, null, this.state.text),
            React.createElement(react_native_1.Text, null,
                "Content Size: ",
                JSON.stringify(this.state.contentSize))));
    };
    return AutogrowingTextInputExample;
}(React.Component));
exports.AutogrowingTextInputExample = AutogrowingTextInputExample;
var styles = react_native_1.StyleSheet.create({
    page: {
        paddingBottom: 300,
    },
    default: {
        borderWidth: react_native_1.StyleSheet.hairlineWidth,
        borderColor: '#0f0f0f',
        flex: 1,
        fontSize: 13,
        padding: 4,
    },
    multiline: {
        borderWidth: react_native_1.StyleSheet.hairlineWidth,
        borderColor: '#0f0f0f',
        flex: 1,
        fontSize: 13,
        height: 50,
        padding: 4,
        marginBottom: 4,
    },
    multilineExpandable: {
        height: 'auto',
        maxHeight: 100,
    },
    multilineWithFontStyles: {
        color: 'blue',
        fontWeight: 'bold',
        fontSize: 18,
        fontFamily: 'Cochin',
        height: 60,
    },
    multilineChild: {
        width: 50,
        height: 40,
        position: 'absolute',
        right: 5,
        backgroundColor: 'red',
    },
    eventLabel: {
        margin: 3,
        fontSize: 12,
    },
    labelContainer: {
        flexDirection: 'row',
        marginVertical: 2,
        flex: 1,
    },
    label: {
        width: 115,
        alignItems: 'flex-end',
        marginRight: 10,
        paddingTop: 2,
    },
    rewriteContainer: {
        flexDirection: 'row',
        alignItems: 'center',
    },
    remainder: {
        textAlign: 'right',
        width: 24,
    },
    hashtag: {
        color: 'blue',
        fontWeight: 'bold',
    },
});
exports.displayName = undefined;
exports.title = '<TextInput>';
exports.description = 'Single and multi-line text inputs.';
exports.examples = [
    {
        title: 'Auto-focus',
        render: function () {
            return (React.createElement(react_native_1.TextInput, { autoFocus: true, style: styles.default, accessibilityLabel: "I am the accessibility label for text input" }));
        },
    },
    {
        title: "Live Re-Write (<sp>  ->  '_') + maxLength",
        render: function () {
            return React.createElement(RewriteExample, null);
        },
    },
    {
        title: 'Live Re-Write (no spaces allowed)',
        render: function () {
            return React.createElement(RewriteExampleInvalidCharacters, null);
        },
    },
    {
        title: 'Live Re-Write (ひ -> 日)',
        render: function () {
            return React.createElement(RewriteExampleKana, null);
        },
    },
    /*
    {
      title: 'Keyboard Accessory View',
      render: function() {
        return <TextInputAccessoryViewExample />;
      },
    },
    */
    {
        title: 'Auto-capitalize',
        render: function () {
            return (React.createElement(react_native_1.View, null,
                React.createElement(WithLabel, { label: "none" },
                    React.createElement(react_native_1.TextInput, { autoCapitalize: "none", style: styles.default })),
                React.createElement(WithLabel, { label: "sentences" },
                    React.createElement(react_native_1.TextInput, { autoCapitalize: "sentences", style: styles.default })),
                React.createElement(WithLabel, { label: "words" },
                    React.createElement(react_native_1.TextInput, { autoCapitalize: "words", style: styles.default })),
                React.createElement(WithLabel, { label: "characters" },
                    React.createElement(react_native_1.TextInput, { autoCapitalize: "characters", style: styles.default }))));
        },
    },
    {
        title: 'Auto-correct',
        render: function () {
            return (React.createElement(react_native_1.View, null,
                React.createElement(WithLabel, { label: "true" },
                    React.createElement(react_native_1.TextInput, { autoCorrect: true, style: styles.default })),
                React.createElement(WithLabel, { label: "false" },
                    React.createElement(react_native_1.TextInput, { autoCorrect: false, style: styles.default }))));
        },
    },
    /* TODO: enable text in TextInput
    {
      title: 'Nested content and `value` property',
      render: function() {
        return (
          <View>
            <WithLabel label="singleline">
              <TextInput style={styles.default} value="(value property)">
                (first raw text node)
                <Text style={{color: "red"}}>(internal raw text node)</Text>
                (last raw text node)
              </TextInput>
            </WithLabel>
            <WithLabel label="multiline">
              <TextInput
                style={styles.default}
                multiline={true}
                value="(value property)">
                (first raw text node)
                <Text style={{color: "red"}}>(internal raw text node)</Text>
                (last raw text node)
              </TextInput>
            </WithLabel>
          </View>
        );
      },
    },
    */
    {
        title: 'Keyboard types',
        render: function () {
            var keyboardTypes = [
                'default',
                'ascii-capable',
                'numbers-and-punctuation',
                'url',
                'number-pad',
                'phone-pad',
                'name-phone-pad',
                'email-address',
                'decimal-pad',
                'twitter',
                'web-search',
                'numeric',
            ];
            var examples = keyboardTypes.map(function (type) {
                return (React.createElement(WithLabel, { key: type, label: type },
                    React.createElement(react_native_1.TextInput, { keyboardType: type, style: styles.default })));
            });
            return React.createElement(react_native_1.View, null, examples);
        },
    },
    {
        title: 'Keyboard appearance',
        render: function () {
            var keyboardAppearance = ['default', 'light', 'dark'];
            var examples = keyboardAppearance.map(function (type) {
                return (React.createElement(WithLabel, { key: type, label: type },
                    React.createElement(react_native_1.TextInput, { keyboardAppearance: type, style: styles.default })));
            });
            return React.createElement(react_native_1.View, null, examples);
        },
    },
    {
        title: 'Return key types',
        render: function () {
            var returnKeyTypes = [
                'default',
                'go',
                'google',
                'join',
                'next',
                'route',
                'search',
                'send',
                'yahoo',
                'done',
                'emergency-call',
            ];
            var examples = returnKeyTypes.map(function (type) {
                return (React.createElement(WithLabel, { key: type, label: type },
                    React.createElement(react_native_1.TextInput, { returnKeyType: type, style: styles.default })));
            });
            return React.createElement(react_native_1.View, null, examples);
        },
    },
    {
        title: 'Enable return key automatically',
        render: function () {
            return (React.createElement(react_native_1.View, null,
                React.createElement(WithLabel, { label: "true" },
                    React.createElement(react_native_1.TextInput, { enablesReturnKeyAutomatically: true, style: styles.default }))));
        },
    },
    {
        title: 'Secure text entry',
        render: function () {
            return React.createElement(SecureEntryExample, null);
        },
    },
    {
        title: 'Event handling',
        render: function () {
            return React.createElement(TextEventsExample, null);
        },
    },
    {
        title: 'Colored input text',
        render: function () {
            return (React.createElement(react_native_1.View, null,
                React.createElement(react_native_1.TextInput, { style: [styles.default, { color: 'blue' }], defaultValue: "Blue" }),
                React.createElement(react_native_1.TextInput, { style: [styles.default, { color: 'green' }], defaultValue: "Green" })));
        },
    },
    {
        title: 'Colored highlight/cursor for text input',
        render: function () {
            return (React.createElement(react_native_1.View, null,
                React.createElement(react_native_1.TextInput, { style: styles.default, selectionColor: 'green', defaultValue: "Highlight me" }),
                React.createElement(react_native_1.TextInput, { style: styles.default, selectionColor: 'rgba(86, 76, 205, 1)', defaultValue: "Highlight me" })));
        },
    },
    {
        title: 'Clear button mode',
        render: function () {
            return (React.createElement(react_native_1.View, null,
                React.createElement(WithLabel, { label: "never" },
                    React.createElement(react_native_1.TextInput, { style: styles.default, clearButtonMode: "never" })),
                React.createElement(WithLabel, { label: "while editing" },
                    React.createElement(react_native_1.TextInput, { style: styles.default, clearButtonMode: "while-editing" })),
                React.createElement(WithLabel, { label: "unless editing" },
                    React.createElement(react_native_1.TextInput, { style: styles.default, clearButtonMode: "unless-editing" })),
                React.createElement(WithLabel, { label: "always" },
                    React.createElement(react_native_1.TextInput, { style: styles.default, clearButtonMode: "always" }))));
        },
    },
    {
        title: 'Clear and select',
        render: function () {
            return (React.createElement(react_native_1.View, null,
                React.createElement(WithLabel, { label: "clearTextOnFocus" },
                    React.createElement(react_native_1.TextInput, { placeholder: "text is cleared on focus", defaultValue: "text is cleared on focus", style: styles.default, clearTextOnFocus: true })),
                React.createElement(WithLabel, { label: "selectTextOnFocus" },
                    React.createElement(react_native_1.TextInput, { placeholder: "text is selected on focus", defaultValue: "text is selected on focus", style: styles.default, selectTextOnFocus: true })),
                React.createElement(WithLabel, { label: "clearTextOnFocus (multiline)" },
                    React.createElement(react_native_1.TextInput, { placeholder: "text is cleared on focus", defaultValue: "text is cleared on focus", style: styles.default, clearTextOnFocus: true, multiline: true })),
                React.createElement(WithLabel, { label: "selectTextOnFocus (multiline)" },
                    React.createElement(react_native_1.TextInput, { placeholder: "text is selected on focus", defaultValue: "text is selected on focus", style: styles.default, selectTextOnFocus: true, multiline: true }))));
        },
    },
    {
        title: 'Blur on submit',
        render: function () {
            return React.createElement(BlurOnSubmitExample, null);
        },
    },
    {
        title: 'Multiline blur on submit',
        render: function () {
            return (React.createElement(react_native_1.View, null,
                React.createElement(react_native_1.TextInput, { style: styles.multiline, placeholder: "blurOnSubmit = true", returnKeyType: "next", blurOnSubmit: true, multiline: true, onSubmitEditing: function (event) { return alert(event.nativeEvent.text); } })));
        },
    },
    {
        title: 'Multiline',
        render: function () {
            return (React.createElement(react_native_1.View, null,
                React.createElement(react_native_1.TextInput, { placeholder: "multiline text input", multiline: true, style: styles.multiline }),
                React.createElement(react_native_1.TextInput, { placeholder: "multiline text input with font styles and placeholder", multiline: true, clearTextOnFocus: true, autoCorrect: true, autoCapitalize: "words", placeholderTextColor: "red", keyboardType: "url", style: [styles.multiline, styles.multilineWithFontStyles] }),
                React.createElement(react_native_1.TextInput, { placeholder: "multiline text input with max length", maxLength: 5, multiline: true, style: styles.multiline }),
                React.createElement(react_native_1.TextInput, { placeholder: "uneditable multiline text input", editable: false, multiline: true, style: styles.multiline }),
                React.createElement(react_native_1.TextInput, { defaultValue: "uneditable multiline text input with phone number detection: 88888888.", editable: false, multiline: true, style: styles.multiline, dataDetectorTypes: "phoneNumber" })));
        },
    },
    {
        title: 'TextInput Intrinsic Size',
        render: function () {
            return (React.createElement(react_native_1.View, null,
                React.createElement(react_native_1.Text, null, "Singleline TextInput"),
                React.createElement(react_native_1.View, { style: { height: 80 } },
                    React.createElement(react_native_1.TextInput, { style: {
                            position: 'absolute',
                            fontSize: 16,
                            backgroundColor: '#eeeeee',
                            borderColor: '#666666',
                            borderWidth: 5,
                            borderTopWidth: 20,
                            borderRadius: 10,
                            borderBottomRightRadius: 20,
                            padding: 10,
                            paddingTop: 20,
                        }, testID: "singleline_textinput", placeholder: "Placeholder defines intrinsic size" })),
                React.createElement(react_native_1.Text, null, "Multiline TextInput"),
                React.createElement(react_native_1.View, { style: { height: 130 } },
                    React.createElement(react_native_1.TextInput, { style: {
                            position: 'absolute',
                            fontSize: 16,
                            backgroundColor: '#eeeeee',
                            borderColor: '#666666',
                            borderWidth: 5,
                            borderTopWidth: 20,
                            borderRadius: 10,
                            borderBottomRightRadius: 20,
                            padding: 10,
                            paddingTop: 20,
                            maxHeight: 100,
                        }, testID: "multiline_textinput", multiline: true, placeholder: "Placeholder defines intrinsic size" })),
                React.createElement(react_native_1.View, null,
                    React.createElement(react_native_1.TextInput, { style: {
                            fontSize: 16,
                            backgroundColor: '#eeeeee',
                            borderColor: '#666666',
                            borderWidth: 5,
                            borderTopWidth: 20,
                            borderRadius: 10,
                            borderBottomRightRadius: 20,
                            padding: 10,
                            paddingTop: 20,
                        }, testID: "multiline_textinput_with_flex", multiline: true, placeholder: "Placeholder defines intrinsic size" }))));
        },
    },
    {
        title: 'Auto-expanding',
        render: function () {
            return (React.createElement(react_native_1.View, null,
                React.createElement(react_native_1.TextInput, { placeholder: "height increases with content", defaultValue: "React Native enables you to build world-class application experiences on native platforms using a consistent developer experience based on JavaScript and React. The focus of React Native is on developer efficiency across all the platforms you care about - learn once, write anywhere. Facebook uses React Native in multiple production apps and will continue investing in React Native.", multiline: true, enablesReturnKeyAutomatically: true, returnKeyType: "go", style: [styles.multiline, styles.multilineExpandable] })));
        },
    },
    /* NYI Text in TextInput
    {
      title: 'Auto-expanding',
      render: function() {
        return (
          <View>
            <AutogrowingTextInputExample
              enablesReturnKeyAutomatically={true}
              returnKeyType="done"
              multiline={true}
              style={{
                maxHeight: 400,
                minHeight: 20,
                paddingTop: 0,
                backgroundColor: '#eeeeee',
                color: 'blue',
              }}>
              <Text style={{fontSize: 30, color: 'green'}}>huge</Text>
              generic generic generic
              <Text style={{fontSize: 6, color: 'red'}}>
                small small small small small small
              </Text>
              <Text>regular regular</Text>
              <Text style={{fontSize: 30, color: 'green'}}>
                huge huge huge huge huge
              </Text>
              generic generic generic
            </AutogrowingTextInputExample>
          </View>
        );
      },
    },
    */
    {
        title: 'Attributed text',
        render: function () {
            return React.createElement(TokenizedTextExample, null);
        },
    },
    {
        title: 'Text selection & cursor placement',
        render: function () {
            return (React.createElement(react_native_1.View, null,
                React.createElement(SelectionExample, { style: styles.default, value: "text selection can be changed" }),
                React.createElement(SelectionExample, { multiline: true, style: styles.multiline, value: 'multiline text selection\ncan also be changed' })));
        },
    },
    {
        title: 'TextInput maxLength',
        render: function () {
            return (React.createElement(react_native_1.View, null,
                React.createElement(WithLabel, { label: "maxLength: 5" },
                    React.createElement(react_native_1.TextInput, { maxLength: 5, style: styles.default })),
                React.createElement(WithLabel, { label: "maxLength: 5 with placeholder" },
                    React.createElement(react_native_1.TextInput, { maxLength: 5, placeholder: "ZIP code entry", style: styles.default })),
                React.createElement(WithLabel, { label: "maxLength: 5 with default value already set" },
                    React.createElement(react_native_1.TextInput, { maxLength: 5, defaultValue: "94025", style: styles.default })),
                React.createElement(WithLabel, { label: "maxLength: 5 with very long default value already set" },
                    React.createElement(react_native_1.TextInput, { maxLength: 5, defaultValue: "9402512345", style: styles.default }))));
        },
    },
];
//# sourceMappingURL=TextInputExample.uwp.js.map