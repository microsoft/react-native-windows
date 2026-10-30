# React Native Windows Azure DevOps pipelines

This folder holds the Azure Pipelines definitions for React Native Windows. For a
high-level tour of the stages and release flow, see
[docs/build-pipelines.md](../docs/build-pipelines.md). This README documents the
`.ado`-specific wiring that isn't obvious from the YAML: the Office/ISS template
setup and the network-isolation workarounds.

## Branch status

`0.84-stable` is retired. Pushes to it no longer run CI, release preparation, or
Release, so merged changes are not built or published. The PR pipeline still
validates PRs into this branch so that an emergency fix can be reviewed. See the
[supported branches](https://github.com/microsoft/react-native-windows/blob/main/.ado/README.md#supported-branches)
on `main`.

## Entry points

| Pipeline | Entry file | ADO project | 1ES template | Notes |
|----------|------------|-------------|--------------|-------|
| CI | `ci-pipeline.yml` | `ISS` | `Office.Official` | signed Official build; `trigger` branch filters and a weekly heartbeat `schedule` are defined in YAML |
| PR | `pr-pipeline.yml` | public (`ms`) | `1ES.Unofficial` | validation build for GitHub PRs |
| Release | `release-pipeline.yml` | `ISS` | `Office.Official` | triggered by CI completion; publishes packages and symbols |

CI and PR share all build/test/pack logic through `build-template.yml`; the
`buildEnvironment` parameter (`Continuous` vs `PullRequest`) gates the
environment-specific steps.

## Template-repository wiring (per-entry `PipelineTemplates` alias)

CI runs in the internal **ISS** project and extends the **Office** 1ES templates.
PR runs in the **public** project, which cannot reach `OfficePipelineTemplates`, so
it extends the **general** 1ES templates. Azure DevOps forbids a template
expression (`${{ if }}`) on a `resources.repositories` entry, so the template repo
cannot be selected conditionally inside the shared `build-template.yml`.

The resolution: each **entry** pipeline declares the repository itself, all under a
single alias `PipelineTemplates`:

- `ci-pipeline.yml` → `1ESPipelineTemplates/OfficePipelineTemplates`
- `pr-pipeline.yml` → `1ESPipelineTemplates/1ESPipelineTemplates`
- `release-pipeline.yml` → `OfficePipelineTemplates` (standalone; does not use `build-template.yml`)

`build-template.yml` then only selects the template **path** (a template expression
*is* allowed on `extends`):

- `Continuous` → `v1/Office.Official.PipelineTemplate.yml@PipelineTemplates`
- `PullRequest` → `v1/1ES.Unofficial.PipelineTemplate.yml@PipelineTemplates`

All repository resources pin `ref: refs/tags/release`.

## Agent pools

- **CI:** `fabric-internal-pool-large` (ISS), with image `rnw-img-vs2026-node24`
  selected via `demands: ImageOverride`.
- **PR:** public `rnw-pool-4` (default) and `rnw-pool-8` (native builds), same image.

## Package feed and network isolation

ISS agents run under 1ES **Network Isolation**, which blocks egress to public CDNs.
The pipelines route package requests through approved endpoints and neutralize
three background egress attempts that green builds would otherwise hide:

- **NuGet restores:** the root `NuGet.Config` maps all packages to the
  `ms/react-native-public` ADO feed, which has NuGet.org as an upstream and also
  hosts the V8 packages. A single upstream-enabled source avoids Secure Supply
  Chain Analysis's multiple-feed rejection without disabling the scanner.
  Anonymous restores require versions already saved in the feed. Feed warm-up
  runs from `main`; newly introduced versions on this branch may need a one-off
  warm using the pipeline's `packages` parameter.
  CI imports this policy for generated apps/libraries and third-party configs;
  CLI-init tests also retain their local feed for freshly built RNW packages.
  NuGet 6.0 or later is required to enforce these source mappings.
- **Package registries:** `variables/shared.yml` routes npm/npx and Yarn through
  the anonymous PackageFeedProxy (`NPM_CONFIG_REGISTRY` /
  `YARN_NPM_REGISTRY_SERVER`). On CI, `templates/auth-npm-feed.yml` mints a
  managed-identity token for direct access to the `ms/react-native-public`
  Azure Artifacts feed. The token is scoped to that feed in `~/.npmrc` and
  passed separately to the Verdaccio uplink; Yarn does not send it to the proxy.
- **Verdaccio (CLI init tests):** `verdaccio/generate-config.js` emits the config at
  runtime, so the uplink targets the feed (with the CI token when present) while
  local packages still publish offline. The former static `config.yaml` was removed.
- **React Native DevTools:** `RNDT_DEV: 1` stops DevTools from fetching its
  standalone shell binary from a Facebook CDN when Metro starts. The debugger is
  never opened in CI, so this has no effect on tests.
- **VS Installer:** `templates/prepare-build-env.yml` disables the VS Installer
  background auto-update, which would otherwise download updates from the Microsoft
  CDN mid-build. This is an interim step; the durable fix is to bake it into the
  agent image.

Anonymous NuGet restores and public PR Verdaccio requests to the Azure Artifacts
feed can only restore versions an authenticated identity has already saved. The
scheduled feed warm-up maintained on `main` keeps that feed populated.
npm/Yarn restores through PackageFeedProxy do not depend on that warm-up.
Publishing and authenticated feed maintenance keep their existing endpoints;
NuGet defaults shipped in generated apps/libraries are unchanged.

## SDL and Component Governance

The Office template runs SDL (CredScan, BinSkim, CodeQL, Component Governance,
ESLint). PREfast, SpotBugs, and Bandit are disabled (no actionable findings for
this repo). `guardian/sdl/.gdnsuppress` is an **empty** suppression set: all
Component Governance alerts are fixed at the source (dependency `resolutions` in the
root `package.json`), so nothing is suppressed.

## Release publishing targets

`release-pipeline.yml` defines these publishing targets from ISS:

- npm → npmjs.com via `EsrpRelease@14`
- npm and NuGet → `ms/react-native` (private) and `ms/react-native-public` (public) feeds
- NuGet → nuget.org via `EsrpRelease@14`
- PDB symbols → Microsoft Symbol Server (`templates/publish-symbols.yml`)

NuGet.org publishing stages only `Microsoft.ReactNative.*.nupkg` from the CI
`ReactWindows-final-nuget` artifact in a package-only ESRP submission folder.
ESRP publishes centrally, without a NuGet.org API key or direct NuGet.org access
from the release agent. Release jobs use 1ES Network Isolation in `Enforce` mode.

One item remains deferred: a more authoritative npm publish-eligibility check (the
current check reads the feed anonymously and sees only cached versions).
