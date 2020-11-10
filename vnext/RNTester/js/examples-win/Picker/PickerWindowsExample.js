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
Object.defineProperty(exports, "__esModule", { value: true });
exports.examples = exports.description = exports.title = exports.displayName = void 0;
var React = require("react");
var react_native_1 = require("react-native");
var PickerWindows_1 = require("../../../../Libraries/Components/Picker/PickerWindows");
var CAR_MAKES_AND_MODELS = {
    amc: {
        name: 'AMC',
        models: ['AMX', 'Concord', 'Eagle', 'Gremlin', 'Matador', 'Pacer'],
    },
    alfa: {
        name: 'Alfa-Romeo',
        models: [
            '159',
            '4C',
            'Alfasud',
            'Brera',
            'GTV6',
            'Giulia',
            'MiTo',
            'Spider',
        ],
    },
    aston: {
        name: 'Aston Martin',
        models: ['DB5', 'DB9', 'DBS', 'Rapide', 'Vanquish', 'Vantage'],
    },
    audi: {
        name: 'Audi',
        models: [
            '90',
            '4000',
            '5000',
            'A3',
            'A4',
            'A5',
            'A6',
            'A7',
            'A8',
            'Q5',
            'Q7',
        ],
    },
    austin: {
        name: 'Austin',
        models: ['America', 'Maestro', 'Maxi', 'Mini', 'Montego', 'Princess'],
    },
    borgward: {
        name: 'Borgward',
        models: ['Hansa', 'Isabella', 'P100'],
    },
    buick: {
        name: 'Buick',
        models: [
            'Electra',
            'LaCrosse',
            'LeSabre',
            'Park Avenue',
            'Regal',
            'Roadmaster',
            'Skylark',
        ],
    },
    cadillac: {
        name: 'Cadillac',
        models: ['Catera', 'Cimarron', 'Eldorado', 'Fleetwood', 'Sedan de Ville'],
    },
    chevrolet: {
        name: 'Chevrolet',
        models: [
            'Astro',
            'Aveo',
            'Bel Air',
            'Captiva',
            'Cavalier',
            'Chevelle',
            'Corvair',
            'Corvette',
            'Cruze',
            'Nova',
            'SS',
            'Vega',
            'Volt',
        ],
    },
};
var PickerExample = /** @class */ (function (_super) {
    __extends(PickerExample, _super);
    function PickerExample() {
        var _this = _super !== null && _super.apply(this, arguments) || this;
        _this.state = {
            carMake: 'cadillac',
            modelIndex: 3,
        };
        return _this;
    }
    PickerExample.prototype.render = function () {
        var _this = this;
        var make = CAR_MAKES_AND_MODELS[this.state.carMake];
        var selectionString = make.name + ' ' + make.models[this.state.modelIndex];
        return (React.createElement(react_native_1.View, null,
            React.createElement(react_native_1.Text, null, "Please choose a make for your car:"),
            React.createElement(PickerWindows_1.Picker, { selectedValue: this.state.carMake, onValueChange: function (carMake) {
                    return _this.setState({ carMake: carMake, modelIndex: 0 });
                } }, Object.keys(CAR_MAKES_AND_MODELS).map(function (carMake) { return (React.createElement(PickerWindows_1.Picker.Item, { key: carMake, value: carMake, label: CAR_MAKES_AND_MODELS[carMake].name })); })),
            React.createElement(react_native_1.Text, null,
                "Please choose a model of ",
                make.name,
                ":"),
            React.createElement(PickerWindows_1.Picker, { selectedValue: this.state.modelIndex, key: this.state.carMake, onValueChange: function (modelIndex) { return _this.setState({ modelIndex: modelIndex }); } }, CAR_MAKES_AND_MODELS[this.state.carMake].models.map(function (modelName, modelIndex) { return (React.createElement(PickerWindows_1.Picker.Item, { key: _this.state.carMake + '_' + modelIndex, value: modelIndex, label: modelName })); })),
            React.createElement(react_native_1.Text, null,
                "You selected: ",
                selectionString)));
    };
    return PickerExample;
}(React.Component));
var PickerStyleExample = /** @class */ (function (_super) {
    __extends(PickerStyleExample, _super);
    function PickerStyleExample() {
        var _this = _super !== null && _super.apply(this, arguments) || this;
        _this.state = {
            carMake: 'cadillac',
            modelIndex: 0,
        };
        return _this;
    }
    PickerStyleExample.prototype.render = function () {
        var _this = this;
        /*
            itemStyle={{
              fontSize: 11,
              color: 'red',
              textAlign: 'left',
              fontWeight: 'bold',
            }}
          */
        return (React.createElement(PickerWindows_1.Picker, { selectedValue: this.state.carMake, onValueChange: function (carMake) {
                return _this.setState({ carMake: carMake, modelIndex: 0 });
            } }, Object.keys(CAR_MAKES_AND_MODELS).map(function (carMake) { return (React.createElement(PickerWindows_1.Picker.Item, { key: carMake, value: carMake, label: CAR_MAKES_AND_MODELS[carMake].name })); })));
    };
    return PickerStyleExample;
}(React.Component));
var PickerUpdateItemsExample = /** @class */ (function (_super) {
    __extends(PickerUpdateItemsExample, _super);
    function PickerUpdateItemsExample() {
        var _this = _super !== null && _super.apply(this, arguments) || this;
        _this.state = {
            selected: '111',
            items: ['111', '222', '333', '444', '555', '666'],
        };
        return _this;
    }
    PickerUpdateItemsExample.prototype.render = function () {
        var _this = this;
        return (React.createElement(react_native_1.View, null,
            React.createElement(react_native_1.View, { style: { flexDirection: 'row', justifyContent: 'space-between' } },
                React.createElement(react_native_1.Button, { title: "Remove first item", onPress: function () {
                        var selected = _this.state.selected;
                        if (_this.state.items.length > 1 &&
                            _this.state.items[0] === selected) {
                            selected = _this.state.items[1];
                        }
                        _this.setState({ items: _this.state.items.slice(1), selected: selected });
                    } }),
                React.createElement(react_native_1.Button, { title: "Remove last item", onPress: function () {
                        var selected = _this.state.selected;
                        if (_this.state.items.length > 1 &&
                            _this.state.items[_this.state.items.length - 1] === selected) {
                            selected = _this.state.items[_this.state.items.length - 2];
                        }
                        _this.setState({
                            items: _this.state.items.slice(0, _this.state.items.length - 1),
                            selected: selected,
                        });
                    } }),
                React.createElement(react_native_1.Button, { title: "Reset", onPress: function () {
                        return _this.setState({
                            selected: '111',
                            items: ['111', '222', '333', '444', '555', '666'],
                        });
                    } })),
            React.createElement(PickerWindows_1.Picker, { selectedValue: this.state.selected, onValueChange: function (selected) { return _this.setState({ selected: selected }); } }, this.state.items.map(function (item) { return (React.createElement(PickerWindows_1.Picker.Item, { key: item, value: item, label: item })); })),
            React.createElement(react_native_1.Text, null,
                "You selected: ",
                this.state.selected)));
    };
    return PickerUpdateItemsExample;
}(React.Component));
var PickerEditableExample = /** @class */ (function (_super) {
    __extends(PickerEditableExample, _super);
    function PickerEditableExample() {
        var _this = _super !== null && _super.apply(this, arguments) || this;
        _this.state = {
            selected: '111',
            text: 'n',
            items: ['111', '222', '333', '444', '555', '666'],
        };
        _this.onValueChange = function (selected, _index, text) {
            _this.setState({ selected: selected, text: text });
        };
        return _this;
    }
    PickerEditableExample.prototype.render = function () {
        return (React.createElement(react_native_1.View, { style: { flexDirection: 'column' } },
            React.createElement(react_native_1.Text, null,
                "You selected:",
                this.state.selected),
            React.createElement(react_native_1.Text, null,
                "Text change:",
                this.state.text),
            React.createElement(PickerWindows_1.Picker, { editable: true, selectedValue: this.state.selected, text: this.state.text, onValueChange: this.onValueChange }, this.state.items.map(function (item) { return (React.createElement(PickerWindows_1.Picker.Item, { key: item, value: item, label: item })); }))));
    };
    return PickerEditableExample;
}(React.Component));
exports.displayName = function (_undefined) { };
exports.title = '<PickerWindows>';
exports.description = 'Render lists of selectable options with UWP ComboBox.';
exports.examples = [
    {
        title: '<Picker>',
        render: function () {
            return React.createElement(PickerExample, null);
        },
    },
    {
        title: '<Picker> with custom styling (Not yet implemented)',
        render: function () {
            return React.createElement(PickerStyleExample, null);
        },
    },
    {
        title: '<Picker> update items maintains selection test',
        render: function () {
            return React.createElement(PickerUpdateItemsExample, null);
        },
    },
    {
        title: '<Picker> editable combobox example',
        render: function () {
            return React.createElement(PickerEditableExample, null);
        },
    },
];
//# sourceMappingURL=PickerWindowsExample.js.map