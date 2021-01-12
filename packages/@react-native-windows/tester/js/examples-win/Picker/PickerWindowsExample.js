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
const react_native_windows_1 = require("react-native-windows");
const CAR_MAKES_AND_MODELS = {
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
class PickerExample extends React.Component {
    constructor() {
        super(...arguments);
        this.state = {
            carMake: 'cadillac',
            modelIndex: 3,
        };
    }
    render() {
        const make = CAR_MAKES_AND_MODELS[this.state.carMake];
        const selectionString = make.name + ' ' + make.models[this.state.modelIndex];
        return (React.createElement(react_native_1.View, null,
            React.createElement(react_native_1.Text, null, "Please choose a make for your car:"),
            React.createElement(react_native_windows_1.Picker, { selectedValue: this.state.carMake, onValueChange: (carMake) => this.setState({ carMake, modelIndex: 0 }) }, Object.keys(CAR_MAKES_AND_MODELS).map(carMake => (React.createElement(react_native_windows_1.Picker.Item, { key: carMake, value: carMake, label: CAR_MAKES_AND_MODELS[carMake].name })))),
            React.createElement(react_native_1.Text, null,
                "Please choose a model of ",
                make.name,
                ":"),
            React.createElement(react_native_windows_1.Picker, { selectedValue: this.state.modelIndex, key: this.state.carMake, onValueChange: (modelIndex) => this.setState({ modelIndex }) }, CAR_MAKES_AND_MODELS[this.state.carMake].models.map((modelName, modelIndex) => (React.createElement(react_native_windows_1.Picker.Item, { key: this.state.carMake + '_' + modelIndex, value: modelIndex, label: modelName })))),
            React.createElement(react_native_1.Text, null,
                "You selected: ",
                selectionString)));
    }
}
class PickerStyleExample extends React.Component {
    constructor() {
        super(...arguments);
        this.state = {
            carMake: 'cadillac',
            modelIndex: 0,
        };
    }
    render() {
        /*
            itemStyle={{
              fontSize: 11,
              color: 'red',
              textAlign: 'left',
              fontWeight: 'bold',
            }}
          */
        return (React.createElement(react_native_windows_1.Picker, { selectedValue: this.state.carMake, onValueChange: (carMake) => this.setState({ carMake, modelIndex: 0 }) }, Object.keys(CAR_MAKES_AND_MODELS).map(carMake => (React.createElement(react_native_windows_1.Picker.Item, { key: carMake, value: carMake, label: CAR_MAKES_AND_MODELS[carMake].name })))));
    }
}
class PickerUpdateItemsExample extends React.Component {
    constructor() {
        super(...arguments);
        this.state = {
            selected: '111',
            items: ['111', '222', '333', '444', '555', '666'],
        };
    }
    render() {
        return (React.createElement(react_native_1.View, null,
            React.createElement(react_native_1.View, { style: { flexDirection: 'row', justifyContent: 'space-between' } },
                React.createElement(react_native_1.Button, { title: "Remove first item", onPress: () => {
                        let selected = this.state.selected;
                        if (this.state.items.length > 1 &&
                            this.state.items[0] === selected) {
                            selected = this.state.items[1];
                        }
                        this.setState({ items: this.state.items.slice(1), selected });
                    } }),
                React.createElement(react_native_1.Button, { title: "Remove last item", onPress: () => {
                        let selected = this.state.selected;
                        if (this.state.items.length > 1 &&
                            this.state.items[this.state.items.length - 1] === selected) {
                            selected = this.state.items[this.state.items.length - 2];
                        }
                        this.setState({
                            items: this.state.items.slice(0, this.state.items.length - 1),
                            selected,
                        });
                    } }),
                React.createElement(react_native_1.Button, { title: "Reset", onPress: () => this.setState({
                        selected: '111',
                        items: ['111', '222', '333', '444', '555', '666'],
                    }) })),
            React.createElement(react_native_windows_1.Picker, { selectedValue: this.state.selected, onValueChange: (selected) => this.setState({ selected }) }, this.state.items.map(item => (React.createElement(react_native_windows_1.Picker.Item, { key: item, value: item, label: item })))),
            React.createElement(react_native_1.Text, null,
                "You selected: ",
                this.state.selected)));
    }
}
class PickerEditableExample extends React.Component {
    constructor() {
        super(...arguments);
        this.state = {
            selected: '111',
            text: 'n',
            items: ['111', '222', '333', '444', '555', '666'],
        };
        this.onValueChange = (selected, _index, text) => {
            this.setState({ selected, text });
        };
    }
    render() {
        return (React.createElement(react_native_1.View, { style: { flexDirection: 'column' } },
            React.createElement(react_native_1.Text, null,
                "You selected:",
                this.state.selected),
            React.createElement(react_native_1.Text, null,
                "Text change:",
                this.state.text),
            React.createElement(react_native_windows_1.Picker, { editable: true, selectedValue: this.state.selected, text: this.state.text, onValueChange: this.onValueChange }, this.state.items.map(item => (React.createElement(react_native_windows_1.Picker.Item, { key: item, value: item, label: item }))))));
    }
}
exports.displayName = (_undefined) => { };
exports.title = 'PickerWindows';
exports.category = 'UI';
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