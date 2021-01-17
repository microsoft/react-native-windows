# React Native Windows Backport and Integration Bots

There are several processes followed in the react-native-windows repo that could benefit from automation. This document describes an overall architecture to implement two bots: a bot to automatically backport pull requests, and another bot to automatically perform daily integrations of react-native nightly builds. This architecture could be extended to future bots that can interact with GitHub actions and perform sophisticated tasks.

# Backport Bot Overview

As the number of branches we support has increased, so has the number of backport PRs. In many cases these PRs are simple cherry-picks that could be automated.

Under a newly automated flow, backporting a change could look like:

1. Label a PR to backport with &quot;Request Backport to 0.xx&quot;
2. A bot would then:
  1. Create a PR cherry-picking the change to the specified branch
  2. Comment on both PRs linking them together
  3. Notify triage approvers to review the backport PR
3. Triage approvers would approve or request changes to the backport PR
4. The bot will update the labels of the original PR to point to &quot;Backported to 0.xx&quot; if merged

![image](https://user-images.githubusercontent.com/835219/104834402-e6a47700-5853-11eb-8966-26d4fad6a8a8.png)

_Figure 1. Automatic Backports_

Engineer intervention would still be required in cases where the change does not apply cleanly, or where silent merge conflicts exist. In the former case, the bot should notify the user through commenting.

# Integration Bot Overview

Our strategy for keeping up with react-native has shifted towards frequent integration of upstream native bits. While effects have been generally positive, more frequent integration has added a downside of additional overhead. Tooling to perform integration with upstream has become increasingly algorithmic, allowing for automation.

To reduce overhead and establish schedule, a bot can automate the creation of PRs integrating upstream changes. PRs should be created only when no other integration PR is active, and should have a maximum safe duration from last integration (e.g. one week). Integration should be ideally triggered as soon as a new package is available, or an integration PR is merged.

# Technical Requirements

A system supporting these scenarios must have a variety of capabilities:

- Perform actions in response to GitHub actions
  - Label added to PR
  - Backport PR is merged or closed
- Perform actions on periodic basis
  - Poll for newer react-native releases to integrate
- Interact with the GitHub API
  - Add/remove labels
  - Add comments
  - Create PRs
- Cherry-pick and push changes
- Run auto-integration script and push results

# Architecture

Technical requirements necessitate an environment with our repo tooling, such as Node and our Git history. A natural choice to automate tasks against that environment is Azure pipelines. Previous sync scripts followed this approach and ran as periodic pipelines. This setup breaks down when needing to perform actions in response to user actions on the GitHub repo however.

Apart from polling for changes, the de-facto way to respond to actions on a GitHub repo is through webhooks. In this scheme, GitHub will call a user-defined REST endpoint to notify it of an action. This requires a service which can consume these notifications. A common low-maintenance option to consume webhooks is a serverless solution such as Azure Functions. Azure functions provides too restrictive of an environment to perform some of the above tasks.

While neither option can fill all our needs, we can combine straightforward usage of Azure Functions and Azure Pipelines to meet our needs. GitHub webhooks can notify an Azure Functions &quot;coordinator&quot; which delegates to Azure Pipelines &quot;workers&quot;. These workers can execute a task in a sophisticated and controlled environment, then call the coordinator with results. The coordinator triggers a worker by using the Azure Pipelines REST API to start a pipeline with defined, job specific variables.

![image](https://user-images.githubusercontent.com/835219/104834460-46028700-5854-11eb-8b30-1a081eff2be4.png)

_Figure 2. Example flow for backport bot_

Apart from allowing the usage of a configurable environment with low maintenance, this separation offers some desirable properties:

- CLI interfacing work is shown in pipeline logs for easier troubleshooting
- Functions are very short-lived (minimal spend)
- Functions can filter irrelevant notifications (e.g. non-backport labels) without spinning up a pipeline
- Functions can quickly interact with GitHub entities (e.g. adding comments) without the latency of spinning up a pipeline

# Security

This system requires storage of several secrets for access to the GitHub repo and Azure pipelines permissions. PATs will be generated for the RNBot service account and stored in the Azure keyvault for the function application, and Pipeline secrets for the pipelines needing access. Credentials for the Azure Pipelines API will be stored in the KeyVault as well (user tbd). Using secrets in the pipeline does not share the concerns of secret usage in PRs, as changes to worker pipelines would have to be signed off on before first execution.

Because endpoints are public, malicious actors could attempt to trigger or spam bot actions. GitHub allows [protecting](https://docs.github.com/en/github-ae@latest/developers/webhooks-and-events/securing-your-webhooks) against this scenario by using a pre-shared secret between GitHub and the service. All endpoints should check against this secret, to be present in calls from both GitHub and Pipelines workers.
