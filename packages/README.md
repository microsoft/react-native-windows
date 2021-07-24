This directory contains the npm packages hosted by the react-native-windows repo. 

## Scoped Packages

Several packages are organized into specific package scopes:

- `@office-iss`: Office internal packages
- `@react-native`: Copies of private upstream packages already using the `@react-native` scope. E.g. `@react-native/tester` for the RNTester package.
- `@react-native-windows`: Internal packages used by react-native-windows itself. Versioned and published alongside RNW.
- `@rnw-bots`: Bot infrastructure for the react-native-windows repo. Published as part of the main branch.
- `@rnw-scripts`: Scripts for repository tooling. Published as part of the main branch.

## Unscoped packages

Several packages do not belong to a scope for intentional (or historical) reasons. E.g. `react-native-platform-override` isn't RNW-specific and
is unscoped. `react-native-windows-init` is run by name, and as such makes sense to be unscoped.

## Adding a new scope

> ⚠ Caution: Failing to create and give access to an NPM organization will break CI.

New package scopes for related packages can be added by adding a new directory with the scope name and prefixing the
packages name in `package.json` with the scope.

E.g. for `packages/@example-scope/create-widgets/package.json`:
```jsonc
{
  "name": "@example-scope/create-widgets",
  ...
}
```

After adding a scope directory, add the scope to the list of yarn workspaces in the repo-root `package.json`.
```jsonc
  "workspaces": {
    "packages": [
      ...
      "packages/@example-scope/*",
      ...
    ]
  },
```

Publishing a scoped package requires that the **rnbot** NPM user is an owner of an **npm organization** with the
same name as the scope. You can check whether an organization exists by viewing [npmjs.com/org/<scope>](https://www.npmjs.com/org/rnw-scripts).
See acoates for granting permissions to rnbot.
