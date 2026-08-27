# @rnw-scripts/warm-feed

Warms the `ms/react-native-public` Azure DevOps upstream feed so that
network-isolated PR/CI builds — which read the feed **anonymously** — can restore
the package versions they need.

An Azure Artifacts upstream feed only serves a version once an **authenticated**
identity has pulled it from upstream (which *saves* it into the feed). This tool
performs those authenticated pulls.

## How it works

The warmer is **feed-centric** — it needs no repo checkouts or lockfiles, so one
run serves every repo that shares the feed:

1. **Enumerate** the feed with the ADO Get Packages API — every npm and NuGet
   package already in the feed, plus each package's already-saved versions.
2. **Expand** each package to the latest stable patch of every `major.minor`
   line already in use (and, by default, the newest stable version overall). The
   candidate versions come from the authenticated packument / flat2 index, which
   includes not-yet-saved upstream versions.
3. **Skip** versions the feed already has (the enumeration result is the
   authoritative "already cached" set — no separate cache is needed).
4. **Warm** the rest with an authenticated GET of the tarball / `.nupkg`, which
   triggers the upstream save. The response body is streamed and discarded.

## Local usage

Auth is resolved in order: `--pat`, `$env:ADO_PAT` / `$env:AZURE_DEVOPS_EXT_PAT`,
`$env:WARM_FEED_TOKEN`, then an AAD token from `az account get-access-token`
(run `az login` first).

```powershell
# Preview what a full warm would do (no writes):
yarn warm-feed --dry-run

# Preview just one ecosystem:
yarn warm-feed --dry-run --only nuget

# Warm a specific version on demand (one-off; skips enumeration):
yarn warm-feed --packages "npm:left-pad@1.3.0"
yarn warm-feed --packages "nuget:Newtonsoft.Json@13.0.3"

# Warm everything the feed needs (mutates the feed):
yarn warm-feed
```

`yarn warm-feed` builds the package and then runs it. Common flags: `--only
npm|nuget`, `--dry-run`, `--verify` (warm even already-cached targets),
`--concurrency <n>`, `-v` / `--verbose`. See `--help` for the full list.

## Closure (graph) warming

Enumeration keeps **known** package lines fresh but can't introduce a **brand-new**
name the feed has never seen — e.g. the packages a bumped
`create-react-native-library` pulls into a generated project. Closure warming
closes that gap: it resolves a dependency **graph** and warms every version in it.

