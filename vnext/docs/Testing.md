# Testing ReactNative for Windows

When contributing to this project, unit and integration tests should be run to help prevent new bugs and regressions.

## Windows Desktop
Tests should be run with a VSTest-compatible client
(i.e. [VSTest.Console.exe](https://docs.microsoft.com/en-us/visualstudio/test/vstest-console-options?view=vs-2019),
[Visual Studio Test task](https://docs.microsoft.com/en-us/azure/devops/pipelines/tasks/test/vstest?view=azure-devops)).
This project includes convenience scripts to set up and run the test artifacts.

### Unit Tests
*Implemented in the React.Windows.Desktop.UnitTests project.*
Isolated, self-contained tests covering concrete implementations against mock dependencies and data.
```powershell
& Scripts\UnitTests.ps1
```

### Integration Tests
*Implemented in the React.Windows.Desktop.IntegrationTests project.*
Higher-level workflows interacting with real external dependencies (i.e. file system, networking servers).
These tests can run on the local development host.
```powershell
# Starts test services and runs tests.
& Scripts\IntegrationTests.ps1
```

### E2E Tests
*Implemented in packages/E2ETest*
<BR/>Please follow [Author and Run E2E Test for React Native Windows](E2ETest.md) for E2E Tests