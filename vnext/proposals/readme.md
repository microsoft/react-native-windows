# Proposals for React Native for Windows

This folder contains archived and in-progress proposals for features that are more substantial than simple bug fixes and would benefit from additional review and documentation specific to `react-native-windows @vnext`. 

[Active Proposal List](https://github.com/microsoft/react-native-windows/pulls?q=is%3Aopen+is%3Apr+label%3AProposal)

## Some notes:
The proposal process here takes its inspiration from the react-native-community [discussions-and-proposals](https://github.com/react-native-community/discussions-and-proposals) as well as the [Windows UI Library](https://github.com/microsoft/microsoft-ui-xaml-specs) spec processes.

- For features that could be submitted to `react-native` and are not specific to _Windows_, please follow the workflow outlined in [discussions-and-proposals](https://github.com/react-native-community/discussions-and-proposals). 
   - We are currently staging feature improvement proposals that will accrue value to `react-native` and are originating from `react-native-windows` in a fork here - [ReactWindows/discussions-and-proposals](https://github.com/ReactWindows/discussions-and-proposals)
- For features that need to be addressed by the native Windows platform, please follow the [Windows UI Library](https://github.com/microsoft/microsoft-ui-xaml-specs) proposal process.

## Proposal process

1. Follow the steps outlined in [Contribution guidelines](https://github.com/microsoft/react-native-windows/blob/master/vnext/docs/CONTRIBUTING.md) to set up your fork and branch to begin contributing.

2. Create a new folder for your proposal under the [active](active/readme.md) folder

3. Author your proposal using the [proposal template](https://github.com/react-native-community/discussions-and-proposals/blob/master/proposals/0000-template.md).  

4. When your proposal is ready for review:
   * Open a new pull request to merge your proposal to the ```master``` branch
   * Update the related issue(s) with a link to the proposal PR
   * Add the label - `Proposal` - to your PR so it shows up in the active proposals list.

5. For react-native-windows team members : Review proposal in checkpoint meetings to close open issues. 
   For community members submitting proposals : The team will pick up an active proposal to do more internal reviews if there are outstanding questions and loop back on the PR.

6. When your proposal is complete and signed off, merge the PR.

7. Once the feature has been implemented and shipped:
   * Add the following header to your proposal:
     > This proposal is archived and the feature was shipped as part of `<insert react-native-windows @vnext version number>`
   * Move your proposal from [active](active/readme.md) to [archive](archive/readme.md)