We release `react-native-windows` in lock-step with [facebook/react-native](https://github.com/facebook/react-native). I.e., for every release of `react-native`, we create a release of `react-native-windows` with a matching major and minor version.

```
...
react-native@0.38.* -> react-native-windows@0.38.*
react-native@0.39.* -> react-native-windows@0.39.*
...
```

This makes the CLI experience for `react-native-windows` significantly easier to manage, as we can automate the process of installing a compatible version of `react-native-windows` to an existing `react-native` project. It also keeps our compatibility matrix small; we don't need to test every version of `react-native-windows` against versions of `react-native`, only those with matching major and minor versions. The philosophy is that if something is added in a later version that is needed by users of an older version of `react-native-windows`, we can always cherry-pick that change into an older version, or users can upgrade.

With that in mind, here is the process for publishing a new release of `react-native-windows`.

## Publishing a release candidate

### Upgrade NPM dependencies

The `facebook/react-native` project publishes a release candidate from their master branch around the same time they publish a stable version, currently on a monthly cadence.

After a new release candidate is cut, upgrade the [react](https://www.npmjs.com/package/react) and [react-native](https://www.npmjs.com/package/react-native) dependencies. Be sure to check which version of `react` is currently being used by in the [react-native package.json](https://github.com/facebook/react-native/blob/master/package.json), as we keep this peer dependency aligned as well.

```
npm i --save react-native@<major>.<minor>.<build>-rc.<id>
npm i --save react@<version>
```

It may also make sense to look for other NPM dependencies that can be upgraded at this time, especially those that are shared with `react-native`.

#### Update the package.json version

Make sure you also update the `react-native-windows` version in [package.json](https://github.com/ReactWindows/react-native-windows/blob/master/current/package.json) to align with the react-native version you've upgraded to. We use the same `-rc.*` convention for release candidates. **TODO:** We should introduce a script like [bump-oss-version.js](https://github.com/facebook/react-native/blob/master/scripts/bump-oss-version.js).

#### Testing the upgrade

Before moving on to the next step, you'll want to test the package upgrades on the [Playground app](https://github.com/ReactWindows/react-native-windows/tree/master/current/ReactWindows/Playground) in the ReactWindows folder. The Playground app is a very low bar for testing, as it only uses basic `react-native` components. However, it will catch the majority of breaking changes to the `react-native-windows` bridge, which typically include props that have been changes or removed for common components like `View` and `Text`, changes to the batched bridge protocol, and new core components and modules that have been added upstream, but have not been added for `react-native-windows`.

If there is a bug or issue, fix it and create a specific commit for it in the stable branch you're working on. Once the branch is working and you complete the release (as described below), don't forget to rebase and merge back into the master branch to bring that fix back.

### Update the `RNTester` branch

The same example apps from `react-native` are also available for `react-native-windows`, including the [RNTester](https://github.com/facebook/react-native/tree/master/Examples/UIExplorer).

We maintain a fork of the `RNTester` folder from `react-native` as a submodule of `react-native-windows`. The fork uses `git filter-branch` to produce a branch of `react-native` that includes only the content of the Examples folder. We then merge all the changes specific to `react-native-windows` with that filtered branch.

```bash
# Be sure that you have all submodules initialized and up-to-date for react-native-windows.
cd RNTester

# If you don't already have facebook/react-native set up as a Git remote...
git remote add facebook git@github.com:facebook/react-native

# Fetch the latest from facebook
git fetch facebook

# Create a new branch to run the `filter-branch` command only
git checkout -b fbmaster facebook/master

# Filter the react-native master branch for Examples only, this will take some time
# You may have to use `-f` if you've previously run a `filter-branch` command
git filter-branch --prune-empty --subdirectory-filter RNTester fbmaster

# Fetch the latest from react-native-windows
git fetch origin

# Create a new staging branch to perform a merge onto the react-native-windows `examples` branch
git checkout -b staging origin/rntester

# Merge the latest from facebook/react-native RNTester and resolve any merge conflicts
git merge fbmaster

# Fast-forward the `rntester` branch from the `staging` branch
# Before doing this, it's probably a good idea to test that the examples are working by running them
# If anything has broken (it's common), fix it
git checkout rntester
git merge staging

# Use the RNTester to test changes before pushing to react-native-windows

# Push (or PR) your changes to react-native-windows
git push origin rntester

# Cleanup your staging branches
git branch -D fbmaster
git branch -D staging
```

#### Fix bugs

We typically do ad hoc testing of core components using the UIExplorer. After upgrading to the latest release candidate of `react-native` and getting the latest changes to the `react-native` Examples, you should be able to run the UIExplorer on `react-native-windows`. After the app launches, be sure to open each module and component section in the app, and test out all the functionality. More than 50% of the time, nothing has broken since the last release. However, changes to props, new APIs being tested in the UIExplorer, or changes to component base classes are common reasons why the UIExplorer can break.

### Publish and release

After fixing all the bugs with the UIExplorer, you're ready to share your awesome work with the world!

#### Create a release branch

Create a new branch named with the following convention: `<major>.<minor>-stable`. Using version 0.40 as an example:

```
git checkout -b 0.40-stable
git tag v0.40.0-rc.0
git push origin 0.40-stable --tags
```

#### Publish to NPM

Assuming you have authorization to publish a new version of `react-native-windows` to NPM, this is the easy part. From the release branch you just created, run:

```
npm publish
```

#### Pull request to master

We work off the latest release candidate of `react-native` on our master branch of `react-native-windows`, so now is a good time to submit a PR of these changes against the master branch.

#### Release Notes

Coming soon. **TODO:** We need a process around this like [`react-native`](https://github.com/facebook/react-native/blob/master/Releases.md#make-sure-we-have-release-notes).

## Publishing a stable release

Once the corresponding `react-native` version has stable a stable release, usually at the end of the month the release candidate was dropped, we also need to publish a stable release for `react-native-windows`.

Generally, we should just be able to convert the latest release candidate for `react-native-windows` to the stable release by updating the version and dependency information in `package.json`. However, if the release branch is behind on features that were added to the corresponding `react-native` release, then there may be some work to catch up with the latest from `react-native`.

## Patching a release

Whether you are patching a release candidate or patching a stable release, the rule of thumb is to first submit a pull request against the master branch if the fix is still applicable to the latest source. Once that pull request is approved and merged, we will cherry-pick that merged commit onto the versions it is applicable to.
