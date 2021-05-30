"use strict";
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
exports.TextWin32 = void 0;
const react_1 = __importDefault(require("react"));
const RN = require("react-native");
class TextWin32 extends react_1.default.Component {
    constructor(props) {
        super(props);
    }
    render() {
        return react_1.default.createElement(RN.Text, Object.assign({}, this.props));
    }
}
exports.TextWin32 = TextWin32;
//# sourceMappingURL=TextWin32.js.map