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

exports.copyRNLibraries = () => {
  const rnPath = path.dirname(require.resolve('react-native/package.json'));

  const integrationTestsDest = path.resolve(__dirname, '../IntegrationTests');
  if (fs.existsSync(integrationTestsDest)) {
    rimraf.sync(integrationTestsDest + path.sep + '*.js');
  }
  const librariesDest = path.resolve(__dirname, '../Libraries');
  if (fs.existsSync(librariesDest)) {
    rimraf.sync(librariesDest);
  }
  const rnTesterDest = path.resolve(__dirname, '../RNTester');
  if (fs.existsSync(rnTesterDest)) {
    rimraf.sync(rnTesterDest);
  }
  const baseDir = path.resolve(__dirname, '..');
  if (fs.existsSync(path.resolve(rnPath, 'IntegrationTests'))) {
    copyJSFolderRecursiveSync(
      path.resolve(rnPath, 'IntegrationTests'),
      baseDir,
    );
  }
  copyJSFolderRecursiveSync(path.resolve(rnPath, 'Libraries'), baseDir);

  fs.writeFileSync(
    path.resolve(__dirname, '../rn-get-polyfills.js'),
    fs.readFileSync(path.resolve(rnPath, 'rn-get-polyfills.js')),
  );

  if (fs.existsSync(path.resolve(rnPath, 'RNTester'))) {
    copyJSFolderRecursiveSync(path.resolve(rnPath, 'RNTester'), baseDir);
  }

  /*
  if (!fs.existsSync(path.resolve(__dirname, '../lib/local-cli'))) {
    fs.mkdirSync(path.resolve(__dirname, '../lib/local-cli'));
  }
  if (!fs.existsSync(path.resolve(__dirname, '../lib/local-cli/bundle'))) {
    fs.mkdirSync(path.resolve(__dirname, '../lib/local-cli/bundle'));
  }
  */
};
