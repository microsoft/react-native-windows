# @react-native-windows/bot-coordinator

TBD

## Running locally
1. Ensure Azure Functions Core Tools is installed (e.g. through `choco install azure-functions-core-tools-3`)
1. Copy `local.settings.example.json` to `local.settings.json` and add secrets. This file is ignored by Git.
1. Run `yarn start` in the package root

Functions can be invoked with arbitrary inputs by sending a REST request to localhost. E.g.

```
POST http://localhost:7071/admin/functions/HeartbeatFunction
Content-Type: application/json

{
    "input": null
}
```

## Deployment
The Azure functions application is deployed nightly along with other artifacts published by the repo.