# React Native Windows Azure DevOps pipelines

This folder holds the Azure Pipelines definitions for React Native Windows. For a
high-level tour of the stages and release flow, see
[docs/build-pipelines.md](../docs/build-pipelines.md). This README documents the
`.ado`-specific wiring that isn't obvious from the YAML: the Office/ISS template
setup and the network-isolation workarounds.

## Entry points

| Pipeline | Entry file | ADO project | 1ES template | Notes |
|----------|------------|-------------|--------------|-------|
| CI | `ci-pipeline.yml` | `ISS` | `Office.Official` | signed Official build; `trigger` branch filters and a monthly keep-alive `schedule` on `main` are defined in YAML |
| PR | `pr-pipeline.yml` | public (`ms`) | `1ES.Unofficial` | validation build for GitHub PRs; monthly keep-alive on `main` |
| Release | `release-pipeline.yml` | `ISS` | `Office.Official` | triggered by CI completion; publishes packages and symbols |
| Feed warm-up | `warm-feed-pipeline.yml` | `ISS` | `Office.Unofficial` | scheduled; keeps `ms/react-native-public` populated for anonymous PR restores |

CI and PR share all build/test/pack logic through `build-template.yml`; the
`buildEnvironment` parameter (`Continuous` vs `PullRequest`) gates the
environment-specific steps.

## Supported branches

CI, PR, Release, and the prepare-release bot all run against the same set of
branches, all driven from YAML. Keep the branch lists in `ci-pipeline.yml`,
`pr-pipeline.yml`, `release-pipeline.yml`, and `prepare-release-bot.yml` (its
`trigger` and the `targetBranch` dropdown) in sync with this table.

| Branch | Status |
|--------|--------|
| `main` | Active development |
| `0.81-stable` | LTS — last release supporting both Fabric and Paper UI |
| `0.83-stable` | Latest release − 1 |
| `0.84-stable` | Current release |
| `0.85-stable` | Preview |
| `0.86-stable` | In upstream React Native, not yet integrated |
| `0.87-stable` | In upstream React Native, not yet integrated |

`0.82-stable` is deprecated and removed from all pipelines. `0.80-stable` and
earlier are not built.

Feed warm-up (`warm-feed-pipeline.yml`, scheduled on `main`) has two passes: the
latest-patch enumeration is branch-agnostic (feed-centric), while the closure pass
reproduces the `create-react-native-library` CLI-init graph **per built release
branch**. That branch list lives in
[`warm-feed.config.json`](../packages/@rnw-scripts/warm-feed/warm-feed.config.json)
(`closure.modules.create-react-native-library.branches`) — keep it in sync with the
built branches above (it omits the not-yet-integrated ones). Branch *creation* is
governed separately by the ruleset in
[`repo-rules/`](./repo-rules/README.md).

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
The pipelines use approved package feeds and neutralize three background egress
attempts that green builds would otherwise hide:

- **NuGet restores:** the root `NuGet.Config` maps NuGet.org dependencies to
  `https://packagefeedproxy.microsoft.io/nuget/v3/index.json` anonymously.
  Only `ReactNative.V8Jsi.Windows*` and `Microsoft.JavaScript.V8*` use the
  `ms/react-native-public` ADO feed, where these V8 packages are published.
  CI imports this policy for generated apps/libraries and third-party configs;
  CLI-init tests also retain their local feed for freshly built RNW packages.
  NuGet 6.0 or later is required to enforce these source mappings.
  BinSkim downloads use the same proxy instead of nuget.org.
- **Package registries:** `variables/shared.yml` routes npm/npx and Yarn through
  the anonymous PackageFeedProxy (`NPM_CONFIG_REGISTRY` /
  `YARN_NPM_REGISTRY_SERVER`) in both PR and Continuous validation.
  `templates/auth-npm-feed.yml` keeps the managed-identity token scoped to direct
  Azure Artifacts requests in `~/.npmrc` and passes it to the Verdaccio uplink;
  Yarn does not send the token to the proxy.
- **Dependabot:** `.github/dependabot.yml` configures the same anonymous proxy
  separately; Dependabot does not inherit the pipeline variables. Its registry
  declarations also allow the exact `ms-feed-*` hosts advertised by proxy
  metadata for tarball downloads. Existing archive URLs in the main lockfile
  also use proxy paths without changing dependency versions or checksums.
  Major version updates are ignored before grouping. React, React Native, the
  test renderer, and React types also ignore minor updates to stay on each
  branch's selected release lines; exact versions remain manifest-controlled.
- **Verdaccio (CLI init tests):** `verdaccio/generate-config.js` emits the config at
  runtime, so the uplink uses the proxy for PRs and the ADO feed with the CI token while
  local packages still publish offline. The former static `config.yaml` was removed.
- **React Native DevTools:** `RNDT_DEV: 1` stops DevTools from fetching its
  standalone shell binary from a Facebook CDN when Metro starts. The debugger is
  never opened in CI, so this has no effect on tests.
- **VS Installer:** the `rnw-img-vs2026-node24` agent image sets the
  `BackgroundDownloadDisabled` policy so the VS Installer's background auto-update
  never contacts the Microsoft CDN mid-build. `templates/prepare-build-env.yml`
  reapplies the same policy per job as an interim belt until the updated image
  ships; it can be dropped once a rebuilt image is confirmed clean.

PackageFeedProxy restores are **anonymous** and do not depend on the RNW feed
warm-up. The scheduled warm-up maintained on `main` still populates the ADO feed
for its remaining consumers. Publishing and authenticated feed maintenance keep
their existing endpoints; NuGet defaults shipped in generated apps/libraries
are unchanged.

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
