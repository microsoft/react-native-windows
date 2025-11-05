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
const rnCopiesDir = path.join(
  path.dirname(require.resolve('react-native-windows/package.json')),
  'ReactCopies',
);

exports.copyTask = baseDir => {
  const reactNative = (...files) => files.map(f => path.join(rnDir, f));
  const reactCopies = (...files) => files.map(f => path.join(rnCopiesDir, f));
  const src = (...files) => files.map(f => path.join(baseDir, 'src-win', f));
  const base = file => path.join(baseDir, file);

  return series(
    exports.cleanTask(baseDir),

    // For the TS compiler to be able to reference the files and create
    // correct output the imported .d.ts files must be within our src-win dir
    copyTask({
      paths: reactNative('types/**/*.d.ts'),
      dest: base('types'),
    }),
    copyTask({
      paths: reactNative('flow/**'),
      dest: base('flow'),
    }),
    copyTask({
      paths: reactNative('jest/**'),
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
    },
  );
};

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
