"use strict";
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 */
Object.defineProperty(exports, "__esModule", { value: true });
exports.examples = exports.description = exports.title = exports.displayName = exports.LoginTestPage = void 0;
var React = require("react");
var react_1 = require("react");
var react_native_1 = require("react-native");
var Consts_1 = require("./Consts");
var styles = react_native_1.StyleSheet.create({
    container: {
        padding: 20,
    },
    input: {
        height: 40,
        backgroundColor: 'rgba(225,225,225,0.2)',
        marginBottom: 10,
        padding: 10,
        color: '#000000',
    },
    buttonContainer: {
        backgroundColor: '#2980b6',
        paddingVertical: 15,
    },
    buttonText: {
        color: '#fff',
        textAlign: 'center',
        fontWeight: '700',
    },
});
function LoginTestPage() {
    var _a = react_1.useState(''), loginState = _a[0], setLoginState = _a[1];
    var _b = react_1.useState(''), userName = _b[0], setUserName = _b[1];
    var _c = react_1.useState(''), password = _c[0], setPassword = _c[1];
    var _d = react_1.useState(true), passwordHidden = _d[0], setPasswordHidden = _d[1];
    var onPressShowPassword = function () {
        var previousState = passwordHidden;
        setPasswordHidden(!previousState);
    };
    var onPress = function () {
        if (userName === 'username' && password === 'password') {
            setLoginState('Success');
        }
        else {
            setLoginState('Fail');
        }
    };
    return (React.createElement(react_native_1.View, null,
        React.createElement(react_native_1.TextInput, { style: styles.input, placeholder: "Email or Mobile Num", placeholderTextColor: "rgba(225,225,225,0.7)", testID: Consts_1.USERNAME_ON_LOGIN, onChange: function (text) {
                setUserName(text.nativeEvent.text);
            } }),
        React.createElement(react_native_1.TextInput, { style: styles.input, placeholder: "Password", testID: Consts_1.PASSWORD_ON_LOGIN, placeholderTextColor: "rgba(225,225,225,0.7)", secureTextEntry: passwordHidden, onChange: function (text) {
                setPassword(text.nativeEvent.text);
            } }),
        React.createElement(react_native_1.Button, { title: passwordHidden ? 'Show Password' : 'Hide Password', onPress: onPressShowPassword, testID: Consts_1.SHOWPASSWORD_ON_LOGIN }),
        React.createElement(react_native_1.TouchableOpacity, { style: styles.buttonContainer, testID: Consts_1.SUBMIT_ON_LOGIN, onPress: onPress },
            React.createElement(react_native_1.Text, { style: styles.buttonText }, "LOGIN")),
        React.createElement(react_native_1.Text, { testID: Consts_1.LOGINRESULT_ON_LOGIN }, loginState)));
}
exports.LoginTestPage = LoginTestPage;
exports.displayName = function (_undefined) { };
exports.title = '<LegacyLoginTest>';
exports.description = 'Legacy e2e test for TextInput with password';
exports.examples = [
    {
        render: function () {
            return React.createElement(LoginTestPage, null);
        },
    },
];
//# sourceMappingURL=LoginTestPage.js.map