/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 * @ts-check
 */

const glob = require('glob');
const path = require('path');
const fs = require('fs');
const {cleanTask, copyTask, series} = require('just-scripts');

const rnDir = path.dirname(require.resolve('react-native/package.json'));
// The Jest setup, mocks, and related helpers moved out of the react-native
// package and into @react-native/jest-preset in newer React Native versions.
const jestPresetDir = path.dirname(
  require.resolve('@react-native/jest-preset/package.json'),
);
const rnCopiesDir = path.join(
  path.dirname(require.resolve('react-native-windows/package.json')),
  'ReactCopies',
);

exports.copyTask = baseDir => {
  const reactNative = (...files) => files.map(f => path.join(rnDir, f));
  const jestPreset = (...files) => files.map(f => path.join(jestPresetDir, f));
  const reactCopies = (...files) => files.map(f => path.join(rnCopiesDir, f));
  const src = (...files) => files.map(f => path.join(baseDir, 'src-win', f));
  const base = file => path.join(baseDir, file);

  return series(
    exports.cleanTask(baseDir),

    // For the TS compiler to be able to reference the files and create
    // correct output the imported .d.ts files must be within our src-win dir
    copyTask({
      paths: reactNative('types_DEPRECATED/**/*.d.ts'),
      dest: base('types'),
    }),
    copyTask({
      paths: reactNative('flow/**'),
      dest: base('flow'),
    }),
    copyTask({
      paths: jestPreset('jest/**'),
      dest: base('jest'),
    }),
    copyTask({
      paths: reactCopies('IntegrationTests/**'),
      dest: base('IntegrationTests'),
    }),
    copyTask({
      paths: reactNative(
        'Libraries/**/*.+(d.ts|js|jsx|png|gif|jpg|html|js.flow)',
      ),
      dest: base('Libraries'),
    }),
    copyTask({
      paths: reactNative('src/**/*.+(d.ts|js|jsx|png|gif|jpg|html)'),
      dest: base('src'),
    }),
    copyTask({
  paths: reactNative('src/private/webapis/performance/UserTimingExtensibility.js.flow'),
  dest: base('src/private/webapis/performance'),
  rename: 'UserTimingExtensibility.js', // Rename .js.flow to .js
}),
    copyTask({
      paths: reactNative('index.js', 'interface.js', 'rn-get-polyfills.js'),
      dest: base('.'),
    }),

    copyTask({paths: src('**/*+(.d.ts|.js|.png|.flow)'), dest: base('.')}),

    () => {
      const typesPath = path.resolve(baseDir, 'types/index.d.ts');
      types = fs.readFileSync(typesPath);
      types =
        types +
        "\n// Export platform specific types\nexport * from '../Libraries/platform-types';\n";
      fs.writeFileSync(typesPath, types);
      exports.applyWindowsTypeExtensions(baseDir);
    },
  );
};

