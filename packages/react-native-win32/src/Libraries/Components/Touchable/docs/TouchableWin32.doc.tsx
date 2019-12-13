import { TouchableWin32Status } from './TouchableWin32.checklist';

export const TouchableWin32PageProps = {
  title: 'TouchableWin32',
  componentName: 'TouchableWin32',
  componentUrl:
    'https://office.visualstudio.com/ISS/SDX%20Polyester/_git/sdx-platform?path=/packages/react-native-win32/src/Libraries/Components/Touchable',
  componentStatus: TouchableWin32Status,
  examples: null,
  propertiesTablesSources: [
    require('!raw-loader!@office-iss/react-native-win32/src/Libraries/Components/Touchable/Touchable.Props.ts') as string,
  ],
  overview: require('!raw-loader!@office-iss/react-native-win32/src/Libraries/Components/View/docs/TouchableWin32Overview.md') as string,
  bestPractices: null,
  isHeaderVisible: true,
};
