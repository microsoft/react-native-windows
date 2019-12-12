import { ButtonWin32Status } from './ButtonWin32.checklist';

export const ButtonWin32PageProps = {
  title: 'ButtonWin32',
  componentName: 'ButtonWin32',
  componentUrl:
    'https://office.visualstudio.com/ISS/SDX%20Polyester/_git/sdx-platform?path=/packages/react-native-win32/src/Libraries/Components/Button',
  componentStatus: ButtonWin32Status,
  examples: null,
  propertiesTablesSources: [
    require('!raw-loader!@office-iss/react-native-win32/src/Libraries/Components/Button/ButtonWin32.Props.ts') as string,
  ],
  overview: require('!raw-loader!@office-iss/react-native-win32/src/Libraries/Components/Button/docs/ButtonWin32Overview.md') as string,
  bestPractices: null,
  isHeaderVisible: true,
};
