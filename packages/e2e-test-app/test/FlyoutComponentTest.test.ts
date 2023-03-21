import {dumpVisualTree} from '@react-native-windows/automation-commands';
import {goToComponentExample} from './RNTesterNavigation';
import {verifyNoErrorLogs} from './Helpers';

beforeAll(async () => {
  await goToComponentExample('Flyout');
});

afterEach(async () => {
  await verifyNoErrorLogs();
});

describe('FlyoutTests', () => {
  test('Flyout', async () => {
    const dump = await dumpVisualTree('flyout');
    expect(dump).toMatchSnapshot();
  });
});