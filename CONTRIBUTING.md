# Contributing to React Native for Windows

👍🎉 First off, thanks for taking the time to contribute! 🎉👍

Below is our guidance for how to report issues, propose new features, and submit contributions via Pull Requests (PRs).

## 🪟 Open Development Workflow

The React Native Windows team is VERY active in this GitHub Repo. In fact, we live in it all day long and carry out all our development in the open!

When the team finds issues we file them in the repo. When we propose new ideas or think-up new features, we file new feature requests. When we work on fixes or features, we create branches and work on those improvements. And when PRs are reviewed, we review in public - including all the good, the bad, and the ugly parts.

The point of doing all this work in public is to ensure that we are holding ourselves to a high degree of transparency, and so that the community sees that we apply the same processes and hold ourselves to the same quality-bar as we do to community-submitted issues and PRs. 

> To learn more about our approach to issue Triage, see ["Triage Process"](https://github.com/microsoft/react-native-windows/wiki/Triage-Process) in our Wiki. 

### 🤖 Repo Bot

The team triages new issues several times a week. During triage, the team uses labels to categorize, manage, and drive the project workflow.

We employ a bot engine to help us automate common processes within our workflow.

In the case that we need some additional information or changes to approve the PR, the triage team will mark the PR or Issue with label such as "Needs: Author Feedback". Keep an eye out for your GitHub notifications as the bot might automatically close your issue if it fails to recognize a response after some time. Should this happen to you, feel free to reactivate and provide the requested information.

---
## Reporting Security Issues

**Please do not report security vulnerabilities through public GitHub issues.** Instead, please report them to the Microsoft Security Response Center (MSRC). See [SECURITY.md](.github/security.md) for more information.

## Before you start, file an issue

Please follow this simple rule to help us eliminate any unnecessary wasted effort & frustration, and ensure an efficient and effective use of everyone's time - yours, ours, and other community members':

> 👉 If you have a question, think you've discovered an issue, would like to propose a new feature, etc., then find/file an issue **BEFORE** starting work to fix/implement it.

### Search existing issues first

Before filing a new issue, search existing open and closed issues first: This project is moving fast! It is likely someone else has found the problem you're seeing, and someone may be working on or have already contributed a fix!

If no existing item describes your issue/feature, great - please file a new issue:

### File a new Issue

* Don't know whether you're reporting an issue or requesting a feature? File an issue
* Have a question that you don't see answered in docs, videos, etc.? File an issue
* Want to know if we're planning on building a particular feature? File an issue
* Got a great idea for a new feature? File an issue/request/idea
* Don't understand how to do something? File an issue
* Found an existing issue that describes yours? Great - upvote and add additional commentary / info / repro-steps / etc.

When you hit "New Issue", select the type of issue closest to what you want to report/ask/request to be prompted with the appropriate issue template.

### Complete the template

**Complete the information requested in the issue template, providing as much information as possible**. The more information you provide, the more likely your issue/ask will be understood and implemented. 

* **We LOVE detailed repro steps!** What steps do we need to take to reproduce the issue? Assume we love to read repro steps. As much detail as you can stand is probably _barely_ enough detail for us!
* Prefer error message text where possible or screenshots of errors if text cannot be captured
* We MUCH prefer text command-line script than screenshots of command-line script.
* **If you intend to implement the fix/feature yourself then say so!** If you do not indicate otherwise we will assume that the issue is our to solve, or may label the issue as `Help-Wanted`.

---

## Contributing fixes / features

If you're able & willing to help fix issues and/or implement features, we'd love your contribution!

The best place to start is the list of ["good first issue"](https://github.com/microsoft/react-native-windows/issues?q=is%3Aopen+is%3Aissue+label%3A%22good+first+issue%22) issues. These are bugs or tasks that we on the team believe could be implemented for someone without any prior experience in the codebase. Once you're feeling more comfortable in the codebase, feel free to just use the ["Help Wanted"](https://github.com/microsoft/react-native-windows/issues?q=is%3Aopen+is%3Aissue+label%3A%22help+wanted%22+) label, or just find an issue your interested in and hop in!

Often, we like to assign issues that generally belong to somebody's area of expertise to the team member that owns that area. This doesn't mean the community can't jump in -- they should reach out and have a chat with the assignee to see if it'd okay to take. If an issue's been assigned more than a month ago, there's a good chance it's fair game to try yourself.

For a primer on our codebase, see ["Repository Contents"](https://github.com/microsoft/react-native-windows/wiki/Repository-Contents) and ["Project Structure."](https://github.com/microsoft/react-native-windows/blob/main/docs/project-structure.md)

---

## 👨‍💻 Development

> **System Requirements:** Contributing to this project requires a Windows machine, see [rnw-dependencies](https://microsoft.github.io/react-native-windows/docs/next/rnw-dependencies) for more info on this and the script for installing required dependencies.

### 🚧 Fork, Clone, Branch and Create your PR

As is standard for GitHub and open source projects, RNW uses the ["Fork and pull"](https://docs.github.com/pull-requests/collaborating-with-pull-requests/getting-started/about-collaborative-development-models#fork-and-pull-model) model for accepting and integrating code changes.

Here's what the contribution process looks like:

1. Fork the repo if you haven't already.
1. Clone your fork locally.
1. Create & push a feature branch.
1. Make the desired changes.
1. Test your changes.
1. Ensure your code passes linting checks:
    1. For C++ changes, you can run `yarn format` at the root of the repo to automatically reformat your code.
    1. For JavaScript/TypeScript, you can run `yarn lint:fix` at the root of the repo.
1. After you've created your commits, make sure to run `yarn change` at the root of the repo to create "change files":
    1. When prompted for `Change type`, the first option (usually `Prerelease` or `Patch`) is correct 99% of the time (don't select `None`).
    1. When prompted to `Describe changes`, please provide a descriptive message (defaults to your commit message).
    1. The command will automatically create a new commit with the change files, make sure they're included in your PR.
    1. **Note:** You may be prompted multiple times or not at all (depending on *which* packages you modified).
1. Open a Pull Request (PR) and address any feedback.

> 👉 If this is your first time working with forks, you may find [GitHub's reference on forks](https://docs.github.com/pull-requests/collaborating-with-pull-requests/working-with-forks/about-forks) helpful. If this is your first time working with git or GitHub altogether, see the [GitHub Quickstart guide](https://docs.github.com/get-started/quickstart) for a walkthrough of setting up git, contributing to projects, and explanations of commonly used terms. 

> ⚛️ For RNW-specific walkthrough on setting up your branch, see the [Setup page](https://github.com/microsoft/react-native-windows/wiki/Setup) in our Wiki. 

### 🔙 **Backporting**
There are cases when we'll want to port a fix back into one or more of the previous stable releases. If you are working on backporting a fix, your development process will look similar to what's described above, main difference being that your feature branch will need to build off of one of the n-stable branches as the base, instead of main. For example, if you were backporting a change to 0.67-stable you would run the following commands to get your branch set up:

```
> git fetch upstream
> git checkout 0.67-stable
> git pull upstream 0.67-stable
> git checkout -b backport_branch_name
```

Then once you've added and tested the desired changes, you'll have to make sure to specify the target stable branch, instead of main, as the base branch when creating your PR.

> Note that in order to preserve stability and compatibility we don't backport all fixes, and all backport changes beyond a certain release (currently 0.66-stable) will go through additional triage to get approved.

For more information, see our [Wiki page on Backporting](https://github.com/microsoft/react-native-windows/wiki/Backporting-a-Change). 
### 🧪 Testing

Build and see if your solution works. Consult [Building React-Native-Windows](https://github.com/microsoft/react-native-windows/blob/main/docs/building-rnw.md) if you have problems.

Testing is a key component in the development workflow. If your changes affect existing test cases, or you're working on brand new features and also the accompanying test cases, see [End-to-End Testing](https://github.com/microsoft/react-native-windows/blob/main/docs/e2e-testing.md) for more information about how to validate your work locally.

### 🛡️ Security & Compliance

React Native Windows follows Microsoft's Secure Development Lifecycle (SDL) requirements. As part of this commitment:

- **Attack Surface Analyzer (ASA)** runs automatically on every PR to detect security regressions during the build process. This ensures that builds don't inadvertently weaken the OS security configuration.
- If ASA detects changes in your PR, review the artifacts in the PR build to understand the security implications.
- For more information about ASA, see [Attack Surface Analyzer documentation](https://github.com/microsoft/react-native-windows/blob/main/docs/attack-surface-analyzer.md).

Most changes won't trigger ASA findings, but if yours does, the documentation explains how to review and address the findings.

### ✅ Code Review

When you'd like the team to review your PR, (even if the work is not yet fully-complete), open a PR so that the team can review your work and provide comments, suggestions, and request changes. It may take several cycles, but the end result will be solid, testable, conformant code that is safe for us to merge.

> Most contributions require you to agree to a Contributor License Agreement (CLA) declaring that you have the right to, and actually do, grant us the rights to use your contribution. For details, visit https://cla.microsoft.com. When you submit a pull request, a CLA-bot will automatically determine whether you need to provide a CLA and decorate the PR appropriately (e.g., label, comment). Simply follow the instructions provided by the bot. You will only need to do this once across all repos using our CLA.

### 🥳 Merge

Once your code has been reviewed and approved by the team, it will be merged into the main branch. Once merged, your PR will be automatically closed. 

---

## 🦸 Thank you

Thank you in advance for your contribution! Now, [what's next on the list](https://github.com/microsoft/react-native-windows/issues?q=is%3Aopen+is%3Aissue+label%3A%22good+first+issue%22)? 😜
