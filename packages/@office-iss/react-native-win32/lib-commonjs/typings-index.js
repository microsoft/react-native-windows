"use strict";
/**
 * @packagedocumentation
 *
 * This package provides Win32 specific components and provides JS implementations for some react-native primitives
 * Cross platform React-native primitives should be imported from 'react-native'
 * Win32 specific components need to be imported from '\@office-iss/react-native-win32'
 *
 */
var __createBinding = (this && this.__createBinding) || (Object.create ? (function(o, m, k, k2) {
    if (k2 === undefined) k2 = k;
    Object.defineProperty(o, k2, { enumerable: true, get: function() { return m[k]; } });
}) : (function(o, m, k, k2) {
    if (k2 === undefined) k2 = k;
    o[k2] = m[k];
}));
var __exportStar = (this && this.__exportStar) || function(m, exports) {
    for (var p in m) if (p !== "default" && !exports.hasOwnProperty(p)) __createBinding(exports, m, p);
};
Object.defineProperty(exports, "__esModule", { value: true });
/*
   This file is used to provide the typings for this package.  The typings should only include the Win32 specific types, even though
   the package actually exports the whole of the win32 implementation of react-native.

   NOTE: Concrete classes, objects etc that actually need to be exported from the package,
         need to also be added to index.win32.js
*/
__exportStar(require("./Libraries/Components/View/ViewWin32.Props"), exports);
__exportStar(require("./Libraries/Components/View/ViewWin32"), exports);
__exportStar(require("./Libraries/Components/Text/TextWin32.Props"), exports);
__exportStar(require("./Libraries/Components/Text/TextWin32"), exports);
__exportStar(require("./Libraries/Components/Button/ButtonWin32.Props"), exports);
__exportStar(require("./Libraries/Components/Button/ButtonWin32"), exports);
__exportStar(require("./Libraries/StyleSheet/PlatformColorValueTypesWin32"), exports);
__exportStar(require("./Libraries/Components/Touchable/TouchableWin32.Props"), exports);
__exportStar(require("./Libraries/Components/Touchable/TouchableWin32.Types"), exports);
__exportStar(require("./Libraries/Components/Touchable/TouchableWin32"), exports);
__exportStar(require("./Libraries/PersonaCoin/PersonaCoin"), exports);
__exportStar(require("./Libraries/PersonaCoin/PersonaCoinTypes"), exports);
__exportStar(require("./Libraries/Utilities/FocusManager.win32"), exports);
//# sourceMappingURL=typings-index.js.map