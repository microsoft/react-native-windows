# Milestone 0.81 Open Issues

This document provides information about fetching open issues for milestone 0.81.

## Quick Summary

As of November 18, 2025, there are **46 open issues** in milestone 0.81 for the microsoft/react-native-windows repository.

A complete list with details is available in [MILESTONE_081_ISSUES_LIST.txt](MILESTONE_081_ISSUES_LIST.txt).

## Fetching Issues

### Using the Provided Script

A Node.js script is included in the repository to fetch and display open issues for milestone 0.81:

```bash
node get-milestone-081-issues.js [format]
```

**Available Formats:**
- `detailed` (default) - Shows full details of all issues with labels summary
- `summary` - Compact list with issue numbers and titles
- `csv` - CSV format for importing into spreadsheets
- `json` - JSON format for programmatic use

**Examples:**

```bash
# Detailed view (default)
node get-milestone-081-issues.js

# Summary view
node get-milestone-081-issues.js summary

# Export to CSV
node get-milestone-081-issues.js csv > milestone-081-issues.csv

# Export to JSON
node get-milestone-081-issues.js json > milestone-081-issues.json
```

### Using GitHub API Directly

You can also use the GitHub API directly to fetch issues:

```bash
curl -H "Accept: application/vnd.github.v3+json" \
  "https://api.github.com/search/issues?q=repo:microsoft/react-native-windows+milestone:0.81+is:open&per_page=100"
```

### Using GitHub CLI

If you have the GitHub CLI installed:

```bash
gh issue list --repo microsoft/react-native-windows --milestone "0.81" --state open
```

### Using GitHub Web Interface

Visit the milestone page directly:
https://github.com/microsoft/react-native-windows/milestone/41

## Common Issue Labels in Milestone 0.81

Based on the current open issues, common labels include:

- **bug** - Issues tracking bugs
- **enhancement** - Feature requests and improvements
- **Area: Fabric** - Issues related to Fabric architecture
- **New Architecture** - Issues related to the new RN architecture
- **Integration Follow-up** - Follow-up items from RN integration
- **Fix Hermes Debugger for RNW** - Debugger-related issues
- **Area: CLI** - Command-line interface issues
- **Area: TextInput** - TextInput component issues
- **Area: Release Process** - Release-related items

## Notable Issues

Some key areas of focus in milestone 0.81:

1. **Modern Inspector Support** - Integration and improvement of the modern inspector
2. **XAML Support in Fabric** - Adding support for XAML controls in Fabric applications
3. **Props Parity** - Ensuring feature parity between Paper and Fabric architectures
4. **Community Modules** - Upgrading and supporting 3rd party modules
5. **Debugging Improvements** - Various debugger enhancements
6. **Release Process** - 0.81 release tracking and validation

## Filtering and Sorting

When viewing issues on GitHub, you can use filters:

- **By assignee**: `assignee:username`
- **By label**: `label:"label-name"`
- **By author**: `author:username`
- **Exclude labels**: `-label:"label-name"`

Example: To see all bug issues assigned to someone:
```
is:open milestone:0.81 label:bug assignee:username
```

## Contributing

To help with milestone 0.81:

1. Check the [open issues](https://github.com/microsoft/react-native-windows/issues?q=is%3Aopen+milestone%3A0.81)
2. Look for issues labeled `good first issue` or issues without assignees
3. Comment on the issue to express interest
4. Follow the [contributing guidelines](CONTRIBUTING.md)

## API Rate Limits

Note: The GitHub API has rate limits:
- Unauthenticated requests: 60 requests per hour
- Authenticated requests: 5,000 requests per hour

For authenticated requests, set a GitHub token:

```bash
# In the script, you could modify it to accept a token
export GITHUB_TOKEN="your_token_here"
```

## Last Updated

This document was generated automatically. For the most current information, run the provided script or check the GitHub milestone page directly.
