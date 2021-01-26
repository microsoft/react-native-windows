This directory contains the npm packages hosted by the react-native-windows repo. 

## Scoped Packages

Several packages are organized into specific package scopes:

- `@react-native`: Copies of private upstream packages already using the `@react-native` scope. E.g. `@react-native/tester` for the RNTester package.
- `@rnw-scripts`: Scripts for repository tooling. Published as part of the master branch.
- `@react-native-windows`: Internal packages used by react-native-windows itself. Versioned and published alongside RNW.

## Unscoped packages

Several pacakges do not belong to a scope for intentional (or historical) reasons. E.g. `react-native-platform-override` isn't RNW-specific and
is unscoped. `react-native-windows-init` is run by name, and as such makes sense to be unscoped.
