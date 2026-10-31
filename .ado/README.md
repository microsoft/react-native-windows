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

Feed warm-up runs as a separate scheduled pipeline maintained centrally on `main`
(it is not duplicated on release branches).

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

Feed warm-up and the branch-namespace ruleset are maintained centrally on `main`
and are not duplicated on release branches: a scheduled warm-up reproduces the
`create-react-native-library` CLI-init graph **per built release branch** to keep
the `ms/react-native-public` feed populated, and branch *creation* is governed
repo-wide by a ruleset. Keep this branch table in sync with the warm-up's per-branch
closure list on `main`.

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
  BinSkim downloads still use PackageFeedProxy instead of nuget.org.
- **Package registries:** `variables/shared.yml` routes npm/npx and Yarn through
  the anonymous PackageFeedProxy (`NPM_CONFIG_REGISTRY` /
  `YARN_NPM_REGISTRY_SERVER`) in both PR and Continuous validation.
  `templates/auth-npm-feed.yml` keeps the managed-identity token scoped to direct
  Azure Artifacts requests in `~/.npmrc` and passes it to the authenticated CI
  Verdaccio uplink; Yarn does not send the token to the proxy.
- **Verdaccio (CLI init tests):** `verdaccio/generate-config.js` emits the config at
  runtime. Public PRs use the anonymous proxy; authenticated CI retains the ADO
  feed uplink. Local packages still publish offline. The file logger uses
  Verdaccio's current `log` setting so failure artifacts include `verdaccio.log`.
  The former static `config.yaml` was removed.
- **React Native DevTools:** `RNDT_DEV: 1` stops DevTools from fetching its
  standalone shell binary from a Facebook CDN when Metro starts. The debugger is
  never opened in CI, so this has no effect on tests.
- **VS Installer:** the `rnw-img-vs2026-node24` agent image sets the
  `BackgroundDownloadDisabled` policy so the VS Installer's background auto-update
  never contacts the Microsoft CDN mid-build. `templates/prepare-build-env.yml`
  reapplies the same policy per job as an interim belt until the updated image
  ships; it can be dropped once a rebuilt image is confirmed clean.

npm/Yarn restores through PackageFeedProxy are **anonymous** and do not depend on
the RNW feed warm-up. Anonymous NuGet restores and the authenticated CI Verdaccio
uplink use the ADO feed, which the scheduled warm-up maintained on `main` populates.
Publishing and authenticated feed maintenance keep their existing endpoints;
NuGet defaults shipped in generated apps/libraries are unchanged.

## Temporary extract-zip mitigation

The root Yarn resolutions apply `.yarn/patches/extract-zip-npm-2.0.1-92a28e392b.patch`
to the repository's `extract-zip` consumers for
[CVE-2026-19693](https://github.com/advisories/GHSA-7pqw-9j4j-h8q3).
It rejects a regular archive entry whose destination is already a symbolic link,
including dangling links, before opening the write stream. Ordinary file overwrites
and legitimate symbolic-link archive entries are preserved; writes through even
an in-directory symbolic-link leaf are intentionally rejected.

`yarn test` runs `.ado/tests/extract-zip.test.cjs` before the package suites.
Real symlink tests require Windows Developer Mode or symlink privileges; the
mocked leaf/error checks run regardless. macOS can run the real symlink cases
without that Windows requirement.

There is no patched upstream release as of October 28, 2026. This is a
repository-local mitigation, not a version upgrade or an alert suppression.
It does not propagate to consumers installing published RNW packages through npm.
CG may continue reporting version 2.0.1; track scanner disposition separately.
Replace the patch with an upstream release once available and validated.

## SDL and Component Governance

The Office template runs SDL (CredScan, BinSkim, CodeQL, Component Governance,
ESLint). PREfast, SpotBugs, and Bandit are disabled (no actionable findings for
this repo). `guardian/sdl/.gdnsuppress` is an **empty** suppression set.
Dependency remediations, including root `package.json` resolutions, do not by
themselves establish scanner clearance; track the corresponding CG alerts separately.

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
