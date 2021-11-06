/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

 import {dumpVisualTree} from '@react-native-windows/automation-commands';
 import {goToComponentExample} from './RNTesterNavigation';
 
 beforeAll(async () => {
   await goToComponentExample('XAML');
 });
 
 describe('XamlTest', () => {
   test('Layout', async () => {
     const dump = await dumpVisualTree('ReactNativeXAMLRoot');
     expect(dump).toMatchSnapshot();
   });
 });
 
 