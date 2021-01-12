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
class AsyncStorageExample extends React.Component {
    constructor() {
        super(...arguments);
        this.state = {
            asyncStorageData: [],
            name: '',
            value: '',
        };
        this.setName = (name) => {
            this.setState({ name: name });
        };
        this.setValue = (value) => {
            this.setState({ value: value });
        };
        this.onAddEntryPress = () => {
            void react_native_1.AsyncStorage.setItem(this.state.name, this.state.value);
            this.updateAsyncStorageData(this.state.name, this.state.value);
        };
        this.onClearAllKeysPress = () => {
            void react_native_1.AsyncStorage.clear();
            this.setState({ asyncStorageData: [] });
        };
    }
    async componentDidMount() {
        const allKeys = await react_native_1.AsyncStorage.getAllKeys();
        const values = await react_native_1.AsyncStorage.multiGet(allKeys);
        this.setState({ asyncStorageData: values });
    }
    updateAsyncStorageData(key, value) {
        this.setState(prevState => {
            const asyncStorageData = [...prevState.asyncStorageData];
            let foundVal = false;
            for (const kvp of asyncStorageData) {
                if (kvp[0] === key) {
                    kvp[1] = value;
                    foundVal = true;
                    break;
                }
            }
            if (!foundVal) {
                asyncStorageData.push([key, value]);
            }
            return { asyncStorageData: asyncStorageData };
        });
    }
    makeOnRemoveEntryPress(key) {
        return () => {
            void react_native_1.AsyncStorage.removeItem(key);
            this.setState(prevState => {
                const asyncStorageData = prevState.asyncStorageData.filter(kvp => kvp[0] !== key);
                return { asyncStorageData: asyncStorageData };
            });
        };
    }
    render() {
        return (React.createElement(react_native_1.View, { style: styles.container },
            React.createElement(react_native_1.Button, { title: "Clear All Keys", onPress: this.onClearAllKeysPress }),
            React.createElement(react_native_1.FlatList, { data: this.state.asyncStorageData, renderItem: ({ item }) => (React.createElement(react_native_1.View, { style: styles.keyValue },
                    React.createElement(react_native_1.Text, null,
                        "\"",
                        item[0],
                        "\": \"",
                        item[1],
                        "\""),
                    React.createElement(react_native_1.Button, { title: "Remove", onPress: this.makeOnRemoveEntryPress(item[0]) }))) }),
            React.createElement(react_native_1.Text, null, "Add/Update an entry:"),
            React.createElement(react_native_1.View, { style: styles.keyValue },
                React.createElement(react_native_1.Text, null, "Name:"),
                React.createElement(react_native_1.TextInput, { autoCapitalize: "none", onChangeText: this.setName, placeholder: "<Key>", style: styles.textInput })),
            React.createElement(react_native_1.View, { style: styles.keyValue },
                React.createElement(react_native_1.Text, null, "Value:"),
                React.createElement(react_native_1.TextInput, { autoCapitalize: "none", onChangeText: this.setValue, placeholder: "<New Value>", style: styles.textInput })),
            React.createElement(react_native_1.Button, { title: "Add Entry", onPress: this.onAddEntryPress })));
    }
}
const styles = react_native_1.StyleSheet.create({
    button: {
        backgroundColor: '#CCCCCC',
        margin: 10,
    },
    container: {
        flex: 1,
        alignItems: 'center',
        marginTop: 80,
    },
    keyValue: {
        alignItems: 'baseline',
        flexDirection: 'row',
    },
    textInput: {
        margin: 5,
        height: 32,
        borderWidth: 1,
        width: 100,
    },
});
exports.displayName = (_undefined) => { };
exports.title = 'AsyncStorage Windows';
exports.category = 'Basic';
exports.description = 'Usage of AsyncStorage';
exports.examples = [
    {
        title: 'AsyncStorage',
        render: function () {
            return React.createElement(AsyncStorageExample, null);
        },
    },
];
//# sourceMappingURL=AsyncStorageExampleWindows.js.map