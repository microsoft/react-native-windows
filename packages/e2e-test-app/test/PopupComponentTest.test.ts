import {dumpVisualTree} from '@react-native-windows/automation-commands';
import {goToComponentExample} from './RNTesterNavigation';
import {verifyNoErrorLogs} from './Helpers';

beforeAll(async () => {
  await goToComponentExample('Popup');
});

afterEach(async () => {
  await verifyNoErrorLogs();
});

describe('PopupTests', () => {
  test('Popups can be anchored', async () => {
    const dump = await dumpVisualTree('anchor-popup');
    expect(dump).toMatchSnapshot();
  });
  test('Popups can have different placement', async () => {
    const dump = await dumpVisualTree('popup-placement');
    expect(dump).toMatchSnapshot();
  });
  test('Popups can have customized accessibility', async () => {
    const dump = await dumpVisualTree('popup-accessibility');
    expect(dump).toMatchSnapshot();
  });
});