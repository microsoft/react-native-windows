# react-native-windows-init

CLI to bootstrap the addition of the windows platform to `react-native` projects.

## Usage

Run this from an existing `react-native` project, to install `react-native-windows` and generate initial project files for windows.


Example usage
```
npx react-native-windows-init --language cpp
```

Options:
| option      | description                                    | type                                             |
|-------------|------------------------------------------------|--------------------------------------------------|
| --help      | Show help                                      | [boolean]                                        |
| --version   | The version of react-native-windows to use.    | [string]                                         |
| --namespace | The native project namespace.                  | [string]                                         |
| --verbose   | Enables logging.                               | [boolean]                                        |
| --language  | Which language the app is written in.          | [string] [choices: "cs", "cpp"] [default: "cpp"] |
| --overwrite | Overwrite any existing files without prompting | [boolean]                                        |
