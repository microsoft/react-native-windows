"use strict";
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */
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
var __awaiter = (this && this.__awaiter) || function (thisArg, _arguments, P, generator) {
    function adopt(value) { return value instanceof P ? value : new P(function (resolve) { resolve(value); }); }
    return new (P || (P = Promise))(function (resolve, reject) {
        function fulfilled(value) { try { step(generator.next(value)); } catch (e) { reject(e); } }
        function rejected(value) { try { step(generator["throw"](value)); } catch (e) { reject(e); } }
        function step(result) { result.done ? resolve(result.value) : adopt(result.value).then(fulfilled, rejected); }
        step((generator = generator.apply(thisArg, _arguments || [])).next());
    });
};
var __generator = (this && this.__generator) || function (thisArg, body) {
    var _ = { label: 0, sent: function() { if (t[0] & 1) throw t[1]; return t[1]; }, trys: [], ops: [] }, f, y, t, g;
    return g = { next: verb(0), "throw": verb(1), "return": verb(2) }, typeof Symbol === "function" && (g[Symbol.iterator] = function() { return this; }), g;
    function verb(n) { return function (v) { return step([n, v]); }; }
    function step(op) {
        if (f) throw new TypeError("Generator is already executing.");
        while (_) try {
            if (f = 1, y && (t = op[0] & 2 ? y["return"] : op[0] ? y["throw"] || ((t = y["return"]) && t.call(y), 0) : y.next) && !(t = t.call(y, op[1])).done) return t;
            if (y = 0, t) op = [op[0] & 2, t.value];
            switch (op[0]) {
                case 0: case 1: t = op; break;
                case 4: _.label++; return { value: op[1], done: false };
                case 5: _.label++; y = op[1]; op = [0]; continue;
                case 7: op = _.ops.pop(); _.trys.pop(); continue;
                default:
                    if (!(t = _.trys, t = t.length > 0 && t[t.length - 1]) && (op[0] === 6 || op[0] === 2)) { _ = 0; continue; }
                    if (op[0] === 3 && (!t || (op[1] > t[0] && op[1] < t[3]))) { _.label = op[1]; break; }
                    if (op[0] === 6 && _.label < t[1]) { _.label = t[1]; t = op; break; }
                    if (t && _.label < t[2]) { _.label = t[2]; _.ops.push(op); break; }
                    if (t[2]) _.ops.pop();
                    _.trys.pop(); continue;
            }
            op = body.call(thisArg, _);
        } catch (e) { op = [6, e]; y = 0; } finally { f = t = 0; }
        if (op[0] & 5) throw op[1]; return { value: op[0] ? op[1] : void 0, done: true };
    }
};
var __spreadArrays = (this && this.__spreadArrays) || function () {
    for (var s = 0, i = 0, il = arguments.length; i < il; i++) s += arguments[i].length;
    for (var r = Array(s), k = 0, i = 0; i < il; i++)
        for (var a = arguments[i], j = 0, jl = a.length; j < jl; j++, k++)
            r[k] = a[j];
    return r;
};
Object.defineProperty(exports, "__esModule", { value: true });
exports.examples = exports.description = exports.title = exports.displayName = void 0;
var React = require("react");
var react_native_1 = require("react-native");
var AsyncStorageExample = /** @class */ (function (_super) {
    __extends(AsyncStorageExample, _super);
    function AsyncStorageExample() {
        var _this = _super !== null && _super.apply(this, arguments) || this;
        _this.state = {
            asyncStorageData: [],
            name: '',
            value: '',
        };
        _this.setName = function (name) {
            _this.setState({ name: name });
        };
        _this.setValue = function (value) {
            _this.setState({ value: value });
        };
        _this.onAddEntryPress = function () {
            void react_native_1.AsyncStorage.setItem(_this.state.name, _this.state.value);
            _this.updateAsyncStorageData(_this.state.name, _this.state.value);
        };
        _this.onClearAllKeysPress = function () {
            void react_native_1.AsyncStorage.clear();
            _this.setState({ asyncStorageData: [] });
        };
        return _this;
    }
    AsyncStorageExample.prototype.componentDidMount = function () {
        return __awaiter(this, void 0, void 0, function () {
            var allKeys, values;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, react_native_1.AsyncStorage.getAllKeys()];
                    case 1:
                        allKeys = _a.sent();
                        return [4 /*yield*/, react_native_1.AsyncStorage.multiGet(allKeys)];
                    case 2:
                        values = _a.sent();
                        this.setState({ asyncStorageData: values });
                        return [2 /*return*/];
                }
            });
        });
    };
    AsyncStorageExample.prototype.updateAsyncStorageData = function (key, value) {
        this.setState(function (prevState) {
            var asyncStorageData = __spreadArrays(prevState.asyncStorageData);
            var foundVal = false;
            for (var _i = 0, asyncStorageData_1 = asyncStorageData; _i < asyncStorageData_1.length; _i++) {
                var kvp = asyncStorageData_1[_i];
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
    };
    AsyncStorageExample.prototype.makeOnRemoveEntryPress = function (key) {
        var _this = this;
        return function () {
            void react_native_1.AsyncStorage.removeItem(key);
            _this.setState(function (prevState) {
                var asyncStorageData = prevState.asyncStorageData.filter(function (kvp) { return kvp[0] !== key; });
                return { asyncStorageData: asyncStorageData };
            });
        };
    };
    AsyncStorageExample.prototype.render = function () {
        var _this = this;
        return (React.createElement(react_native_1.View, { style: styles.container },
            React.createElement(react_native_1.Button, { title: "Clear All Keys", onPress: this.onClearAllKeysPress }),
            React.createElement(react_native_1.FlatList, { data: this.state.asyncStorageData, renderItem: function (_a) {
                    var item = _a.item;
                    return (React.createElement(react_native_1.View, { style: styles.keyValue },
                        React.createElement(react_native_1.Text, null,
                            "\"",
                            item[0],
                            "\": \"",
                            item[1],
                            "\""),
                        React.createElement(react_native_1.Button, { title: "Remove", onPress: _this.makeOnRemoveEntryPress(item[0]) })));
                } }),
            React.createElement(react_native_1.Text, null, "Add/Update an entry:"),
            React.createElement(react_native_1.View, { style: styles.keyValue },
                React.createElement(react_native_1.Text, null, "Name:"),
                React.createElement(react_native_1.TextInput, { autoCapitalize: "none", onChangeText: this.setName, placeholder: "<Key>", style: styles.textInput })),
            React.createElement(react_native_1.View, { style: styles.keyValue },
                React.createElement(react_native_1.Text, null, "Value:"),
                React.createElement(react_native_1.TextInput, { autoCapitalize: "none", onChangeText: this.setValue, placeholder: "<New Value>", style: styles.textInput })),
            React.createElement(react_native_1.Button, { title: "Add Entry", onPress: this.onAddEntryPress })));
    };
    return AsyncStorageExample;
}(React.Component));
var styles = react_native_1.StyleSheet.create({
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
exports.displayName = function (_undefined) { };
exports.title = 'AsyncStorage Windows';
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