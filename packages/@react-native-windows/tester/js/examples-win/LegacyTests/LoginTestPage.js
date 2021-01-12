"use strict";
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 */
Object.defineProperty(exports, "__esModule", { value: true });
exports.examples = exports.description = exports.title = exports.displayName = exports.LoginTestPage = void 0;
const React = require("react");
const react_1 = require("react");
const react_native_1 = require("react-native");
const Consts_1 = require("./Consts");
const styles = react_native_1.StyleSheet.create({
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
    const [loginState, setLoginState] = react_1.useState('');
    const [userName, setUserName] = react_1.useState('');
    const [password, setPassword] = react_1.useState('');
    const [passwordHidden, setPasswordHidden] = react_1.useState(true);
    const onPressShowPassword = () => {
        const previousState = passwordHidden;
        setPasswordHidden(!previousState);
    };
    const onPress = () => {
        if (userName === 'username' && password === 'password') {
            setLoginState('Success');
        }
        else {
            setLoginState('Fail');
        }
    };
    return (React.createElement(react_native_1.View, null,
        React.createElement(react_native_1.TextInput, { style: styles.input, placeholder: "Email or Mobile Num", placeholderTextColor: "rgba(225,225,225,0.7)", testID: Consts_1.USERNAME_ON_LOGIN, onChange: text => {
                setUserName(text.nativeEvent.text);
            } }),
        React.createElement(react_native_1.TextInput, { style: styles.input, placeholder: "Password", testID: Consts_1.PASSWORD_ON_LOGIN, placeholderTextColor: "rgba(225,225,225,0.7)", secureTextEntry: passwordHidden, onChange: text => {
                setPassword(text.nativeEvent.text);
            } }),
        React.createElement(react_native_1.Button, { title: passwordHidden ? 'Show Password' : 'Hide Password', onPress: onPressShowPassword, testID: Consts_1.SHOWPASSWORD_ON_LOGIN }),
        React.createElement(react_native_1.TouchableOpacity, { style: styles.buttonContainer, testID: Consts_1.SUBMIT_ON_LOGIN, onPress: onPress },
            React.createElement(react_native_1.Text, { style: styles.buttonText }, "LOGIN")),
        React.createElement(react_native_1.Text, { testID: Consts_1.LOGINRESULT_ON_LOGIN }, loginState)));
}
exports.LoginTestPage = LoginTestPage;
exports.displayName = (_undefined) => { };
exports.title = 'LegacyLoginTest';
exports.description = 'Legacy e2e test for TextInput with password';
exports.examples = [
    {
        render: function () {
            return React.createElement(LoginTestPage, null);
        },
    },
];
//# sourceMappingURL=LoginTestPage.js.map