# @react-native-windows/find-dotnet-tools

Helpers to locate .NET-based tools (e.g. PowerShell) restored via `dotnet tool restore` or
available on PATH.

Used to resolve tool paths consistently across local development and CI
environments.

## Usage

Add the package as a dependency:

```json
{
  "dependencies": {
    "@react-native-windows/find-dotnet-tools": "<version>"
  }
}
```

### findPowerShell

Locates a PowerShell executable by checking, in order:

1. A `dotnet-tool`-restored copy of `pwsh.exe` (skipped in CI builds)
2. `pwsh.exe` on the system PATH
3. The built-in Windows PowerShell as a last resort

```js
import {findPowerShell} from '@react-native-windows/find-dotnet-tools';

const pwsh = findPowerShell();
// e.g. "C:\\Users\\user\\.nuget\\packages\\PowerShell\\7.6.1\\tools\\net10.0\\any\\win\\pwsh.exe"
```

### getNugetGlobalPackagesFolder

Returns the path to the global NuGet packages folder by checking, in order:

1. The `NUGET_PACKAGES` environment variable
2. The output of `dotnet nuget locals global-packages --list`
3. The default `~/.nuget/packages` location

```js
import {getNugetGlobalPackagesFolder} from '@react-native-windows/find-dotnet-tools';

const packagesDir = getNugetGlobalPackagesFolder();
// e.g. "C:\\Users\\user\\.nuget\\packages"
```
