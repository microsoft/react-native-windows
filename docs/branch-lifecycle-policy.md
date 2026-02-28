# Branch Lifecycle Policy

This repository enforces a branch lifecycle policy to keep the branch list clean and manageable. Stale branches are identified automatically and removed after a period of inactivity.

## How It Works

A scheduled GitHub Action ([cleanup-stale-branches.yml](../.github/workflows/cleanup-stale-branches.yml)) runs **every Monday at 08:00 UTC** and scans all remote branches for inactivity.

### Staleness Thresholds

| Branch type | Stale after | Examples |
|-------------|-------------|----------|
| `copilot/*` | **90 days** | `copilot/fix-xyz`, `copilot/workspace` |
| All other branches | **180 days** | `user/feature`, `abhi/experiment` |

Scheduled runs are always **dry run** (report only). Deletions require a manual trigger.

## Protected Branches

The following branch patterns are **always excluded** from cleanup, regardless of age:

- `main` / `master`
- `develop`
- `release/*`
- `hotfix/*`
- `archive/*`
- `*-stable` (e.g., `0.72-stable`, `0.80-stable`)
- `preview-*` (e.g., `preview-0.80-test`)

## Additional Safeguards

- **Open PRs** — Branches with any open pull request (including drafts) are automatically skipped.

## How to Keep a Branch

If your branch must be retained beyond the staleness threshold, use **any** of these methods:

### 1. Use a protected prefix
Rename your branch under `archive/`:
```
git branch -m my-old-branch archive/my-old-branch
git push origin archive/my-old-branch
git push origin --delete my-old-branch
```

### 2. Keep an open PR
Create or keep a pull request (even a draft) from your branch. The workflow skips any branch with an open PR.

### 3. Add the pattern to the workflow
Add your branch prefix to the `PROTECTED` regex in the workflow file and submit a PR.

## Manual Cleanup

Maintainers can trigger the workflow manually from the **Actions** tab:

1. Go to **Actions → Cleanup Stale Branches**
2. Click **Run workflow**
3. Set `dry_run = false` to actually delete stale branches

## FAQ

**Q: What happens if my branch is deleted by mistake?**
A: Git branch deletions on GitHub are reversible for a short period. You can also restore from a local clone. See [GitHub docs on restoring deleted branches](https://docs.github.com/en/repositories/configuring-branches-and-merges-in-your-repository/managing-branches-in-your-repository/restoring-a-deleted-branch).

**Q: Will this delete branches with open pull requests?**
A: No. The workflow checks for open PRs (including drafts) and skips those branches.

**Q: What about stable and preview branches?**
A: All `*-stable` and `preview-*` branches are protected by default and will never be deleted.
