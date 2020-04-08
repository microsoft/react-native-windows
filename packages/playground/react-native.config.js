const fs = require('fs');
const path = require('path');
module.exports = {
  reactNativePath:  fs.realpathSync(path.resolve(require.resolve('react-native-windows/package.json'), '..')),
};
