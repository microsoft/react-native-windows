const fs = require('fs');
const path = require('path');

module.exports = {
  gitTags: false,
  hooks: {
    prepublish: packagePath => {
      const packageJsonPath = path.join(packagePath, 'package.json');
      const packageJson = JSON.parse(fs.readFileSync(packageJsonPath, 'utf-8'));
      if (packageJson.onPublish) {
        Object.assign(packageJson, packageJson.onPublish);
        delete packageJson.onPublish;
        fs.writeFileSync(packageJsonPath, JSON.stringify(packageJson, null, 2) + '\n');
      }
    }
  }
}
