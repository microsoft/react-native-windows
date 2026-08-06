# React Native Windows Azure DevOps pipelines

This folder holds the Azure Pipelines definitions for React Native Windows. For a
high-level tour of the stages and release flow, see
[docs/build-pipelines.md](../docs/build-pipelines.md). This README documents the
`.ado`-specific wiring that isn't obvious from the YAML: the Office/ISS template
setup and the network-isolation workarounds.

## Entry points

| Pipeline | Entry file | ADO project | 1ES template | Notes |
|----------|------------|-------------|--------------|-------|
| CI | `ci-pipeline.yml` | `ISS` | `Office.Official` | signed Official build; `trigger`/`pr` are set in the ADO definition |
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
The pipelines source packages only from the `ms/react-native-public` Azure Artifacts
feed and neutralize three background egress attempts that green builds would
otherwise hide:

- **Feed registry:** `variables/shared.yml` points npm and Yarn at the
  `ms/react-native-public` feed (`NPM_CONFIG_REGISTRY` / `YARN_NPM_REGISTRY_SERVER`).
  On CI, `templates/auth-npm-feed.yml` mints a managed-identity token and writes it
  to `~/.npmrc` and `~/.yarnrc.yml` before any install.
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

## SDL and Component Governance

The Office template runs SDL (CredScan, BinSkim, CodeQL, Component Governance,
ESLint). PREfast, SpotBugs, and Bandit are disabled (no actionable findings for
this repo). `guardian/sdl/.gdnsuppress` is an **empty** suppression set: all
Component Governance alerts are fixed at the source (dependency `resolutions` in the
root `package.json`), so nothing is suppressed.

## Release publishing targets

`release-pipeline.yml` publishes from ISS to all five destinations:

- npm → npmjs.com via `EsrpRelease@11`
- NuGet → `ms/react-native` (private) and `ms/react-native-public` (public) feeds
- NuGet → nuget.org (API key from `OGX-JSHost-KV`)
- PDB symbols → Microsoft Symbol Server (`templates/publish-symbols.yml`)

Two Release items are intentionally deferred to a follow-up change: the private-feed
NuGet service connection (a placeholder `endpointId` remains in
`release-pipeline.yml`) and a more authoritative publish-eligibility check.
