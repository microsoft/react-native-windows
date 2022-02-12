This directory contains the npm packages hosted by the react-native-windows repo. By convention, the directory naming structure maps 1:1 to the published or workspace names of the npm packages.

## Scoped Packages

Several packages are organized into specific [package scopes](https://docs.npmjs.com/cli/v8/using-npm/scope):

- `@office-iss`: Office internal packages
- `@react-native`: Copies of private upstream packages that are referenced under the `@react-native` scope. E.g. `@react-native/tester` for the "RNTester" npm package.
- `@react-native-windows`: Packages associated with the `react-native-windows` project (both public and private)
- `@rnw-scripts`: Private scripts used by `react-native-windows` infrastructure.

## Unscoped packages

Several packages do not belong to a scope for intentional (or historical) reasons. E.g. `react-native-windows-init` is executed by package name. 

## Adding a new scope

> ⚠ Caution: Failing to create and give access to an NPM organization will lead to publishing failures, and will cause future PRs and rolling builds to fail.

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
