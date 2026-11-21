# Mirrored React Native scripts

The upstream files in `shared/` mirror `scripts/shared/` from the React Native
version recorded in the root [override manifest](../overrides.json). They are
tracked as individual `copy` and `derived` overrides to allow local adaptations.

The `js-api/` directory is an exact mirror of upstream `scripts/js-api/`, tracked
as a directory `copy` override. Keep its contents unchanged, including tests,
fixtures, and templates. See its [README](js-api/README.md) for upstream usage;
mirroring the scripts does not configure the type-generation pipeline for RNW.

Run `yarn react-native-platform-override validate --manifest overrides.json` to
validate these mirrors, or `yarn validate-overrides` to validate all manifests.
Update the mirrors with
`yarn react-native-platform-override upgrade --manifest overrides.json --version <version>`.
When upgrading, check upstream for added or removed files in `scripts/shared/`
and update the manifest accordingly.
