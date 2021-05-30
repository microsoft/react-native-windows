"use strict";
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
exports.examples = exports.description = exports.displayName = exports.title = void 0;
const react_1 = __importDefault(require("react"));
const react_native_1 = require("react-native");
const testImage = require('./img/dpitest.png');
exports.title = 'ImageWin32Test';
exports.displayName = 'Image Win32 test';
exports.description = 'Image Win32 test';
exports.examples = [
    {
        title: 'Win32 Image control test',
        description: 'Test Image',
        render() {
            return (react_1.default.createElement(react_native_1.Image, { style: { width: 100, height: 100 }, source: testImage }));
        },
    },
];
//# sourceMappingURL=ImageWin32Test.js.map