/**
 * @format
 */
// @ts-check
const path = require('path');
const fs = require('fs');
const glob = require('glob');
const rimraf = require('rimraf');

function retryOnError(errorCode, func) {
  const startTime = Date.now();

  while (true) {
    try {
      func();
      return true;
    } catch (ex) {
      if (ex.code !== errorCode) {
        throw ex;
      }
    }

    if (Date.now() - startTime > 5000) {
      return false;
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
      let deleted = retryOnError('ENOTEMPTY', () => {
        if (dirSpec.rmFilter === undefined) {
          rimraf.sync(path.resolve(targetPath, dirSpec.dest));
        } else {
          rimraf.sync(
            path.resolve(targetPath, dirSpec.dest + path.sep + dirSpec.filter),
          );
        }
      });

      if (!deleted) {
        throw new Error(
          'Timed out trying to delete directory. Ensure no locks are being held to project files and try again.',
        );
      }
    }

    const curSrcPath = path.resolve(srcPath, dirSpec.src);
    const curTargetPath = path.resolve(targetPath, dirSpec.dest);

    glob
      .sync('**/*.{js,png,gif,jpg,html,h}', {cwd: curSrcPath})
      .forEach(file => {
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

exports.copyRNLibraries = baseDir => {
  const reactNativePath = path.dirname(
    require.resolve('react-native/package.json'),
  );

  const reactNativeWindowsPath = path.dirname(
    require.resolve('react-native-windows/package.json'),
  );

  copyDirectories(reactNativeWindowsPath, baseDir, [
    {
      src: 'ReactCopies/RNTester',
      dest: 'RNTester',
    },
    {
      src: 'ReactCopies/IntegrationTests',
      dest: 'IntegrationTests',
      rmFilter: '*.js',
    },
  ]);

  copyDirectories(reactNativePath, baseDir, [
    {
      src: 'flow',
      dest: 'flow',
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
  ]);

  copyDirectories(baseDir, baseDir, [
    {
      src: 'src/jest',
      dest: 'jest',
      mergeFiles: true,
    },
  ]);

  copyFile(reactNativePath, baseDir, 'rn-get-polyfills.js');
  copyFile(reactNativePath, baseDir, 'index.js');
  copyFile(reactNativePath, baseDir, 'interface.js');
};
