/**
 * @format
 */
// @ts-check
const path = require('path');
const fs = require('fs');
const glob = require('glob');
const rimraf = require('rimraf');

function retryOnError(errorCode, func) {
  let shouldLoop = true;

  while (shouldLoop) {
    try {
      func();
      shouldLoop = false;
    } catch (ex) {
      if (ex.code !== errorCode) {
        throw ex;
      }
    }
  }
}

function copyDirectories(srcPath, targetPath, dirSpecs) {
  dirSpecs.forEach(dirSpec => {
    if (dirSpec.mergeFiles !== true) {
      // rimraf issue 72: We will see ENOTEMPTY if Windows is still holding a
      // lock to a file in our directory. E.g. if we get unlucky with timing
      // and antivirus decides to run. Keep on spinning until we're able to
      // delete the directory.
      retryOnError('ENOTEMPTY', () => {
        if (dirSpec.rmFilter === undefined) {
          rimraf.sync(path.resolve(targetPath, dirSpec.dest));
        } else {
          rimraf.sync(
            path.resolve(targetPath, dirSpec.dest + path.sep + dirSpec.filter),
          );
        }
      });
    }

    const curSrcPath = path.resolve(srcPath, dirSpec.src);
    const curTargetPath = path.resolve(targetPath, dirSpec.dest);

    glob.sync('**/*.{js,png,gif,h}', {cwd: curSrcPath}).forEach(file => {
      const dir = path.dirname(file);
      const targetDir = path.resolve(curTargetPath, dir);
      const targetFile = path.resolve(curTargetPath, file);

      fs.mkdirSync(targetDir, {recursive: true});
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
