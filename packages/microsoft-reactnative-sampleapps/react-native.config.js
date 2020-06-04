// Change the below to true for autolink to target SampleAppCS instead of SampleAppCPP.
// Then run `npx react-native autolink-windows` to actually run the autolink.
const targetCS = false;

module.exports = {
  reactNativePath: '../../vnext',
  project: {
    windows: {
      sourceDir: 'windows',
      solutionFile: 'SampleApps.sln',
      project: {
        projectFile: targetCS ? 'SampleAppCS\\SampleAppCS.csproj' : 'SampleAppCPP\\SampleAppCPP.vcxproj',
      },
    },
  },
};
