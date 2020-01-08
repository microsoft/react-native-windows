/**
 * @format
 */
// @ts-check
const path = require('path');
const fs = require('fs');
const glob = require('glob');
const rimraf = require('rimraf');

function copyDirectories(srcPath, targetPath, dirSpecs) {
  dirSpecs.forEach(dirSpec => {
    if (dirSpec.mergeFiles !== true) {
      if (dirSpec.rmFilter === undefined) {
        rimraf.sync(path.resolve(targetPath, dirSpec.dest));
      } else {
        rimraf.sync(
          path.resolve(targetPath, dirSpec.dest + path.sep + dirSpec.filter),
        );
      }
    }

    const curSrcPath = path.resolve(srcPath, dirSpec.src);
    const curTargetPath = path.resolve(targetPath, dirSpec.dest);

    glob.sync('**/*.{js,png,gif,h}', { cwd: curSrcPath }).forEach(file => {
      const dir = path.dirname(file);
      const targetDir = path.resolve(curTargetPath, dir);
      const targetFile = path.resolve(curTargetPath, file);

      fs.mkdirSync(targetDir, { recursive: true });
      fs.writeFileSync(
        targetFile,
        fs.readFileSync(path.join(curSrcPath, file)),
      );
    });
  });
}

function copyFile(srcPath, targetPath, filename) {
  fs.writeFileSync(
    path.resolve(targetPath, filename),
    fs.readFileSync(path.resolve(srcPath, filename)),
  );
}

exports.copyRNLibraries = () => {
  const reactNativePath = path.dirname(
    require.resolve('react-native/package.json'),
  );
  const reactNativeWinPath = path.resolve(__dirname, '..');

  copyDirectories(reactNativePath, reactNativeWinPath, [
    {
      src: 'flow',
      dest: 'flow',
    },
    {
      src: 'flow-typed',
      dest: 'flow-typed',
    },
    {
      src: 'IntegrationTests',
      dest: 'IntegrationTests',
      rmFilter: '*.js',
    },
    {
      src: 'jest',
      dest: 'jest',
    },
    {
      src: 'Libraries',
      dest: 'Libraries',
    },
    {
      src: 'packages/react-native-codegen/src',
      dest: 'packages/react-native-codegen/src',
    },
    {
      src: 'RNTester',
      dest: 'RNTester',
    },
    {
      src: 'ReactCommon/turbomodule/core',
      dest: 'WorkingHeaders/jsireact',
    },
  ]);

  copyDirectories(reactNativeWinPath, reactNativeWinPath, [
    {
      src: 'src/jest',
      dest: 'jest',
      mergeFiles: true,
    },
  ]);

  copyFile(reactNativePath, reactNativeWinPath, 'rn-get-polyfills.js');
};
