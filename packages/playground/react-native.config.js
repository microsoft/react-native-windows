// Change the below to true for autolink to target playground-composition instead of playground.
// Then run `npx @react-native-community/cli autolink-windows` to actually run the autolink.
const targetComposition = true;

module.exports = {
  dependencies: {
    // Picker is a UWP/Paper component not compatible with the composition playground
    ...(targetComposition && {'@react-native-picker/picker': {platforms: {windows: null}}}),
  },
  project: {
    windows: {
      sourceDir: 'windows',
      solutionFile: targetComposition ? 'playground-composition.sln' : 'playground.sln',
      project: {
        projectFile: targetComposition ? 'playground-composition\\playground-composition.vcxproj' : 'playground\\playground.vcxproj',
      },
    },
  },
};