// types_DEPRECATED is the public View surface, but it no longer carries the
// Windows accessibility/focus extensions that live in src-win. Splice those
// deltas into the copied declarations so `import {View} from 'react-native'`
// (resolved to this package) accepts the Windows props.
exports.applyWindowsTypeExtensions = baseDir => {
  const viewA11y = path.join(
    baseDir,
    'types/Libraries/Components/View/ViewAccessibility.d.ts',
  );
  const viewProps = path.join(
    baseDir,
    'types/Libraries/Components/View/ViewPropTypes.d.ts',
  );

  replaceOnce(
    viewA11y,
    "  'aria-label'?: string | undefined;\n",
    "  'aria-label'?: string | undefined;\n" +
      '\n' +
      '  accessibilityLevel?: number | undefined; // Windows\n' +
      "  'aria-level'?: number | undefined; // Windows\n" +
      '  accessibilityPosInSet?: number | undefined; // Windows\n' +
      "  'aria-posinset'?: number | undefined; // Windows\n" +
      '  accessibilitySetSize?: number | undefined; // Windows\n' +
      "  'aria-setsize'?: number | undefined; // Windows\n" +
      '  accessibilityAnnotation?: AccessibilityAnnotationInfo | undefined; // Windows\n' +
      '  accessibilityItemType?: string | undefined; // Windows\n' +
      '  accessibilityAccessKey?: string | undefined; // Windows\n' +
      '  accessibilityDescription?: string | undefined; // Windows\n' +
      "  'aria-description'?: string | undefined; // Windows\n",
  );
  replaceOnce(
    viewA11y,
    "  | 'iconmenu';\n",
    "  | 'iconmenu'\n" +
      "  | 'group' // Windows\n" +
      "  | 'tree' // Windows\n" +
      "  | 'treeitem' // Windows\n" +
      "  | 'listitem'; // Windows\n",
  );
  replaceOnce(
    viewA11y,
    '  expanded?: boolean | undefined;\n}',
    '  expanded?: boolean | undefined;\n' +
      '  /** @platform windows */\n' +
      '  readOnly?: boolean | undefined;\n' +
      '  /** @platform windows */\n' +
      '  multiselectable?: boolean | undefined;\n' +
      '  /** @platform windows */\n' +
      '  required?: boolean | undefined;\n}',
  );
  if (!fs.readFileSync(viewA11y, 'utf8').includes('export type AnnotationType')) {
    fs.appendFileSync(
      viewA11y,
      '\n// [Windows]\n' +
        'export type AnnotationType =\n' +
        "  | 'AdvanceProofingIssue'\n" +
        "  | 'Author'\n" +
        "  | 'CircularReferenceError'\n" +
        "  | 'Comment'\n" +
        "  | 'ConflictingChange'\n" +
        "  | 'DataValidationError'\n" +
        "  | 'DeletionChange'\n" +
        "  | 'EditingLockedChange'\n" +
        "  | 'Endnote'\n" +
        "  | 'ExternalChange'\n" +
        "  | 'Footer'\n" +
        "  | 'Footnote'\n" +
        "  | 'FormatChange'\n" +
        "  | 'FormulaError'\n" +
        "  | 'GrammarError'\n" +
        "  | 'Header'\n" +
        "  | 'Highlighted'\n" +
        "  | 'InsertionChange'\n" +
        "  | 'Mathematics'\n" +
        "  | 'MoveChange'\n" +
        "  | 'SpellingError'\n" +
        "  | 'TrackChanges'\n" +
        "  | 'Unknown'\n" +
        "  | 'UnsyncedChange';\n" +
        '\n' +
        'export type AccessibilityAnnotationInfo = Readonly<{\n' +
        '  typeID: AnnotationType;\n' +
        '  typeName?: string;\n' +
        '  author?: string;\n' +
        '  dateTime?: string;\n' +
        '  target?: string;\n' +
        '}>;\n',
    );
  }

  // Windows focus uses the full HTML tabIndex range, not just 0 | -1.
  replaceOnce(
    viewProps,
    '  tabIndex?: 0 | -1 | undefined;',
    '  tabIndex?: number | undefined; // Windows',
  );
};

function replaceOnce(filePath, oldText, newText) {
  const text = fs.readFileSync(filePath, 'utf8');
  const count = text.split(oldText).length - 1;
  if (count !== 1) {
    throw new Error(
      `Expected 1 occurrence of Windows type anchor in ${filePath}, found ${count}`,
    );
  }
  fs.writeFileSync(filePath, text.replace(oldText, newText));
}

exports.cleanTask = baseDir => {
  const base = file => path.join(baseDir, file);

  return cleanTask({
    paths: [
      base('flow'),
      base('jest'),

      // IntegrationTests overlaps with the desktop test runner, so we need to
      // be careful about what is removed until that is fixed.
      ...glob.sync('IntegrationTests/**/*.+(js|command|png)', {
        cwd: baseDir,
        absolute: true,
      }),

      base('Libraries'),
      base('src'),
      base('index.js'),
      base('interface.js'),
      base('rn-get-polyfills.js'),
      base('src-win/rntypes'),
      base('types'),

      // Remove TS compiled gunk in our root
      ...glob.sync(
        '+(index|typings-index)*(.windows|.win32)*(.d)+(.js|.ts)*(.map)',
        {
          cwd: baseDir,
          absolute: true,
        },
      ),
    ],
  });
};
