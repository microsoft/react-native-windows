# GitHub Actions Workflows

This directory contains GitHub Actions workflows for the React Native Windows repository.

## Available Workflows

### Cherry-pick Commit to Branch

**File:** `cherry-pick-commit.yml`

Cherry-picks a specific commit into a target branch.

**Usage:**

1. Go to the Actions tab in the GitHub repository
2. Select "Cherry-pick Commit to Branch" workflow
3. Click "Run workflow"
4. Enter the required inputs:
   - **Commit SHA or ID**: The full commit hash or short SHA of the commit to cherry-pick
   - **Target branch name**: The branch where you want to cherry-pick the commit

**Example:**

- Commit ID: `d1a95351e5203a6c0651cf73885cd7ea99e7d2b9`
- Target branch: `0.79-stable`

**Notes:**

- The workflow will fail if there are merge conflicts during cherry-pick
- If conflicts occur, you'll need to resolve them manually
- The commit will be pushed directly to the target branch after successful cherry-pick
### Trigger AZP on Dependabot PRs

**File:** `dependabot-azp-trigger.yml`

Automatically comments `/azp run PR` on pull requests opened or updated by Dependabot, triggering Azure Pipelines CI without manual intervention. The comment is posted using rnbot's PAT (`RNBOT_GITHUB_PAT`) so AZP recognizes the trigger.

**Trigger:** Runs on `pull_request_target` events (`opened`, `synchronize`, `reopened`) when the PR author is `dependabot[bot]`.

**Setup:** Requires a `RNBOT_GITHUB_PAT` GitHub Actions secret containing rnbot's personal access token with `repo` scope.

**Notes:**

- Checks `github.event.pull_request.user.login` (not `github.actor`) so it still triggers when a maintainer clicks "Update branch"
- Fires on `opened`, `synchronize` (new pushes/rebases), and `reopened` events