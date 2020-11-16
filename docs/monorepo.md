# Monorepo details for react-native-windows
react-native-windows is a monorepo with several [packages](https://github.com/microsoft/react-native-windows/tree/master/packages) and uses monorepo management tools to install, build, and publish.  This page is all about what tools we use to manage this monorepo and some technical details, aimed at folks who are contributing to react-native-windows.

# Installation of npm dependencies
react-native-windows uses [yarn workspaces](https://legacy.yarnpkg.com/en/docs/workspaces/) to install all react-native-windows npm dependencies.  See "workspaces" prop inside [package.json](https://github.com/microsoft/react-native-windows/blob/master/package.json).

# Lage Build
react-native-windows uses [lage](https://microsoft.github.io/lage/) to build all the "non-native" parts of react-native-windows.  See "scripts" prop inside  [package.json](https://github.com/microsoft/react-native-windows/blob/master/package.json).  Also see [laga.config.js](https://github.com/microsoft/react-native-windows/blob/master/lage.config.js).

# Beachball publishing
TODO - add details
