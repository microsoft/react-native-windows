'use strict';
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
exports.PersonaCoin = void 0;
const react_1 = __importDefault(require("react"));
const react_native_1 = require("react-native");
const PersonaCoinPropTypes_1 = require("./PersonaCoinPropTypes");
const RCTPersonaCoin = react_native_1.requireNativeComponent('RCTPersonaCoin');
exports.PersonaCoin = (props) => react_1.default.createElement(RCTPersonaCoin, Object.assign({}, props));
exports.PersonaCoin.defaultProps = PersonaCoinPropTypes_1.PersonaCoinDefaultProps;
exports.PersonaCoin.displayName = 'PersonaCoin';
//# sourceMappingURL=PersonaCoin.js.map