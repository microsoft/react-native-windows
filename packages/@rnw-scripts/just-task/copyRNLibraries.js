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
      paths: reactNative(
        'src/private/webapis/performance/UserTimingExtensibility.js.flow',
      ),
      dest: base('src/private/webapis/performance'),
      rename: 'UserTimingExtensibility.js', // Rename .js.flow to .js
    }),
    copyTask({
      paths: reactNative('index.js', 'interface.js', 'rn-get-polyfills.js'),
      dest: base('.'),
    }),

    copyTask({paths: src('**/*+(.d.ts|.js|.png|.flow)'), dest: base('.')}),

    () => {
      exports.applyWindowsFlowFixes(baseDir);
    },
  );
};

// Flow's cloneElement overloads do not accept React.MixedElement. The copied
// AnimatedScrollView hits that on both Windows packages, so suppress it here
// rather than only in the generated file that yarn build deletes.
exports.applyWindowsFlowFixes = baseDir => {
  const animatedScrollView = path.join(
    baseDir,
    'Libraries/Animated/components/AnimatedScrollView.js',
  );
  replaceOnce(
    animatedScrollView,
    '    const refreshControl = cloneElement(props.refreshControl, {\n',
    '    // $FlowFixMe[incompatible-type] Flow cloneElement overloads do not accept MixedElement.\n' +
      '    const refreshControl = cloneElement(props.refreshControl, {\n',
  );

  // Flow 0.316 flags extracting console methods. These files are recopied
  // from React Native, so the suppression has to live in the copy step.
  const logBox = path.join(baseDir, 'Libraries/LogBox/LogBox.js');
  replaceOnce(
    logBox,
    '        originalConsoleWarn = console.warn.bind(console);\n',
    '        // $FlowFixMe[method-unbinding]\n' +
      '        originalConsoleWarn = console.warn.bind(console);\n',
  );

  const logError = path.join(baseDir, 'Libraries/Utilities/logError.js');
  replaceOnce(
    logError,
    '    console.error.apply(console, args);\n',
    '    // $FlowFixMe[method-unbinding]\n' +
      '    console.error.apply(console, args);\n',
  );

  const localSetup = path.join(baseDir, 'jest/local-setup.js');
  replaceOnce(
    localSetup,
    '// $FlowFixMe[incompatible-type]\nconst consoleError: (...data: ReadonlyArray<unknown>) => void = console.error;\n',
    '// $FlowFixMe[incompatible-type]\n// $FlowFixMe[method-unbinding]\nconst consoleError: (...data: ReadonlyArray<unknown>) => void = console.error;\n',
  );
  replaceOnce(
    localSetup,
    '// $FlowFixMe[incompatible-type]\nconst consoleWarn: (...data: ReadonlyArray<unknown>) => void = console.warn;\n',
    '// $FlowFixMe[incompatible-type]\n// $FlowFixMe[method-unbinding]\nconst consoleWarn: (...data: ReadonlyArray<unknown>) => void = console.warn;\n',
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
