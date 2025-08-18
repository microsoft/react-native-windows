### Type of Change
Automate release notes creation by adding a new yarn script. Automating the process of creating release notes so that we don't have to manually copy paste the commits. 


### Why
To save us some time when generating release notes. Fetches commit from start and end date range, ignores bots and creates the release notes md file. It also categorizes the commits. Please cross-check the generated release-notes.md file and update it manually if required like regrouping commits or updating the Summary/Explanation for the PR commit.

## Format

`Explanation. [PRName (#11168) · microsoft/react-native-windows@aaaaaaa (github.com)](link)`

### Steps to follow

#### 1. Set up your personal access token

- Go to GitHub and log in: https://github.com/
- Click on your profile picture (top-right corner), then click Settings
- On the left sidebar, click Developer settings
- Then click Personal access tokens > Tokens (classic)
- Click Generate new token > Generate new token (classic)
- Give it a name like "Release Notes Script"
- Set an expiration (choose less than 90 days)
- Under Scopes, select the permissions your script needs. For fetching commits and repo info, you typically need:
repo (full control of private repositories)
or at least repo:status, repo_deployment, public_repo (for public repos)
- Click Generate token
- Find the token you're using (whichever token you created).
- You should see a message or option to "Grant access to your organization" or "Authorize SAML SSO" for your token.
- Click that button to authorize the token with the organization.
- Copy the generated token

#### 2. Set variables as params and run "yarn release-notes"

```
Usage:
  yarn release-notes --token <GITHUB_TOKEN> --start <START_DATE> --end <END_DATE> [--repo <OWNER/REPO>] [--tag <RELEASE_TAG>]

Options:
  --token       (required) GitHub personal access token.
  --start       (required) Start date in YYYY-MM-DD.
  --end         (required) End date in YYYY-MM-DD.
  --repo        Repository in OWNER/REPO format. Default: microsoft/react-native-windows
  --tag         Release tag label. Default: Unreleased. Expected: 0.80 or similar
  --help        Show this help message.
```

#### 3. You will see a release-notes.md file generated at packages\@rnw-scripts\generate-release-notes\release_notes.md which will have all the data you need.

#### 4. Verify the release notes generated from all commits section and delete the "All Commits" and "Excluded" sections after validation
