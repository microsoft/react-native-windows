# Repository rules (rulesets)

Infrastructure-as-code for the GitHub **repository rulesets** applied to
`microsoft/react-native-windows`. Each `*.json` file in this folder is the exact
request body for GitHub's create/update ruleset REST API, so the live policy
always has a reviewable history in git. Edit the JSON, get it reviewed, then
apply it with the commands below.

## Files

| File | Ruleset name | Purpose |
|------|--------------|---------|
| [`branch-namespace-ruleset.json`](./branch-namespace-ruleset.json) | Require namespaced branches | Stops new **top-level** branches. New branches must live under an approved namespace (personal work goes under `user/<alias>/…`). |

## What `branch-namespace-ruleset.json` does

- Targets **all branches** (`~ALL`) *except* the approved patterns under
  `conditions.ref_name.exclude`.
- Applies the **`creation`** rule ("Restrict creations"): a branch whose name is
  not in the allow list cannot be created by anyone without bypass permission.
- **Creation only.** Existing branches and pushes to them are untouched — this
  refuses only *new* non-conforming branch names. The ~90 legacy top-level
  branches keep working; nobody can add more like them.
- `bypass_actors` is empty on purpose: **no one is exempt**, not even repo
  admins. (An admin can still edit or delete the ruleset itself.)

### Approved branch namespaces

| Branch pattern | Who / what uses it |
|----------------|--------------------|
| `user/<alias>/…` | Humans — the required home for personal and PR branches |
| `users/merlinbot/…` | 1ES PT auto-baselining bot |
| `dependabot/…` | Dependabot |
| `copilot/…` | Copilot coding agent |
| `chore/weekly-lock-refresh-*` | `compliance-yarn-lock.yml` weekly bot |
| `*-stable` | Release branches (e.g. `0.85-stable`) |
| `preview-*-test` | Preview test branches (e.g. `preview-0.84-test`) |
| `prepare-release/…` | Release preparation |
| `release/…`, `hotfix/…`, `archive/…` | Reserved by the [branch lifecycle policy](../../docs/branch-lifecycle-policy.md) (`archive/` is the documented way to preserve a branch) |
| `integrate/…` | React Native nightly integration (`integrate/nightly-*`) |
| `cherry-pick/…` | Cherry-pick branches |
| `revert-*` | GitHub "Revert" button on merged PRs |
| `gh-readonly-queue/…` | Merge queue (only if enabled) |
| `gh-pages` | Docs site publishing |

### Pattern syntax & gotchas

GitHub matches ref patterns with Ruby's `File.fnmatch` under the
`File::FNM_PATHNAME` flag. It has sharp edges — each point below was verified
against that engine:

- **A single `*` never crosses `/`.** `foo/*` matches exactly one level
  (`foo/bar`), never `foo/a/b`. Inside a single segment `*` is greedy, so
  `*-stable` matches both `0.85-stable` and `0.58-vnext-stable`, and
  `preview-*-test` requires the literal `-test` suffix (plain `preview-0.84` is
  blocked).
- **A bare trailing `**` does *not* cross `/` either.** `foo/**` behaves like
  `foo/*` and silently misses nested branches — never use it to mean "everything
  under `foo/`".
- **Use `foo/**/*` for a whole namespace.** The `**/` piece matches *zero or
  more* directories, so a single `foo/**/*` matches both single-level (`foo/bar`)
  and deep (`foo/a/b/c`). That is why each folder namespace needs only this one
  entry.
- **`~ALL` — not `refs/heads/**` — means "all branches".** The `include` uses the
  special `~ALL` token; a trailing `**` cannot express it (see above).
- **Every `exclude` entry keeps the `refs/heads/` prefix**; patterns match the
  full ref path, not the short branch name.
- **Reverts can contain slashes.** GitHub's "Revert" button creates
  `revert-<PR#>-<original-branch>` and preserves slashes when the original was
  namespaced (e.g. `revert-15633-user/WatsonFix/0.81`). That is why revert takes
  two entries: `revert-*` (flat) and `revert-*/**/*` (nested).
- **Matching is case-sensitive.** `fix-*` does not match `Fix-lint`.
- **Unsupported syntax:** no negated character classes (`[^…]`), no brace/extglob
  expansion (`{a,b}`, `File::FNM_EXTGLOB`), and backslash is not an escape
  character.
- **Creation-only.** The rule blocks new non-conforming branch *names*; it never
  touches existing branches or pushes to them.

## Prerequisites

1. **Admin on the repo.** Rulesets are an admin-only setting. Elevate through
   just-in-time (JIT) admin access to get the **Admin** role on
   `microsoft/react-native-windows`, and drop it when you're done.
2. **GitHub CLI**, authenticated as the elevated account:
   ```powershell
   gh --version
   gh auth login   # or set $env:GH_TOKEN
   ```

## Apply (first time)

```powershell
cd .ado/repo-rules
gh api --method POST repos/microsoft/react-native-windows/rulesets --input branch-namespace-ruleset.json
```

## Update an existing ruleset

Look up the ruleset id, then `PUT` the edited file:

```powershell
gh api repos/microsoft/react-native-windows/rulesets --jq '.[] | "\(.id): \(.name)"'
gh api --method PUT repos/microsoft/react-native-windows/rulesets/RULESET_ID --input branch-namespace-ruleset.json
```

## Change the allowed patterns

Edit `conditions.ref_name.exclude` in the JSON, commit for review, then run the
**update** command above. Add a single `refs/heads/foo/**/*` entry per folder
namespace — it matches that namespace at any depth. Mind the
[pattern gotchas](#pattern-syntax--gotchas): keep the `refs/heads/` prefix and
never use a bare `foo/**` (it will not cross `/`).

## Verify

```powershell
# A non-namespaced name should be rejected:
git push upstream HEAD:policy-smoke-test

# A namespaced name should succeed, then clean it up:
git push upstream HEAD:user/<your-alias>/policy-smoke-test
git push upstream --delete user/<your-alias>/policy-smoke-test
```

Or inspect which rules would apply to a candidate branch name (it need not exist):

```powershell
gh api repos/microsoft/react-native-windows/rules/branches/policy-smoke-test
```

## Roll back

```powershell
# Soft-disable: set "enforcement": "disabled" in the JSON and PUT it, or delete it:
gh api --method DELETE repos/microsoft/react-native-windows/rulesets/RULESET_ID
```

## Notes

- To trial the policy without blocking anyone, set `"enforcement": "evaluate"`
  first (violations are logged, not enforced), then switch to `"active"`.
- The allow list was derived from the repo's live branches plus the tooling that
  creates them: Dependabot, Copilot, [`compliance-yarn-lock.yml`](../../.github/workflows/compliance-yarn-lock.yml),
  [`integrate-rn`](../../packages/@rnw-scripts/integrate-rn), the release process,
  and the protected prefixes in the [branch lifecycle policy](../../docs/branch-lifecycle-policy.md).
- If you later want an emergency bypass, add an entry to `bypass_actors` (for
  example an `OrganizationAdmin` or a specific `Team`) rather than loosening the
  patterns.
