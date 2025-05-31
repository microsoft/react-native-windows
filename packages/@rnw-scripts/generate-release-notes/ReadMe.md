### Type of Change
Automate release notes creation by adding a new yarn script

### Why
To save us some time when generating release notes. Before created it as a github workflow but now moving it to a local script.

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

#### 2. Set env variables at root of the repo

```
set GITHUB_TOKEN=<your-personal-access-token>
set RELEASE_TAG=0.79.0
set START_DATE=2025-05-06
set END_DATE=2025-05-30

```
#### 3. Run "`yarn release-notes`" at the root of the repo

#### 4. You will see a release-notes.md file generated at packages\@rnw-scripts\generate-release-notes\release_notes.md which will have all the data you need.