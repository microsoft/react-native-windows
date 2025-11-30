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

---

### Create Branch from Upstream

**File:** `sync-upstream-branch.yml`

Creates a new branch in your fork based on a branch from the upstream microsoft/react-native-windows repository.

**Usage:**

1. Go to the Actions tab in your fork
2. Select "Create Branch from Upstream" workflow
3. Click "Run workflow"
4. Enter the required inputs:
   - **Upstream branch name**: The branch from microsoft/react-native-windows to base your new branch on (e.g., `0.81-stable`, `main`)
   - **New branch name** (optional): The name for the new branch in your fork. If left empty, the same name as the upstream branch will be used

**Example:**

- Upstream branch: `0.81-stable`
- New branch name: `my-feature-0.81` (or leave empty to create `0.81-stable` in your fork)

**Notes:**

- The workflow will fail if the specified upstream branch does not exist
- The workflow will fail if the branch already exists in your fork (to prevent overwriting)
- This is useful for creating feature branches based on specific upstream stable releases
