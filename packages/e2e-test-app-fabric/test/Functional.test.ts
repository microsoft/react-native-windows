
import {app} from '@react-native-windows/automation';
import {verifyNoErrorLogs} from './Helpers';

afterEach(async () => {
  await verifyNoErrorLogs();
});

describe('Functional', () => {
  test('control', async () => {
    const searchBox = await app.findElementByTestID('explorer_search');
    await searchBox.addValue('hello');
    //const text = await searchBox.getAttribute('autoCapitalize').then((text) => { expect(text).toBe('none');});
    console.warn(searchBox);
    expect(await searchBox.getText()).toBe('hello');
    //expect(await searchBox.getValue()).toBe('hello');
  });
});

export {};
