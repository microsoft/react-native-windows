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

const link = (name, target) => {
  const p = path.join(__dirname, 'node_modules', name);

  if (!fs.existsSync(p)) {
    fs.symlinkSync(target, p, 'dir');
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
