"use strict";
/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 */
Object.defineProperty(exports, "__esModule", { value: true });
const BasePage_1 = require("./BasePage");
// import TextInputTestPage from './TextInputTestPage';
// import {
//   TEXTINPUT_TESTPAGE,
//   LOGIN_TESTPAGE,
//   DIRECT_MANIPULATION_TESTPAGE,
//   IMAGE_TESTPAGE,
//   CONTROL_STYLE_TESTPAGE,
//   TRANSFORM_TESTPAGE,
// } from '../../app/Consts';
// import LoginPage from './LoginPage';
// import DirectManipulationPage from './DirectManipulationPage';
// import ImageTestPage from './ImageTestPage';
// import ControlStyleTestPage from './ControlStylePage';
class HomePage extends BasePage_1.BasePage {
    loadRNTester() {
        this.loadButton.click();
        this.waitForPageLoaded();
    }
}
exports.default = new HomePage();
//# sourceMappingURL=HomePage.js.map