# React Native Windows Azure DevOps pipelines

This folder holds the Azure Pipelines definitions for React Native Windows. For a
high-level tour of the stages and release flow, see
[docs/build-pipelines.md](../docs/build-pipelines.md). This README documents the
`.ado`-specific wiring that isn't obvious from the YAML: the Office/ISS template
setup and the network-isolation workarounds.

## Entry points

| Pipeline | Entry file | ADO project | 1ES template | Notes |
|----------|------------|-------------|--------------|-------|
| CI | `ci-pipeline.yml` | `ISS` | `Office.Official` | signed Official build; `trigger` branch filters and a weekly heartbeat `schedule` are defined in YAML |
| PR | `pr-pipeline.yml` | public (`ms`) | `1ES.Unofficial` | validation build for GitHub PRs |
| Release | `release-pipeline.yml` | `ISS` | `Office.Official` | triggered by CI completion; publishes packages and symbols |
| Feed warm-up | `warm-feed-pipeline.yml` | `ISS` | `Office.Unofficial` | scheduled; keeps `ms/react-native-public` populated for anonymous PR restores |

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
- **VS Installer:** the `rnw-img-vs2026-node24` agent image sets the
  `BackgroundDownloadDisabled` policy so the VS Installer's background auto-update
  never contacts the Microsoft CDN mid-build. `templates/prepare-build-env.yml`
  reapplies the same policy per job as an interim belt until the updated image
  ships; it can be dropped once a rebuilt image is confirmed clean.

Public PR builds read this feed **anonymously**, so they can only restore versions
an authenticated identity has already saved. `warm-feed-pipeline.yml` (the
`@rnw-scripts/warm-feed` tool) runs on a schedule to re-pull the latest patch of
every npm/NuGet line already in the feed and keep those anonymous restores working.

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

The nuget.org publish runs 1ES Network Isolation in report-only mode
(`settings.networkIsolationMode: Audit`): `Enforce` blocks `api.nuget.org`, and the
per-domain allow-list is not plumbed for release jobs, so mode is the only
in-pipeline lever. Returning to `Enforce` requires adding nuget.org to the
`CFSClean` isolation policy for this pipeline (follow-up).

One item remains deferred: a more authoritative npm publish-eligibility check (the
current check reads the feed anonymously and sees only cached versions).
