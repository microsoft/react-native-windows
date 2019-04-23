// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
'use strict';
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
var React = require("react");
var react_native_1 = require("react-native");
var RNTesterActions = require('react-native/RNTester/js/RNTesterActions');
var RNTesterExampleContainer = require('react-native/RNTester/js/RNTesterExampleContainer');
var RNTesterExampleList = require('react-native/RNTester/js/RNTesterExampleList');
var RNTesterList_uwp_1 = require("./RNTesterList.uwp");
var RNTesterNavigationReducer_1 = require("./RNTesterNavigationReducer");
var styles = react_native_1.StyleSheet.create({
    headerContainer: {
        borderBottomWidth: react_native_1.StyleSheet.hairlineWidth,
        borderBottomColor: '#96969A',
        backgroundColor: '#F5F5F6'
    },
    header: {
        height: 40,
        flexDirection: 'row'
    },
    headerLeft: {},
    headerCenter: {
        flex: 1,
        position: 'absolute',
        top: 7,
        left: 0,
        right: 0
    },
    title: {
        fontSize: 19,
        fontWeight: '600',
        textAlign: 'center'
    },
    exampleContainer: {
        flex: 1
    }
});
var APP_STATE_KEY = 'RNTesterAppState.v2';
var Header = function (_a) {
    var onBack = _a.onBack, title = _a.title;
    return (React.createElement(react_native_1.View, { style: styles.headerContainer },
        React.createElement(react_native_1.View, { style: styles.header },
            React.createElement(react_native_1.View, { style: styles.headerCenter },
                React.createElement(react_native_1.Text, { style: styles.title }, title)),
            onBack && (React.createElement(react_native_1.View, { style: styles.headerLeft },
                React.createElement(react_native_1.Button, { title: "Back", onPress: onBack }))))));
};
var RNTesterApp = /** @class */ (function (_super) {
    __extends(RNTesterApp, _super);
    function RNTesterApp() {
        var _this = _super !== null && _super.apply(this, arguments) || this;
        _this._handleBack = function () {
            _this._handleAction(RNTesterActions.Back());
        };
        _this._handleAction = function (action) {
            if (!action) {
                return;
            }
            var newState = RNTesterNavigationReducer_1.default(_this.state, action);
            if (_this.state !== newState) {
                _this.setState(newState, function () { return react_native_1.AsyncStorage.setItem(APP_STATE_KEY, JSON.stringify(_this.state)); });
            }
        };
        return _this;
    }
    RNTesterApp.prototype.render = function () {
        if (!this.state) {
            return React.createElement(react_native_1.Text, null, "null state");
        }
        if (this.state.openExample) {
            var Component = RNTesterList_uwp_1.default.Modules[this.state.openExample];
            if (Component.external) {
                // tslint:disable-next-line:no-any
                var Comp = Component;
                return React.createElement(Comp, { onExampleExit: this._handleBack });
            }
            else {
                return (React.createElement(react_native_1.View, { style: styles.exampleContainer },
                    React.createElement(Header, { onBack: this._handleBack, title: Component.title }),
                    React.createElement(RNTesterExampleContainer, { module: Component })));
            }
        }
        return (React.createElement(react_native_1.View, { style: styles.exampleContainer },
            React.createElement(Header, { title: "RNTester" }),
            React.createElement(RNTesterExampleList, { onNavigate: this._handleAction, list: RNTesterList_uwp_1.default })));
    };
    /*
    UNSAFE_componentWillMount() {
      BackHandler.addEventListener('hardwareBackPress', this._handleBack);
    }
    */
    RNTesterApp.prototype.componentDidMount = function () {
        /*
        Linking.getInitialURL().then((url) => {
          AsyncStorage.getItem(APP_STATE_KEY, (err, storedString) => {
            const exampleAction = URIActionMap(this.props.exampleFromAppetizeParams);
            const urlAction = URIActionMap(url);
            const launchAction = exampleAction || urlAction;
         if (err || !storedString) {
          */
        var initialAction = { type: 'InitialAction' };
        this.setState(RNTesterNavigationReducer_1.default(undefined, initialAction));
        return;
        /*
            }
            const storedState = JSON.parse(storedString);
            if (launchAction) {
              this.setState(RNTesterNavigationReducer(storedState, launchAction));
              return;
            }
            this.setState(storedState);
          });
        });
    
        Linking.addEventListener('url', (url) => {
          this._handleAction(URIActionMap(url));
        });
        */
    };
    return RNTesterApp;
}(React.Component));
react_native_1.AppRegistry.registerComponent('RNTesterApp', function () { return RNTesterApp; });
module.exports = RNTesterApp;
//# sourceMappingURL=RNTesterApp.uwp.js.map