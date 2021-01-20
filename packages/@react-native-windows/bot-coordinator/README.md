# @react-native-windows/bot-coordinator

This package contains code bridging Azure Functions to the bot-actors platform.

## Running locally
1. Install Azure Functions Core Tools (e.g. through `choco install azure-functions-core-tools-3`)
1. Ensure the [Azurite]() extension is installed
1. Add any secrets needed to `local.settings.json`. This file is ignored by Git, but be careful not to edit the example file instead!
1. Run `yarn start` in the package root

HttpTriggers can be triggered from localhost using a REST client like PostMan or curl. E.g.

```
curl -X POST \
  http://localhost:7071/api/raiseEvent \
  -H 'content-type: application/json' \
  -d '{
    "name": '\''integration-timer-fired'\''
}'
```

In production, HTTP requests must be signed using an HMAC secret. This is disabled by default when running locally for convenience, but can be re-enabled with changes to the default `ENABLE_SIGNATURE_VERIFICATION` setting.

## Deployment
The Azure functions application is deployed nightly along with other artifacts published by the repo.
