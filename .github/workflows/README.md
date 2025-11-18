# GitHub Actions Workflows

This directory contains GitHub Actions workflows for the React Native Windows repository.

## Available Workflows

### Cherry-pick Commit to Branch

**File:** `cherry-pick-commit.yml`

Cherry-picks a specific commit into a target branch. You can choose to either apply the cherry-pick directly to the target branch or create a Pull Request for review.

**Usage:**

1. Go to the Actions tab in the GitHub repository
2. Select "Cherry-pick Commit to Branch" workflow
3. Click "Run workflow"
4. Enter the required inputs:
   - **Commit SHA or ID**: The full commit hash or short SHA of the commit to cherry-pick
   - **Target branch name**: The branch where you want to cherry-pick the commit
   - **Create a PR instead of direct cherry-pick**: Check this box to create a Pull Request instead of directly applying the cherry-pick

**Workflow Options:**

- **Direct Cherry-pick** (checkbox unchecked): The commit will be cherry-picked and pushed directly to the target branch
- **Pull Request** (checkbox checked): Creates a new branch with the cherry-picked commit and opens a Pull Request to the target branch for review

**Example:**

- Commit ID: `d1a95351e5203a6c0651cf73885cd7ea99e7d2b9`
- Target branch: `0.79-stable`
- Create PR: ✅ (checked) - Creates a PR for review
- Create PR: ❌ (unchecked) - Applies cherry-pick directly

**Notes:**

- The workflow will fail if there are merge conflicts during cherry-pick
- If conflicts occur, you'll need to resolve them manually
- When creating a PR, the branch name will be automatically generated as `cherry-pick-<commit-id>-to-<target-branch>`, where `<commit-id>` is the full commit hash (e.g., `cherry-pick-d1a95351e5203a6c0651cf73885cd7ea99e7d2b9-to-0.79-stable`). This may result in long branch names.
- When creating a PR, the title and description will include details about the original commit and target branch
