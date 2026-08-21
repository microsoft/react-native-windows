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

## Scope and limitations

- Warms **latest patch per in-use line**, not a specific build's exact
  lockfile-pinned closure. A build pinning an older patch, or a version whose
  transitive graph differs, is not guaranteed by this pass alone.
- Cannot introduce a **brand-new package name** the feed has never seen (that name
  is not in the feed's list). First use is covered by the authenticated CI build
  that restores it, or by a one-off `--packages` warm.
- Does not resolve transitive closures (each warmed version is fetched on its own).
