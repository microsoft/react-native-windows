/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import { goToComponentExample, dumpVisualTree } from './framework';

beforeAll(async () => {
    await goToComponentExample('View');
});

describe('ViewComponentTest', () => {
    /* Test case #1: display:none disabled */
    test('ViewComponentTestWithoutDisplayNone', async () => {
        const dump = await dumpVisualTree('view-component-switch-view');
        expect(dump).toMatchSnapshot();
    });

    /* Test case #2: Click button once, update view*/
    test('ViewComponentTestWithDisplayNone', async () => {
        await toggleDisplayNone();
        const dump = await dumpVisualTree('view-component-switch-view');
        expect(dump).toMatchSnapshot();
    });

    /* Test case #3: Click button one more, return to #1*/
    test('ViewComponentTestWithoutDisplayNone #2', async () => {
        await toggleDisplayNone();
        const dump = await dumpVisualTree('view-component-switch-view');
        expect(dump).toMatchSnapshot();
    });
});

async function toggleDisplayNone() {
    const showDisplayNoneToggle = await $('~toggle-display:none');
    await showDisplayNoneToggle.click();
}
