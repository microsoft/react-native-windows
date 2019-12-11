import { ViewWin32Status } from './ViewWin32.checklist';

export const ViewWin32PageProps = {
  title: 'ViewWin32',
  componentName: 'ViewWin32',
  componentUrl:
    'https://office.visualstudio.com/ISS/SDX%20Polyester/_git/sdx-platform?path=/packages/react-native-win32/src/Libraries/Components/View',
  componentStatus: ViewWin32Status,
  examples: null,
  propertiesTablesSources: [
    require('!raw-loader!@office-iss/react-native-win32/src/Libraries/Components/View/ViewWin32.Props.ts') as string,
  ],
  overview: require('!raw-loader!@office-iss/react-native-win32/src/Libraries/Components/View/docs/ViewWin32Overview.md') as string,
  bestPractices: null,
  isHeaderVisible: true,
};
