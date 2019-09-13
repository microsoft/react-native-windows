/**
 * The react-native cli getPlugins assumes that all the react-native platform packages
 * are located in node_modules.
 *
 * When in a yarn workspace, the react-native platforms can be hoisted, so we need to
 * add a link to the real location so that getPlugins works correctly.
 *
 * @format
 *
 */
// @ts-check

const fs = require('fs');
const path = require('path');
const os = require('os');

const checkOrCreate_node_modules = () => {
  const p = path.join(__dirname, 'node_modules');

  if (!fs.existsSync(p)) {
    fs.mkdirSync(p);
  }
};
checkOrCreate_node_modules();

const link = (name, target) => {
  const p = path.join(__dirname, 'node_modules', name);

  if (!fs.existsSync(p)) {
    fs.symlinkSync(target, p, os.platform() === 'win32' ? 'junction' : 'dir');
  }
};

link('react-native-windows', path.resolve(__dirname, '../../vnext'));
link(
  'react-native',
  path.resolve(require.resolve('react-native/package.json'), '..'),
);
link(
  'rnpm-plugin-windows',
  path.resolve(require.resolve('rnpm-plugin-windows/package.json'), '..'),
);
