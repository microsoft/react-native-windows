# Branch Lifecycle Policy

This repository enforces a branch lifecycle policy to keep the branch list clean and manageable. Stale branches are identified automatically and removed after a period of inactivity.

## How It Works

A scheduled GitHub Action ([cleanup-stale-branches.yml](../.github/workflows/cleanup-stale-branches.yml)) runs **every Monday at 08:00 UTC** and scans all remote branches for inactivity.

| Parameter | Default | Description |
|-----------|---------|-------------|
| Stale threshold | **90 days** | Branches with no commits in this period are considered stale |
| Dry run (scheduled) | **true** | Scheduled runs only report; manual dispatch can delete |

## Protected Branches

The following branch patterns are **always excluded** from cleanup:

- `main` / `master`
- `develop`
- `release/*`
- `hotfix/*`
- `archive/*`

## How to Keep a Branch

If your branch must be retained beyond the staleness threshold (e.g., for archival or long-running work), use **any** of these methods:

### 1. Use a protected prefix
Move or rename your branch under `archive/`:
```
git branch -m my-old-branch archive/my-old-branch
git push origin archive/my-old-branch :my-old-branch
```

### 2. Tag the last commit message
Include one of these markers in any commit message on the branch:
- `[archive]`
- `[keep]`
- `[retain]`
- `[no-cleanup]`

Example:
```
git commit --allow-empty -m "[keep] Retaining branch for future reference"
git push
```

### 3. Notify maintainers
Open an issue or message the maintainers with the branch name and reason for retention. They can add it to the protected patterns in the workflow file.

## Manual Cleanup

Maintainers can trigger the workflow manually from the **Actions** tab:

1. Go to **Actions → Cleanup Stale Branches**
2. Click **Run workflow**
3. Choose `dry_run = false` to actually delete stale branches
4. Optionally adjust the staleness threshold (in days)

## FAQ

**Q: What happens if my branch is deleted by mistake?**
A: Git branch deletions on GitHub are reversible for a short period. You can also restore from a local clone if you still have the branch locally. See [GitHub docs on restoring deleted branches](https://docs.github.com/en/repositories/configuring-branches-and-merges-in-your-repository/managing-branches-in-your-repository/restoring-a-deleted-branch).

**Q: Will this delete branches with open pull requests?**
A: The workflow deletes by inactivity date only. If you have an open PR from a stale branch, the branch may still be deleted. Keep the branch active or mark it with `[keep]` to prevent this.

**Q: Can I change the staleness threshold?**
A: Yes — edit `DEFAULT_STALE_DAYS` in the workflow file, or override it per-run via the manual dispatch input.
