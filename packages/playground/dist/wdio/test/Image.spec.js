"use strict";
/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 */
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
const HomePage_1 = __importDefault(require("../pages/HomePage"));
const BasePage_1 = require("../pages/BasePage");
// import ImageTestPage from '../pages/ImageTestPage';
const assert_1 = __importDefault(require("assert"));
beforeAll(() => {
    HomePage_1.default.loadRNTester();
    HomePage_1.default.waitForPageLoaded();
    // HomePage.clickAndGotoImagePage();
});
describe('basicTest', () => {
    it('basicTest', () => {
        const treedump = BasePage_1.By('x_TreeDump');
        const text = treedump.getValue();
        assert_1.default(text == 'fail', `text = ${text}`);
    });
});
// describe('ImageWithoutBorderTest', () => {
//   /* Test case #1: view and image displayed with no border and cornerRadius */
//   it('ImageWithoutBorderTest', () => {
//     const result = ImageTestPage.getTreeDumpResult();
//     assert(result, '#1. Dump comparison for image without border!');
//   });
//   /* Test case #2: Click button once, update view and image with round border*/
//   it('ImageWithBorderTest', () => {
//     ImageTestPage.toggleImageBorder();
//     const result = ImageTestPage.getTreeDumpResult();
//     assert(result, '#2. Dump comparison for image with border!');
//   });
//   /* Test case #3: Click button one more, remove border from view and image but tree sturcture is different from #1*/
//   it('ImageWithoutBorderTest', () => {
//     ImageTestPage.toggleImageBorder();
//     const result = ImageTestPage.getTreeDumpResult();
//     assert(result, '#3. Second dump comparison for image without border!');
//   });
//   it('ImageRTLTest', () => {
//     ImageTestPage.toggleRTLMode();
//     const result = ImageTestPage.getTreeDumpResult();
//     assert(result, '#4. Dump comparison for image RTL');
//   });
// });
//# sourceMappingURL=Image.spec.js.map