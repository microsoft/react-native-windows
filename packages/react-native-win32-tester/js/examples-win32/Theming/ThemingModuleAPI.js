'use strict';
Object.defineProperty(exports, "__esModule", { value: true });
exports.examples = exports.description = exports.displayName = exports.title = void 0;
const React = require("react");
const react_native_1 = require("react-native");
const styles = react_native_1.StyleSheet.create({
    box: {
        borderWidth: 2,
        flexDirection: 'column',
        marginVertical: 5,
        backgroundColor: 'whitesmoke',
        justifyContent: 'space-around',
        padding: 5,
        maxHeight: 250,
    },
    header: {
        fontSize: 13,
        margin: 6,
    },
    nestedContainer: {
        margin: 6,
        padding: 6,
    },
});
const Theming = react_native_1.NativeModules.Theming;
const ifModuleAvailable = (wrappedComponent) => {
    return Theming ? wrappedComponent : React.createElement(react_native_1.Text, null, "Theming Native Module not available");
};
const RenderThemeFunction = () => {
    const [, setText] = React.useState('');
    const onChangeText = React.useCallback(t => setText(t), [setText]);
    return (React.createElement(react_native_1.View, { style: styles.nestedContainer },
        React.createElement(react_native_1.Text, null, "Arguments: "),
        React.createElement(react_native_1.TextInput, { onChangeText: onChangeText })));
};
const ThemingMethods = () => {
    return (React.createElement(react_native_1.View, null, Object.keys(Theming).map((val) => {
        return typeof Theming[val] === 'function' ? withBox(val, RenderThemeFunction()) : undefined;
    })));
};
const renderNestedObject = (obj) => {
    return (React.createElement(react_native_1.View, { style: styles.nestedContainer },
        React.createElement(react_native_1.ScrollView, null, Object.keys(obj).map((val) => {
            return React.createElement(react_native_1.Text, { key: val }, val + ': ' + JSON.stringify(obj[val]));
        }))));
};
const renderObject = (obj) => {
    const firstKey = Object.keys(obj)[0];
    const formattedOutput = JSON.stringify(obj)
        .replace(/],/g, '],\n\n')
        .replace(/:/g, ': ')
        .replace(/,/g, ', ');
    return obj[firstKey] instanceof Array ? React.createElement(react_native_1.Text, { style: styles.nestedContainer }, formattedOutput) : renderNestedObject(obj);
};
const renderThemeObject = (key) => withBox(key, renderObject(Theming[key]));
const withBox = (key, component) => (React.createElement(react_native_1.View, { style: styles.box, key: key },
    React.createElement(react_native_1.Text, { style: styles.header }, key),
    component));
const ThemingConstants = () => {
    return (React.createElement(react_native_1.View, null, Object.keys(Theming).map((val) => {
        return typeof Theming[val] === 'object' ? renderThemeObject(val) : undefined;
    })));
};
exports.title = 'Theming Module APIs';
exports.displayName = 'Theming Module APIs';
exports.description = 'Tests shape of Theming Native Module';
exports.examples = [
    {
        title: 'Theming Module Constants',
        description: 'All constants',
        render: () => ifModuleAvailable(React.createElement(ThemingConstants, null)),
    },
    {
        title: 'Theming Module Methods',
        description: 'Method invoker',
        render: () => ifModuleAvailable(React.createElement(ThemingMethods, null)),
    },
];
//# sourceMappingURL=ThemingModuleAPI.js.map