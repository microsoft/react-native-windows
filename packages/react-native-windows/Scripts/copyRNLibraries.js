/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 * @ts-check
 */

const glob = require('glob');
const path = require('path');
const {cleanTask, copyTask, series} = require('just-scripts');

const rnDir = path.dirname(require.resolve('react-native/package.json'));
const rnCopiesDir = path.join(
  path.dirname(require.resolve('react-native-windows/package.json')),
  'ReactCopies',
);

exports.copyTask = baseDir => {
  const reactNative = (...files) => files.map(f => path.join(rnDir, f));
  const reactCopies = (...files) => files.map(f => path.join(rnCopiesDir, f));
  const src = (...files) => files.map(f => path.join(baseDir, 'src', f));
  const base = file => path.join(baseDir, file);

  return series(
    exports.cleanTask(baseDir),

    copyTask({
      paths: reactNative('flow/**'),
      dest: base('flow'),
    }),
    copyTask({
      paths: reactNative('flow-typed/**'),
      dest: base('flow-typed'),
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
      paths: reactNative('Libraries/**/*.+(js|jsx|png|gif|jpg|html)'),
      dest: base('Libraries'),
    }),

    copyTask({
      paths: reactNative('index.js', 'interface.js', 'rn-get-polyfills.js'),
      dest: base('.'),
    }),

    copyTask({paths: src('**/*+(.d.ts|.js|.png)'), dest: base('.')}),
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
      base('index.js'),
      base('interface.js'),
      base('rn-get-polyfills.js'),

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
