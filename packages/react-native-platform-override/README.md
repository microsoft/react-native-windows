# react-native-platform-override
Tools to manage "platform overrides" for out of tree React Native platforms.
Facilities for inventorying changes and integrating new upstream changes are included.

Upstream React Native versions are checked out in a temporary Git repository
under the OS temporary directory (`react-native-platform-override/git`).
This is a tool-managed cache: switching versions can overwrite conflicting files
there. Do not use it to store local changes.

## Override Manifests

`react-native-platform-override` operates on a JSON "override manifest" placed
at the root of an npm package. It is expected that these are named
`overrides.json`.

At a Yarn workspace root, validation and upgrades include both the root manifest
and manifests in workspace packages. Inside an individual package, its local
manifest takes precedence. Use `--manifest` to target a single manifest.

An example override manifest
```json
{
  "includePatterns": [
    "src/**"
  ],
  "excludePatterns": [
    "src/README.md"
  ],
  "baseVersion": "0.0.0-10b4b9505",
  "overrides": [
    {
      "type": "derived",
      "file": ".flowconfig",
      "baseFile": ".flowconfig",
      "baseVersion": "0.0.0-56cf99a96",
      "baseHash": "8eab29258f5ad573e478324f10f850b7ccfb49c7"
    },
  ]
}
```

| Name              | Required | Description                                                     | Default  |
|-------------------|----------|-----------------------------------------------------------------|----------|
| `includePatterns` | Optional | A list of globs of files to enforce are listed in the manifest. | `["**"]` |
| `excludePatterns` | Optional | Globs to be excluded from the above list.                       | `[]`     |
| `baseVersion`     | Optional | The default react-native version of an override                 |          |
| `overrides`       | Required | List of registered overrides, added by the tooling              |          |

## Override Types
Overrides can be registered as different types, each with differing behavior.

|          |                                                                                                                  |
|----------|------------------------------------------------------------------------------------------------------------------|
| Platform | Platform-specific logic that doesn't map to an upstream file                                                     |
| Derived  | An override derived from an upstream file. Changes to the original file will be merged into the override.        |
| Patch    | An upstream file with changes made to it. Changes to the original file will be merged into the override.         |
| Copy     | An exact copy of an upstream file or directory. Overrides of this type will remain identical to their originals. |

## Directory copy exclusions

A directory `copy` override can specify `excludePatterns`: glob patterns relative
to both the local `directory` and upstream `baseDirectory`. For example:

```json
{
  "type": "copy",
  "directory": "scripts/js-api",
  "baseDirectory": "scripts/js-api",
  "excludePatterns": ["config.js", "build-types/templates/**"],
  "baseVersion": "<react-native-version>",
  "baseHash": "<hash-generated-by-the-override-factory>"
}
```

Excluded files do not contribute to the base hash or copy comparison and are
neither copied nor deleted during upgrades. Existing local excluded files still
need separate override entries (for example `derived` or `platform`) unless the
manifest's own patterns exclude them. Globs match dotfiles and use `/` separators;
they are exclusion patterns, not negated include patterns.

The directory-copy CLI prompt accepts exclusions as a JSON array. Programmatic
callers can pass them as the fourth argument to
`createDirectoryCopyOverride(directory, baseDirectory, issue, excludePatterns)`.
When adding or changing exclusions on an existing entry, recreate it through the
CLI or factory so its `baseHash` is recalculated. Omitting exclusions preserves
existing directory-copy behavior and hashes.

## CLI

`react-native-platform-override` offers multiple commands to manipulate and verify the override manifest.

### `validate`
Verifies that overrides are recorded and up-to-date.

| Option              | Required | Description                                        | Default                         |
|---------------------|----------|----------------------------------------------------|---------------------------------|
| `--manifest <file>` | Optional | Path to an override manifest to validate           | Package root above cwd          |
| `--version <v>`     | Optional | A version of React Native to check against         | The currently installed version |


### `add <override>`
Adds an override to the manifest, prompting for details.

| Option            | Required | Description               |
|-------------------|----------|---------------------------|
| `<override>`      | Required | The override to add       |

### `remove <override>`
Removes an override to the manifest.

| Option            | Required | Description               |
|-------------------|----------|---------------------------|
| `<override>`      | Required | The override to remove    |

### `upgrade`
Attempts to automatically merge new changes into out-of-date overrides.

| Option              | Required | Description                                                | Default                         |
|---------------------|----------|------------------------------------------------------------|---------------------------------|
| `--manifest <file>` | Optional | Path to an override manifest to validate                   | Package root above cwd          |
| `--version <v>`     | Optional | A version of React Native to check against                 | The currently installed version |
| `--no-conflicts`    | Optional | Whether to allow upgraded files to contain conlict markers | `--conflicts`                   |

### `diff <override>`
Diffs an override against its base file. It it compared to the base file of the override's current version, even if a newer verison of
react-native is installed.

| Option                 | Required | Description                                                                                                    | Default |
|------------------------|----------|----------------------------------------------------------------------------------------------------------------|---------|
| `<override>`           | Required | The override to diff against                                                                                   |         |

## GitHub Tokens
`react-native-platform-override` makes requests to GitHub's API. An OAuth token may optionally be provided by using the `--githubToken`
parameter or setting the PLATFORM_OVERRIDE_GITHUB_TOKEN environment variable.

## Programmatic Use
The package's local TypeScript configuration emits CommonJS for Node.js CLI and
build-script consumers, independently of the shared repository module settings.

`react-native-platform-override` offers a programmatic API with similar capabilities to the CLI. Take a look [here](./src/APi.ts) for details.

E.g.
```js
import {validateManifest} from 'react-native-platform-override';

const validationErrors = await validateManifest({manifestPath: 'src/overrides.json'});
```