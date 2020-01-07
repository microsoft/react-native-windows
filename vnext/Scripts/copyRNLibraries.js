/**
 * @format
 */
// @ts-check
const path = require('path');
const fs = require('fs');
const rimraf = require('rimraf');

function copyFileSync(source, target) {
  var targetFile = target;

  //if target is a directory a new file with the same name will be created
  if (fs.existsSync(target)) {
    if (fs.lstatSync(target).isDirectory()) {
      targetFile = path.join(target, path.basename(source));
    }
  }

  fs.writeFileSync(targetFile, fs.readFileSync(source));
}

function copyJSFolderRecursiveSync(source, target) {
  var files = [];

  //check if folder needs to be created or integrated
  var targetFolder = path.join(target, path.basename(source));
  if (!fs.existsSync(targetFolder)) {
    fs.mkdirSync(targetFolder);
  }

  //copy
  if (fs.lstatSync(source).isDirectory()) {
    files = fs.readdirSync(source);
    files.forEach(function(file) {
      var curSource = path.join(source, file);
      if (fs.lstatSync(curSource).isDirectory()) {
        copyJSFolderRecursiveSync(curSource, targetFolder);
      } else {
        if (
          curSource.endsWith('.js') ||
          curSource.endsWith('.png') ||
          curSource.endsWith('.gif')
        ) {
          copyFileSync(curSource, targetFolder);
        }
      }
    });
  }
}

exports.copyRNLibraries = baseDir => {
  const rnPath = path.dirname(require.resolve('react-native/package.json'));

  const rnCodeGenDest = path.resolve(baseDir, 'packages/react-native-codegen');
  if (fs.existsSync(rnCodeGenDest)) {
    rimraf.sync(rnCodeGenDest);
  }
  const integrationTestsDest = path.resolve(baseDir, 'IntegrationTests');
  if (fs.existsSync(integrationTestsDest)) {
    rimraf.sync(integrationTestsDest + path.sep + '*.js');
  }
  const librariesDest = path.resolve(baseDir, 'Libraries');
  if (fs.existsSync(librariesDest)) {
    rimraf.sync(librariesDest);
  }
  const flowDest = path.resolve(baseDir, 'flow');
  if (fs.existsSync(flowDest)) {
    rimraf.sync(flowDest);
  }
  const flowTypedDest = path.resolve(baseDir, 'flow-typed');
  if (fs.existsSync(flowTypedDest)) {
    rimraf.sync(flowTypedDest);
  }
  const jestDest = path.resolve(baseDir, 'jest');
  if (fs.existsSync(jestDest)) {
    rimraf.sync(jestDest);
  }
  const rnTesterDest = path.resolve(baseDir, 'RNTester');
  if (fs.existsSync(rnTesterDest)) {
    rimraf.sync(rnTesterDest);
  }

  if (fs.existsSync(path.resolve(rnPath, 'IntegrationTests'))) {
    copyJSFolderRecursiveSync(
      path.resolve(rnPath, 'IntegrationTests'),
      baseDir,
    );
  }

  if (!fs.existsSync(path.resolve(baseDir, 'packages'))) {
    fs.mkdirSync(path.resolve(baseDir, 'packages'));
  }
  if (!fs.existsSync(path.resolve(baseDir, 'packages/react-native-codegen'))) {
    fs.mkdirSync(path.resolve(baseDir, 'packages/react-native-codegen'));
  }

  copyJSFolderRecursiveSync(
    path.resolve(rnPath, 'packages/react-native-codegen/src'),
    path.resolve(baseDir, 'packages/react-native-codegen'),
  );
  copyJSFolderRecursiveSync(path.resolve(rnPath, 'flow'), baseDir);
  copyJSFolderRecursiveSync(path.resolve(rnPath, 'flow-typed'), baseDir);
  copyJSFolderRecursiveSync(path.resolve(rnPath, 'Libraries'), baseDir);
  copyJSFolderRecursiveSync(path.resolve(rnPath, 'jest'), baseDir);
  copyJSFolderRecursiveSync(path.resolve(baseDir, 'src/jest'), baseDir); // Copy js files from src/jest to jest

  fs.writeFileSync(
    path.resolve(baseDir, 'rn-get-polyfills.js'),
    fs.readFileSync(path.resolve(rnPath, 'rn-get-polyfills.js')),
  );

  if (fs.existsSync(path.resolve(rnPath, 'RNTester'))) {
    copyJSFolderRecursiveSync(path.resolve(rnPath, 'RNTester'), baseDir);
  }

  /*
  if (!fs.existsSync(path.resolve(baseDir, 'lib/local-cli'))) {
    fs.mkdirSync(path.resolve(baseDir, 'lib/local-cli'));
  }
  if (!fs.existsSync(path.resolve(baseDir, 'lib/local-cli/bundle'))) {
    fs.mkdirSync(path.resolve(baseDir, 'lib/local-cli/bundle'));
  }
  */
};
