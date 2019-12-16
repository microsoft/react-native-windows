import { TextWin32Status } from './TextWin32.checklist';

export const TextWin32PageProps = {
  title: 'TextWin32',
  componentName: 'TextWin32',
  componentUrl:
    'https://office.visualstudio.com/ISS/SDX%20Polyester/_git/sdx-platform?path=/packages/react-native-win32/src/Libraries/Components/Text',
  componentStatus: TextWin32Status,
  examples: null,
  propertiesTablesSources: [
    require('!raw-loader!@office-iss/react-native-win32/src/Libraries/Components/Text/TextWin32.Props.ts') as string,
  ],
  overview: require('!raw-loader!@office-iss/react-native-win32/src/Libraries/Components/Text/docs/TextWin32Overview.md') as string,
  bestPractices: null,
  isHeaderVisible: true,
};
