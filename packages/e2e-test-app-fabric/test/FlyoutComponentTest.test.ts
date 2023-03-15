import {dumpVisualTree} from '@react-native-windows/automation-commands';
import {goToComponentExample} from './RNTesterNavigation';

beforeAll(async () => {
  await goToComponentExample('Flyout');
});

describe('FlyoutTests', () => {
  test('Flyout', async () => {
    const dump = await dumpVisualTree('flyout');
    expect(dump).toMatchSnapshot();
  });
});