The graph is resolved with `npm install --package-lock-only` (metadata only; the
feed save still happens per version). The lockfile covers the **resolve host's own
platform** (Windows x64 in CI). It can also list other-OS/CPU optional variants, but
not dependably against an Azure Artifacts upstream proxy: npm 11+ writes
not-yet-cached, non-current-platform optional deps without a `version`
(npm/cli#9342) and the parser drops version-less entries. Warming every platform's
optional deps would need a per-OS/CPU resolve.

Three ways to feed it a graph:

```powershell
# 1. Roots — warm the full npm graph of one or more packages:
yarn warm-feed --closure npm:create-react-native-library@0.63.0

# 2. Manifest — warm the external-dependency graph of a package.json:
yarn warm-feed --closure-manifest ./some/package.json

# 3. Special module — a registered reproducer for a case enumeration can't see:
yarn warm-feed --closure-module create-react-native-library
yarn warm-feed --closure-module all      # every enabled module
```

### Special modules (pluggable)

A **special module** reproduces a specific install closure and turns it into a
dependency graph to warm. They live in `src/specialModules/` and are registered
in `src/specialModules/index.ts`; add a module there and it's available to config
(`closure.modules.<name>`) and `--closure-module <name>`.

The first module, **`create-react-native-library`**, reproduces the CLI-init lib
test: the test scaffolds a library (+ vanilla example app) and installs *that
generated project*, so its closure — not cRNL's own dependencies — is what the
feed needs. warm-feed runs only from `main`, but the test runs on every release
branch pinned to a different React Native, so the module's config manifest lists
all branches and, per branch, derives the RN/CLI versions (nightly for `main`
from the working-tree `vnext/package.json`; latest stable `0.NN.x` for
`0.NN-stable`), scaffolds, and reads the generated manifests. Mirrors
`vnext/Scripts/creaternwlib.cmd`.

The scheduled pipeline run warms every **enabled** configured module in addition
to the latest-patch sync, so brand-new closures stay warm automatically.

### NuGet lock closure

The special modules above are npm-only. NuGet has the same gap — enumeration only
refreshes lines already in the feed — for which the repo's committed
`packages.lock.json` files are the source of truth: they pin the full resolved
NuGet closure (incl. transitives) every project restores. warm-feed scans them and
warms every `name@resolved` they list, so a brand-new NuGet package (or an exact
non-latest pinned version) restores under isolation.

The scheduled pass does this automatically (unless `closure.nugetLocks.enabled` is
`false`, or `--only npm`); `--nuget-locks` runs just this pass one-off. Scanning is
local (no feed access), so it is included in a `--dry-run` plan. By default it
scans the repo root (`--repo-root`, default cwd), pruning `node_modules`; narrow it
with `closure.nugetLocks.roots`.

## Pipeline usage

`.ado/warm-feed-pipeline.yml` runs the tool on a schedule (and on manual queue)
in the office/ISS project, on the isolated pool, extending the Office Unofficial
1ES template. An `AzureCLI@2` step logs in as the managed identity and mints its
feed token into `WARM_FEED_TOKEN`, then runs `npx warm-feed`.

Warming runs **out of band** — never in a PR build — because saving into the feed
needs the managed identity. To warm a version a PR/Dependabot introduced, a
maintainer queues the pipeline with the `packages` parameter
(`npm:foo@1.2.3 nuget:Bar@4.0.0`); the rights gate is ADO "Queue builds".

## Config

`warm-feed.config.json`:

| Key | Meaning |
| --- | --- |
| `feeds.npm.registry` / `feeds.nuget.index` | Feed endpoints to warm. |
| `enumerate.feedManagementBase` | Feed Management base (`https://feeds.dev.azure.com/<org>/<project>/_apis/packaging/Feeds/<feed>`). |
| `enumerate.apiVersion` / `enumerate.pageSize` | Get Packages API version and page size. |
| `expand.scope` | `in-use-lines` (latest patch per `major.minor`) or `in-use-majors` (latest per major). |
| `expand.includeLatest` | Also warm the newest stable version overall. |
| `expand.includePrerelease` | Include prerelease versions. |
| `expand.maxMajorsBack` | Limit to the N most-recent majors already in use (0 = no limit). |
| `concurrency` | Parallel requests. |
| `ignore` | `id`, `id@version`, or `eco:id@version` entries to skip. |
| `closure.registry` | npm registry for closure resolution (defaults to `feeds.npm.registry`). |
| `closure.modules` | Per-module config blocks (e.g. `create-react-native-library`), keyed by module name; each may set `enabled: false`. |
| `closure.nugetLocks` | NuGet `packages.lock.json` closure: `enabled` (default true) and `roots` (repo-relative dirs to scan; default repo root). |

## Scope and limitations

- The enumeration pass warms **latest patch per in-use line**, not a specific
  build's exact lockfile-pinned closure. A build pinning an older patch is not
  guaranteed by that pass alone — use closure warming for exact graphs.
- Enumeration cannot introduce a **brand-new package name** the feed has never
  seen, nor an exact non-latest pinned version. Covered instead by closure warming:
  for **npm** a special module, `--closure`, or `--closure-manifest`; for **NuGet**
  the `packages.lock.json` closure (above). The authenticated CI build that first
  restores a package, or a one-off `--packages` warm, also cover it.
- Closure resolution needs `npm` on `PATH` (bundled with Node) and, for special
  modules that scaffold, network access to the feed for the generator.
