# Build Pipelines

React Native Windows uses three Azure DevOps pipelines, all built on the 1ES Pipeline Templates.

## Pipeline Overview

| Pipeline | File | Template | Trigger |
|----------|------|----------|---------|
| **CI** | `ci-pipeline.yml` | 1ES Official | Push to `main`, `*-stable` |
| **PR** | `pr-pipeline.yml` | 1ES Unofficial | PRs to `main`, `*-stable` |
| **Release** | `release.yml` | 1ES Official | Triggered by successful CI run |

CI and PR share a single `build-template.yml` that contains all build, test, and packaging logic. The `buildEnvironment` parameter (`Continuous` or `PullRequest`) controls what runs:

- **Both**: build, test, lint, NuGet pack, CLI init verification
- **CI only**: ESRP code signing, release tagging
- **PR only**: beachball change file check

## Stages

```
Setup ──┬── Build ──── CLI
        │
        └── Tests
```

### Setup

Detects whether this is a **release build** (commit message starts with `RELEASE:`) or a **developer build**. For developer builds, runs beachball to bump versions. Publishes version variables for all downstream stages.

### Build

Runs in parallel for each platform/configuration in the build matrix:

- **Desktop** (x64/x86/ARM64EC, Debug/Release) — builds, runs unit and integration tests, signs shipping binaries (CI)
- **Universal** (x64/x86/ARM64, Debug/Release) — builds, runs unit tests, signs shipping binaries (CI)
- **Linting** — JS/TS lint, format check, override validation
- **NPM Pack** — creates npm tarballs
- **NuGet Pack** — downloads all build artifacts, packs NuGet packages, ESRP signs them (CI)

Each build job uses `*-single.yml` step templates that build the full `.sln`, run tests, and stage shipping artifacts — we test what we ship.

### Tests

Runs in parallel with Build (both depend only on Setup):

- **Node Tests** — Jest-based JavaScript tests
- **Playground** — builds playground apps across configs
- **E2E Tests** — Fabric end-to-end tests

### CLI

Depends on Build (needs the NuGet packages). Runs `react-native init` + `react-native-windows init` against the just-built packages to verify the developer experience works.

## Agent Pools

| Pool | Used By |
|------|---------|
| `rnw-pool-4-microsoft` | CI: most jobs (default) |
| `rnw-pool-8-microsoft` | CI: native builds (Desktop, Universal) |
| `rnw-pool-4` | PR: most jobs (default) |
| `rnw-pool-8` | PR: native builds |

PR uses public pools (no `-microsoft` suffix). The PR entry point overrides the pool defaults.

## SDL & Compliance

The 1ES Official template (CI) handles SDL automatically:

- **CredScan** — credential scanning
- **BinSkim** — binary security analysis (per-job targets via `templateContext.sdl`)
- **Component Governance** — open-source license compliance
- **CodeQL** — static analysis
- **ESLint** — JS/TS analysis (with exclusions)

PREfast, SpotBugs, and Bandit are disabled (no actionable findings for this repo).

## Release Flow

1. Developer merges a PR to `main`
2. **CI pipeline** runs: build, test, sign, pack
3. On success, a separate **prepare-release-bot** pipeline creates/updates a release PR (e.g., `prepare-release/main`)
4. When the release PR is merged (commit message: `RELEASE: ...`), CI runs again as a **release build**:
   - Skips beachball bump (versions already committed)
   - Builds, tests, signs, packs
   - Tags the source commit (`react-native-windows_v<version>`)
5. **Release pipeline** triggers on successful CI, publishing packages to npm and NuGet feeds

## Key Files

| File | Purpose |
|------|---------|
| `.ado/ci-pipeline.yml` | CI entry point |
| `.ado/pr-pipeline.yml` | PR entry point |
| `.ado/build-template.yml` | Shared build/test/pack logic |
| `.ado/release.yml` | Release pipeline (publishes to feeds) |
| `.ado/jobs/desktop-single.yml` | Desktop build + test steps |
| `.ado/jobs/universal-single.yml` | Universal build + test steps |
| `.ado/jobs/cli-init-windows.yml` | CLI init verification |
| `.ado/jobs/e2e-test.yml` | E2E test job |
| `.ado/jobs/playground.yml` | Playground build job |
| `.ado/jobs/linting.yml` | Linting job |
| `.ado/jobs/node-tests.yml` | Node/Jest test job |
| `.ado/templates/prep-and-pack-nuget.yml` | NuGet packing steps |
| `.ado/templates/esrp-codesign-*.yml` | ESRP signing steps |
