# Git branch setup

This document describes how to set up your development environment and contribute changes to the **Microsoft/react-native-windows** project. This document assumes basic working knowledge with Git and related tools. We are providing instructions specific to this project.

## Creating your own fork

If you wish to contribute changes back to the **Microsoft/react-native-windows** repository, start by creating your own fork of the repository. This is essential. This will keep the number of branches on the main repository to a small count. There are lots of developers in this project and creating lots of branches on the main repository does not scale. In your own fork, you can create as many branches as you like.

- Navigate to **[GitHub](https://www.github.com)** with a browser and log in to your GitHub account. For the sake of this document, let's assume your username is **johndoe**.
- Navigate to the **[Microsoft/react-native-windows](https://github.com/Microsoft/react-native-windows)** repository in the same browser session.
- Click on the **Fork** button at the top right corner of the page.
- Create the fork under your account. Your GitHub profile should now show **react-native-windows** as one of your repositories.
- Create a folder on your device and clone your fork of the **Microsoft** repository. e.g. `https://github.com/johndoe/react-native-windows.git`. Notice how your GitHub username is in the repository location.

```
> git clone https://github.com/johndoe/react-native-windows.git
```

## Setting up the upstream repository

Before starting to contribute changes, please setup your upstream repository to the primary **Microsoft/react-native-windows** repository.

- When you run `git remote -v`, you should see only your fork in the output list

```
> git remote -v

     origin  https://github.com/johndoe/react-native-windows.git (fetch)

     origin  https://github.com/johndoe/react-native-windows.git (push)
```

- Map the primary **react-native-windows** repository as the upstream remote

```
> git remote add upstream https://github.com/Microsoft/react-native-windows.git
```

- Now running `git remote -v` should show the upstream repository also

```
> git remote -v

     origin  https://github.com/johndoe/react-native-windows.git (fetch)
     origin  https://github.com/johndoe/react-native-windows.git (push)
     upstream        https://github.com/Microsoft/react-native-windows.git (fetch)
     upstream        https://github.com/Microsoft/react-native-windows.git (push)
```

- At this point you are ready to start branching and contributing back changes.

## Making code changes and creating a pull request

Create a branch from your fork and start making the code changes.  We recommend using [**SourceTree**](https://www.sourcetreeapp.com/) for working in your repo. Once you are happy with the changes, and want to merge them to the main **Microsoft/react-native-windows** project, create a pull request from your branch directly to "Microsoft/react-native-windows master".

Members on the **Microsoft/react-native-windows** core team will help merge your changes.

## Merging upstream master into your fork master

From time to time, your fork will get out of sync with the upstream remote. Use the following commands to get the master branch of your fork up up to date.

```
> git fetch upstream
> git checkout master
> git pull upstream master
> git push
```

### Merging upstream master into your current branch

Use these commands instead if you would like to update your *current* branch in your fork from the upstream remote.

```
> git fetch upstream
> git pull upstream master
> git push
```