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
Object.defineProperty(exports, "__esModule", { value: true });
exports.examples = exports.description = exports.title = exports.displayName = void 0;
var React = require("react");
var react_native_1 = require("react-native");
var AppTheme_1 = require("../../../../Libraries/AppTheme/AppTheme");
var AccessibilityBaseExample = /** @class */ (function (_super) {
    __extends(AccessibilityBaseExample, _super);
    function AccessibilityBaseExample() {
        return _super !== null && _super.apply(this, arguments) || this;
    }
    AccessibilityBaseExample.prototype.render = function () {
        return (React.createElement(react_native_1.View, null,
            React.createElement(react_native_1.Text, null, "The following has accessibilityLabel and accessibilityHint:"),
            React.createElement(react_native_1.View, { style: { width: 50, height: 50, backgroundColor: 'blue' }, accessibilityLabel: "A blue box", accessibilityHint: "A hint for the blue box." }),
            React.createElement(react_native_1.Text, null, "The following has accessible and accessibilityLabel:"),
            React.createElement(react_native_1.View, { style: { width: 50, height: 50, backgroundColor: 'red' }, accessible: true, accessibilityLabel: "A hint for the red box." })));
    };
    return AccessibilityBaseExample;
}(React.Component));
var HighContrastExample = /** @class */ (function (_super) {
    __extends(HighContrastExample, _super);
    function HighContrastExample() {
        var _this = _super !== null && _super.apply(this, arguments) || this;
        _this.state = {
            isHighContrast: AppTheme_1.AppTheme.isHighContrast,
            highContrastColorValues: AppTheme_1.AppTheme.currentHighContrastColors,
        };
        // TODO: Make args props
        _this.onHighContrastChanged = function (event) {
            _this.setState({
                isHighContrast: event.isHighContrast,
                highContrastColorValues: event.highContrastColors,
            });
        };
        _this.styles = react_native_1.StyleSheet.create({
            enabled: {
                width: 250,
                height: 50,
            },
            disabled: {
                width: 250,
                height: 50,
                backgroundColor: '#808080',
            },
        });
        return _this;
    }
    HighContrastExample.prototype.componentDidMount = function () {
        AppTheme_1.AppTheme.addListener('highContrastChanged', this.onHighContrastChanged);
    };
    HighContrastExample.prototype.componenetWillUnmount = function () {
        AppTheme_1.AppTheme.removeListener('highContrastChanged', this.onHighContrastChanged);
    };
    HighContrastExample.prototype.render = function () {
        return (React.createElement(react_native_1.View, null,
            React.createElement(react_native_1.Text, null, "The following has HighContrast Event awareness:"),
            React.createElement(react_native_1.View, null,
                React.createElement(react_native_1.Text, null,
                    "isHighContrast: ",
                    this.state.isHighContrast ? 'True' : 'False')),
            React.createElement(react_native_1.View, { style: this.state.isHighContrast
                    ? [
                        this.styles.enabled,
                        {
                            backgroundColor: this.state.highContrastColorValues
                                .ButtonFaceColor,
                        },
                    ]
                    : this.styles.disabled },
                React.createElement(react_native_1.Text, null,
                    "ButtonFace High Contrast Hex Value:",
                    ' ',
                    this.state.highContrastColorValues.ButtonFaceColor)),
            React.createElement(react_native_1.View, { style: this.state.isHighContrast
                    ? [
                        this.styles.enabled,
                        {
                            backgroundColor: this.state.highContrastColorValues
                                .ButtonTextColor,
                        },
                    ]
                    : this.styles.disabled },
                React.createElement(react_native_1.Text, null,
                    "ButtonText High Contrast Color Hex Value:",
                    ' ',
                    this.state.highContrastColorValues.ButtonTextColor)),
            React.createElement(react_native_1.View, { style: this.state.isHighContrast
                    ? [
                        this.styles.enabled,
                        {
                            backgroundColor: this.state.highContrastColorValues
                                .GrayTextColor,
                        },
                    ]
                    : this.styles.disabled },
                React.createElement(react_native_1.Text, null,
                    "GrayText High Contrast Color Hex Value:",
                    ' ',
                    this.state.highContrastColorValues.GrayTextColor)),
            React.createElement(react_native_1.View, { style: this.state.isHighContrast
                    ? [
                        this.styles.enabled,
                        {
                            backgroundColor: this.state.highContrastColorValues
                                .HighlightColor,
                        },
                    ]
                    : this.styles.disabled },
                React.createElement(react_native_1.Text, null,
                    "Highlight High Contrast Color Hex Value:",
                    ' ',
                    this.state.highContrastColorValues.HighlightColor)),
            React.createElement(react_native_1.View, { style: this.state.isHighContrast
                    ? [
                        this.styles.enabled,
                        {
                            backgroundColor: this.state.highContrastColorValues
                                .HighlightTextColor,
                        },
                    ]
                    : this.styles.disabled },
                React.createElement(react_native_1.Text, null,
                    "HighlightText High Contrast Color Hex Value:",
                    ' ',
                    this.state.highContrastColorValues.HighlightTextColor)),
            React.createElement(react_native_1.View, { style: this.state.isHighContrast
                    ? [
                        this.styles.enabled,
                        {
                            backgroundColor: this.state.highContrastColorValues
                                .HotlightColor,
                        },
                    ]
                    : this.styles.disabled },
                React.createElement(react_native_1.Text, null,
                    "Hotlight High Contrast Color Hex Value:",
                    ' ',
                    this.state.highContrastColorValues.HotlightColor)),
            React.createElement(react_native_1.View, { style: this.state.isHighContrast
                    ? [
                        this.styles.enabled,
                        {
                            backgroundColor: this.state.highContrastColorValues
                                .WindowColor,
                        },
                    ]
                    : this.styles.disabled },
                React.createElement(react_native_1.Text, null,
                    "Window High Contrast Color Hex Value:",
                    ' ',
                    this.state.highContrastColorValues.WindowColor)),
            React.createElement(react_native_1.View, { style: this.state.isHighContrast
                    ? [
                        this.styles.enabled,
                        {
                            backgroundColor: this.state.highContrastColorValues
                                .WindowTextColor,
                        },
                    ]
                    : this.styles.disabled },
                React.createElement(react_native_1.Text, null,
                    "WindowText High Contrast Color Hex Value:",
                    ' ',
                    this.state.highContrastColorValues.WindowTextColor))));
    };
    return HighContrastExample;
}(React.Component));
var TouchableExamples = /** @class */ (function (_super) {
    __extends(TouchableExamples, _super);
    function TouchableExamples() {
        var _this = _super !== null && _super.apply(this, arguments) || this;
        _this.state = {
            pressedCount: 0,
        };
        _this.press = function () {
            _this.setState({ pressedCount: _this.state.pressedCount + 1 });
        };
        return _this;
    }
    TouchableExamples.prototype.render = function () {
        return (React.createElement(react_native_1.View, null,
            React.createElement(react_native_1.Text, null, "The following TouchableHighlight has accessibilityLabel, accessibilityHint, accessibilityRole, toolip:"),
            React.createElement(react_native_1.TouchableHighlight, __assign({ style: { width: 50, height: 50, backgroundColor: 'blue' }, accessibilityLabel: "A blue box", accessibilityHint: "A hint for the blue box.", accessibilityRole: "button", onPress: this.press }, { tooltip: 'a blue tooltip' }),
                React.createElement(react_native_1.Text, null, "Blue")),
            React.createElement(react_native_1.Text, { accessibilityLiveRegion: "polite", accessibilityLabel: 'Pressed ' + this.state.pressedCount + ' times' },
                "Pressed ",
                this.state.pressedCount,
                " times")));
    };
    return TouchableExamples;
}(React.Component));
var AccessibilityStateExamples = /** @class */ (function (_super) {
    __extends(AccessibilityStateExamples, _super);
    function AccessibilityStateExamples() {
        var _this = _super !== null && _super.apply(this, arguments) || this;
        _this.state = {
            viewDisabled: false,
            itemsSelected: [false, false, false],
            viewChecked: 0,
            viewBusy: false,
            viewCollapsed: false,
            viewRangeNow: 10,
            viewRangeMax: 125,
            viewRangeMin: 5,
            viewValueText: 'testText',
        };
        _this.disablePress = function () {
            _this.setState({ viewDisabled: !_this.state.viewDisabled });
        };
        _this.selectPress = function (index) {
            var tmp = _this.state.itemsSelected;
            tmp[index] = !tmp[index];
            _this.setState({ itemsSelected: tmp });
        };
        _this.checkedPress = function () {
            var newChecked = _this.state.viewChecked + 1;
            if (newChecked === 3) {
                newChecked = 0;
            }
            _this.setState({ viewChecked: newChecked });
        };
        _this.busyPress = function () {
            _this.setState({ viewBusy: !_this.state.viewBusy });
        };
        _this.collapsePress = function () {
            _this.setState({ viewCollapsed: !_this.state.viewCollapsed });
        };
        _this.rangePress = function () {
            _this.setState({ viewRangeNow: _this.state.viewRangeNow + 1 });
        };
        return _this;
    }
    AccessibilityStateExamples.prototype.render = function () {
        var _this = this;
        var selectableItems = [{}, {}, {}];
        return (React.createElement(react_native_1.View, null,
            React.createElement(react_native_1.Text, null, "The following TouchableHighlight toggles accessibilityState.disabled for the View under it:"),
            React.createElement(react_native_1.TouchableHighlight, { style: { width: 100, height: 50, backgroundColor: 'blue' }, accessibilityRole: "button", onPress: this.disablePress },
                React.createElement(react_native_1.Text, null, "Toggle")),
            React.createElement(react_native_1.View, { style: {
                    backgroundColor: this.state.viewDisabled ? 'gray' : 'lightskyblue',
                }, accessibilityRole: "none", accessibilityState: { disabled: this.state.viewDisabled } },
                React.createElement(react_native_1.Text, null,
                    "This View should be",
                    ' ',
                    this.state.viewDisabled ? 'disabled' : 'enabled',
                    " according to UIA")),
            React.createElement(react_native_1.Text, null, "The following list of TouchableHighlights toggles accessibilityState.selected when touched:"),
            React.createElement(react_native_1.FlatList, { accessibilityLabel: "List of selectable items", data: selectableItems, renderItem: function (item) { return (React.createElement(react_native_1.TouchableHighlight, { style: {
                        width: 100,
                        height: 50,
                        backgroundColor: _this.state.itemsSelected[item.index]
                            ? 'gray'
                            : 'lightskyblue',
                    }, accessibilityRole: "button", accessibilityLabel: 'Selectable item ' + (item.index + 1), accessibilityState: {
                        selected: _this.state.itemsSelected[item.index],
                    }, onPress: function () { return _this.selectPress(item.index); } },
                    React.createElement(react_native_1.Text, null, _this.state.itemsSelected[item.index]
                        ? 'Selected'
                        : 'Unselected'))); }, keyExtractor: function (item, index) { return index.toString(); } }),
            React.createElement(react_native_1.Text, null, "The following TouchableHighlight cycles accessibilityState.checked through unchecked/checked/mixed for the View under it:"),
            React.createElement(react_native_1.TouchableHighlight, { style: { width: 100, height: 50, backgroundColor: 'blue' }, accessibilityRole: "button", onPress: this.checkedPress },
                React.createElement(react_native_1.Text, null, "Toggle")),
            React.createElement(react_native_1.View, { style: {
                    backgroundColor: this.state.viewChecked === 0
                        ? 'green'
                        : this.state.viewChecked === 1
                            ? 'gray'
                            : 'lightskyblue',
                }, 
                //@ts-ignore
                accessibilityRole: "checkbox", 
                //@ts-ignore
                accessibilityState: {
                    checked: this.state.viewChecked === 0
                        ? false
                        : this.state.viewChecked === 1
                            ? true
                            : 'mixed',
                } },
                React.createElement(react_native_1.Text, null,
                    "This View should be",
                    ' ',
                    this.state.viewChecked === 0
                        ? 'Unchecked'
                        : this.state.viewChecked === 1
                            ? 'Checked'
                            : 'Mixed',
                    ' ',
                    "according to UIA")),
            React.createElement(react_native_1.Text, null, "The following TouchableHighlight toggles the acessibilityState.busy for the View under it:"),
            React.createElement(react_native_1.TouchableHighlight, { style: { width: 100, height: 50, backgroundColor: 'blue' }, accessibilityRole: "button", onPress: this.busyPress },
                React.createElement(react_native_1.Text, null, "Toggle")),
            React.createElement(react_native_1.View, { style: {
                    backgroundColor: this.state.viewBusy ? 'gray' : 'lightskyblue',
                }, accessibilityRole: "none", 
                //@ts-ignore
                accessibilityState: { busy: this.state.viewBusy } },
                React.createElement(react_native_1.Text, null,
                    "This View should be ",
                    this.state.viewBusy ? 'Busy' : 'Not Busy',
                    ' ',
                    "according to UIA")),
            React.createElement(react_native_1.Text, null, "The following TouchableHighlight toggles accessibilityState.expanded and accessibilityState.collapsed for the View under it:"),
            React.createElement(react_native_1.TouchableHighlight, { style: { width: 100, height: 50, backgroundColor: 'blue' }, accessibilityRole: "button", onPress: this.collapsePress },
                React.createElement(react_native_1.Text, null, "Toggle")),
            React.createElement(react_native_1.View, { style: {
                    backgroundColor: this.state.viewCollapsed ? 'gray' : 'lightskyblue',
                    height: this.state.viewCollapsed ? 25 : 50,
                }, accessibilityRole: "none", 
                //@ts-ignore
                accessibilityState: {
                    expanded: !this.state.viewCollapsed,
                } },
                React.createElement(react_native_1.Text, null,
                    "This View should be",
                    ' ',
                    this.state.viewCollapsed ? 'Collapsed' : 'Expanded',
                    " according to UIA")),
            React.createElement(react_native_1.Text, null, "The following View exposes accessibilityValue fields (min, max, now)"),
            React.createElement(react_native_1.TouchableHighlight, { style: { width: 100, height: 50, backgroundColor: 'blue' }, onPress: this.rangePress },
                React.createElement(react_native_1.Text, null, "Range value increment")),
            React.createElement(react_native_1.View, { style: {
                    backgroundColor: 'gray',
                    height: 50,
                }, accessibilityValue: {
                    min: this.state.viewRangeMin,
                    max: this.state.viewRangeMax,
                    now: this.state.viewRangeNow,
                }, accessibilityRole: "adjustable" },
                React.createElement(react_native_1.Text, null,
                    "The View's (accessibilityRole == adjustable, ie. Slider) properties should be the following according to UIA: Min-",
                    ' ',
                    this.state.viewRangeMin,
                    "Max- ",
                    this.state.viewRangeMax,
                    "Now- ",
                    this.state.viewRangeNow)),
            React.createElement(react_native_1.Text, null, "The following View exposes the accessibilityValue.Text field"),
            React.createElement(react_native_1.View, { style: {
                    backgroundColor: 'gray',
                    height: 50,
                }, accessibilityValue: {
                    text: this.state.viewValueText,
                }, accessibilityRole: "combobox" },
                React.createElement(react_native_1.Text, null,
                    "The View's properties should be the following according to UIA: Text- ",
                    this.state.viewValueText))));
    };
    return AccessibilityStateExamples;
}(React.Component));
var AccessibilityListExamples = /** @class */ (function (_super) {
    __extends(AccessibilityListExamples, _super);
    function AccessibilityListExamples() {
        return _super !== null && _super.apply(this, arguments) || this;
    }
    AccessibilityListExamples.prototype.render = function () {
        var items = [{}, {}, {}];
        return (React.createElement(react_native_1.View, null,
            React.createElement(react_native_1.Text, null, "The following uses accessibilityRole: 'list', 'listitem', accessibilitySetSize, and accessibilityPosInSet."),
            React.createElement(react_native_1.View
            //@ts-ignore
            , { 
                //@ts-ignore
                accessibilityRole: "list" },
                React.createElement(react_native_1.FlatList, { data: items, renderItem: function (item) { return (React.createElement(react_native_1.View, { style: {
                            width: 100,
                            height: 50,
                            backgroundColor: 'lightskyblue',
                        }, 
                        //@ts-ignore
                        accessibilityRole: "listitem", accessibilitySetSize: items.length, accessibilityPosInSet: item.index + 1 },
                        React.createElement(react_native_1.Text, null,
                            "Item ",
                            item.index + 1))); }, keyExtractor: function (item, index) { return index.toString(); } })),
            React.createElement(react_native_1.View
            //@ts-ignore
            , { 
                //@ts-ignore
                accessibilityRole: "list" },
                React.createElement(react_native_1.Text, null, "The following does the same, but with Touchables."),
                React.createElement(react_native_1.FlatList, { data: items, renderItem: function (item) { return (React.createElement(react_native_1.TouchableHighlight, { style: {
                            width: 100,
                            height: 50,
                            backgroundColor: 'lightskyblue',
                        }, 
                        //@ts-ignore
                        accessibilityRole: "listitem", accessibilitySetSize: items.length, accessibilityPosInSet: item.index + 1 },
                        React.createElement(react_native_1.Text, null,
                            "Touchable ",
                            item.index + 1))); }, keyExtractor: function (item, index) { return index.toString(); } }))));
    };
    return AccessibilityListExamples;
}(React.Component));
exports.displayName = function (_undefined) { };
exports.title = 'Accessibility Windows';
exports.description = 'Usage of accessibility properties.';
exports.examples = [
    {
        title: 'Label, Hint',
        render: function () {
            return React.createElement(AccessibilityBaseExample, null);
        },
    },
    {
        title: 'Touchables',
        render: function () {
            return React.createElement(TouchableExamples, null);
        },
    },
    {
        title: 'HighContrast',
        render: function () {
            return React.createElement(HighContrastExample, null);
        },
    },
    {
        title: 'States',
        render: function () {
            return React.createElement(AccessibilityStateExamples, null);
        },
    },
    {
        title: 'Lists',
        render: function () {
            return React.createElement(AccessibilityListExamples, null);
        },
    },
];
//# sourceMappingURL=AccessibilityExampleWindows.js.map