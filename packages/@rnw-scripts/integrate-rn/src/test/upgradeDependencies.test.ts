/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as _ from 'lodash';
import {
  calcPackageDependencies,
  LocalPackageDeps,
  PackageDeps,
  PackageDiff,
} from '../upgradeDependencies';

const olderReactNative: PackageDeps = {
  packageName: 'react-native',
  peerDependencies: {
    react: '16.11.0',
  },
  dependencies: {
    '@babel/runtime': '^7.0.0',
    '@react-native-community/cli': '^4.5.1',
    '@react-native-community/cli-platform-android': '^4.5.1',
    '@react-native-community/cli-platform-ios': '^4.5.0',
    'abort-controller': '^3.0.0',
  },
  devDependencies: {
    'flow-bin': '^0.113.0',
    'flow-remove-types': '1.2.3',
    jest: '^24.8.0',
    'jest-junit': '^6.3.0',
    jscodeshift: '^0.6.2',
    mkdirp: '^0.5.1',
    prettier: '1.17.0',
    react: '16.11.0',
    'react-test-renderer': '16.11.0',
    shelljs: '^0.7.8',
    signedsource: '^1.0.0',
    ws: '^6.1.4',
    yargs: '^14.2.0',
  },
};

const newerReactNative: PackageDeps = {
  packageName: 'react-native',
  peerDependencies: {
    react: '16.13.1',
  },
  dependencies: {
    '@babel/runtime': '^7.0.0',
    '@react-native-community/cli': '^4.10.0',
    '@react-native-community/cli-platform-android': '^4.10.0',
    '@react-native-community/cli-platform-ios': '^4.10.0',
    'abort-controller': '^3.0.0',
  },
  devDependencies: {
    'flow-bin': '^0.122.0',
    'flow-remove-types': '1.2.3',
    jest: '^24.9.0',
    'jest-junit': '^6.3.0',
    jscodeshift: '^0.7.0',
    mkdirp: '^0.5.1',
    prettier: '1.17.0',
    react: '16.13.1',
    'react-test-renderer': '16.13.1',
    shelljs: '^0.7.8',
    signedsource: '^1.0.0',
    ws: '^6.1.4',
    yargs: '^14.2.0',
  },
};

const rnDiff: PackageDiff = {
  oldPackage: olderReactNative,
  newPackage: newerReactNative,
};

const olderRepoConfig: PackageDeps = {
  packageName: '@react-native/repo-config',
  dependencies: {
    'flow-bin': '^0.131.0',
    jest: '^26.0.1',
    'jest-junit': '^10.0.0',
    jscodeshift: '^0.9.0',
    mkdirp: '^0.5.1',
    prettier: '1.19.1',
    react: '16.13.1',
    'react-shallow-renderer': '16.13.1',
    'react-test-renderer': '16.13.1',
    shelljs: '^0.7.8',
    signedsource: '^1.0.0',
    ws: '^6.1.4',
    yargs: '^15.3.1',
  },
};

const newerRepoConfig: PackageDeps = {
  packageName: '@react-native/repo-config',
  dependencies: {
    'flow-bin': '^0.136.0',
    jest: '^26.5.2',
    'jest-junit': '^10.0.0',
    jscodeshift: '^0.11.0',
    mkdirp: '^0.5.1',
    prettier: '1.19.1',
    react: '16.13.1',
    'react-shallow-renderer': '16.13.2',
    'react-test-renderer': '16.13.1',
    shelljs: '^0.7.8',
    signedsource: '^1.0.0',
    ws: '^6.1.4',
    yargs: '^15.3.1',
  },
};

const repoConfigDiff: PackageDiff = {
  oldPackage: olderRepoConfig,
  newPackage: newerRepoConfig,
};

test('No Local Packages', () => {
  const deps = calcPackageDependencies('0.63.0', rnDiff, repoConfigDiff, []);
  expect(deps.length).toEqual(0);
});

test('Out-of-tree platform (Same Packages)', () => {
  const outOfTreeDeps: LocalPackageDeps = {
    ...olderReactNative,
    packageName: 'react-native-windows',
    outOfTreePlatform: true,
  };

  const deps = calcPackageDependencies('0.63.0', rnDiff, repoConfigDiff, [
    outOfTreeDeps,
  ]);

  expect(deps.length).toEqual(1);
  expect(deps[0]).toEqual({
    ...outOfTreeDeps,
    dependencies: newerReactNative.dependencies,
    peerDependencies: newerReactNative.peerDependencies,
    devDependencies: newerReactNative.devDependencies,
  });

  expectSortedDeps(deps);
});

test('Out-of-tree platform (Removed dependencies)', () => {
  const rnRemovedDiff = _.cloneDeep(rnDiff);
  delete rnRemovedDiff.newPackage.dependencies!['abort-controller'];

  const outOfTreeDeps: LocalPackageDeps = {
    ...olderReactNative,
    packageName: 'react-native-windows',
    outOfTreePlatform: true,
  };

  const deps = calcPackageDependencies(
    '0.63.0',
    rnRemovedDiff,
    repoConfigDiff,
    [outOfTreeDeps],
  );

  expect(deps.length).toEqual(1);
  expect(deps[0]).toEqual({
    ...outOfTreeDeps,
    dependencies: _.omit(newerReactNative.dependencies, 'abort-controller'),
    peerDependencies: newerReactNative.peerDependencies,
    devDependencies: newerReactNative.devDependencies,
  });

  expectSortedDeps(deps);
});

test('Out-of-tree platform (Removed all peerDependencies)', () => {
  const rnRemovedDiff = _.cloneDeep(rnDiff);
  delete rnRemovedDiff.newPackage.peerDependencies!.react;

  const outOfTreeDeps: LocalPackageDeps = {
    ...olderReactNative,
    packageName: 'react-native-windows',
    outOfTreePlatform: true,
  };

  const deps = calcPackageDependencies(
    '0.63.0',
    rnRemovedDiff,
    repoConfigDiff,
    [outOfTreeDeps],
  );

  expect(deps.length).toEqual(1);
  expect(deps[0]).toEqual({
    ..._.omit(outOfTreeDeps, 'peerDependencies'),
    dependencies: newerReactNative.dependencies,
    devDependencies: newerReactNative.devDependencies,
  });

  expectSortedDeps(deps);
});

test('Out-of-tree platform (No dependencies group)', () => {
  const outOfTreeDeps: LocalPackageDeps = {
    ..._.omit(olderReactNative, 'dependencies'),
    packageName: 'react-native-windows',
    outOfTreePlatform: true,
  };

  const deps = calcPackageDependencies('0.63.0', rnDiff, repoConfigDiff, [
    outOfTreeDeps,
  ]);

  expect(deps.length).toEqual(1);
  expect(deps[0]).toEqual({
    ...outOfTreeDeps,
    dependencies: newerReactNative.dependencies,
    peerDependencies: newerReactNative.peerDependencies,
    devDependencies: newerReactNative.devDependencies,
  });

  expectSortedDeps(deps);
});

test('Out-of-tree platform (No peerDependencies group)', () => {
  const outOfTreeDeps: LocalPackageDeps = {
    ..._.omit(olderReactNative, 'peerDependencies'),
    packageName: 'react-native-windows',
    outOfTreePlatform: true,
  };

  const deps = calcPackageDependencies('0.63.0', rnDiff, repoConfigDiff, [
    outOfTreeDeps,
  ]);

  expect(deps.length).toEqual(1);
  expect(deps[0]).toEqual({
    ...outOfTreeDeps,
    dependencies: newerReactNative.dependencies,
    peerDependencies: newerReactNative.peerDependencies,
    devDependencies: newerReactNative.devDependencies,
  });

  expectSortedDeps(deps);
});

test('Out-of-tree platform (No devDependencies group)', () => {
  const outOfTreeDeps: LocalPackageDeps = {
    ..._.omit(olderReactNative, 'devDependencies'),
    packageName: 'react-native-windows',
    outOfTreePlatform: true,
  };

  const deps = calcPackageDependencies('0.63.0', rnDiff, repoConfigDiff, [
    outOfTreeDeps,
  ]);

  expect(deps.length).toEqual(1);
  expect(deps[0]).toEqual({
    ..._.omit(outOfTreeDeps, 'devDependencies'),
    dependencies: newerReactNative.dependencies,
    peerDependencies: newerReactNative.peerDependencies,
  });

  expectSortedDeps(deps);
});

test('Out-of-tree platform (Additional dependencies)', () => {
  const outOfTreeDeps: LocalPackageDeps = {
    ...olderReactNative,
    dependencies: {
      ...olderReactNative.dependencies,
      'extra-dependency': '0.0.1',
    },
    packageName: 'react-native-windows',
    outOfTreePlatform: true,
  };

  const deps = calcPackageDependencies('0.63.0', rnDiff, repoConfigDiff, [
    outOfTreeDeps,
  ]);

  expect(deps.length).toEqual(1);
  expect(deps[0]).toEqual({
    ...outOfTreeDeps,
    dependencies: {
      ...newerReactNative.dependencies,
      'extra-dependency': '0.0.1',
    },
    peerDependencies: newerReactNative.peerDependencies,
    devDependencies: newerReactNative.devDependencies,
  });

  expectSortedDeps(deps);
});

test('Out-of-tree platform (Additional peerDependencies)', () => {
  const outOfTreeDeps: LocalPackageDeps = {
    ...olderReactNative,
    peerDependencies: {
      ...olderReactNative.peerDependencies,
      'extra-dependency': '0.0.1',
    },
    packageName: 'react-native-windows',
    outOfTreePlatform: true,
  };

  const deps = calcPackageDependencies('0.63.0', rnDiff, repoConfigDiff, [
    outOfTreeDeps,
  ]);

  expect(deps.length).toEqual(1);
  expect(deps[0]).toEqual({
    ...outOfTreeDeps,
    dependencies: newerReactNative.dependencies,
    peerDependencies: {
      ...newerReactNative.peerDependencies,
      'extra-dependency': '0.0.1',
    },
    devDependencies: newerReactNative.devDependencies,
  });

  expectSortedDeps(deps);
});

test('Out-of-tree platform (Missing dependencies)', () => {
  const outOfTreeDeps: LocalPackageDeps = {
    ...olderReactNative,
    dependencies: {
      '@babel/runtime': '^7.0.0',
      '@react-native-community/cli': '^4.5.1',
      'extra-dependency': '0.0.1',
    },
    packageName: 'react-native-windows',
    outOfTreePlatform: true,
  };

  const deps = calcPackageDependencies('0.63.0', rnDiff, repoConfigDiff, [
    outOfTreeDeps,
  ]);

  expect(deps.length).toEqual(1);
  expect(deps[0]).toEqual({
    ...outOfTreeDeps,
    dependencies: {
      ...newerReactNative.dependencies,
      'extra-dependency': '0.0.1',
    },
    peerDependencies: newerReactNative.peerDependencies,
    devDependencies: newerReactNative.devDependencies,
  });

  expectSortedDeps(deps);
});

test('Out-of-tree platform (Missing peerDependencies)', () => {
  const outOfTreeDeps: LocalPackageDeps = {
    ...olderReactNative,
    peerDependencies: {
      'extra-dependency': '0.0.1',
    },
    packageName: 'react-native-windows',
    outOfTreePlatform: true,
  };

  const deps = calcPackageDependencies('0.63.0', rnDiff, repoConfigDiff, [
    outOfTreeDeps,
  ]);

  expect(deps.length).toEqual(1);
  expect(deps[0]).toEqual({
    ...outOfTreeDeps,
    dependencies: newerReactNative.dependencies,
    peerDependencies: {
      ...newerReactNative.peerDependencies,
      'extra-dependency': '0.0.1',
    },
    devDependencies: newerReactNative.devDependencies,
  });

  expectSortedDeps(deps);
});

test('Out-of-tree platform (No shared devDependencies)', () => {
  const outOfTreeDeps: LocalPackageDeps = {
    ...olderReactNative,
    devDependencies: {
      'extra-dependency': '0.0.1',
    },
    packageName: 'react-native-windows',
    outOfTreePlatform: true,
  };

  const deps = calcPackageDependencies('0.63.0', rnDiff, repoConfigDiff, [
    outOfTreeDeps,
  ]);

  expect(deps.length).toEqual(1);
  expect(deps[0]).toEqual({
    ...outOfTreeDeps,
    dependencies: newerReactNative.dependencies,
    peerDependencies: newerReactNative.peerDependencies,
    devDependencies: {
      'extra-dependency': '0.0.1',
    },
  });

  expectSortedDeps(deps);
});

test('Out-of-tree platform (Newer devDependencies)', () => {
  const outOfTreeDeps: LocalPackageDeps = {
    ...olderReactNative,
    devDependencies: {
      'flow-bin': '^0.123.0',
      'flow-remove-types': '1.2.4',
    },
    packageName: 'react-native-windows',
    outOfTreePlatform: true,
  };

  const deps = calcPackageDependencies('0.63.0', rnDiff, repoConfigDiff, [
    outOfTreeDeps,
  ]);

  expect(deps.length).toEqual(1);
  expect(deps[0]).toEqual({
    ...outOfTreeDeps,
    dependencies: newerReactNative.dependencies,
    peerDependencies: newerReactNative.peerDependencies,
    devDependencies: {
      'flow-bin': '^0.123.0',
      'flow-remove-types': '1.2.4',
    },
  });

  expectSortedDeps(deps);
});

test('Out-of-tree platform (repo-config devDependencies)', () => {
  const outOfTreeDeps: LocalPackageDeps = {
    ...olderReactNative,
    devDependencies: {
      'react-shallow-renderer': '16.13.1',
    },
    packageName: 'react-native-windows',
    outOfTreePlatform: true,
  };

  const deps = calcPackageDependencies('0.63.0', rnDiff, repoConfigDiff, [
    outOfTreeDeps,
  ]);

  expect(deps.length).toEqual(1);
  expect(deps[0]).toEqual({
    ...outOfTreeDeps,
    dependencies: newerReactNative.dependencies,
    peerDependencies: newerReactNative.peerDependencies,
    devDependencies: {
      'react-shallow-renderer': '16.13.2',
    },
  });

  expectSortedDeps(deps);
});

test('Out-of-tree platform (Multiple)', () => {
  const outOfTreeDeps1: LocalPackageDeps = {
    ...olderReactNative,
    dependencies: {
      ...olderReactNative.dependencies,
      'extra-dependency': '0.0.1',
    },
    packageName: 'react-native-windows',
    outOfTreePlatform: true,
  };

  const outOfTreeDeps2: LocalPackageDeps = {
    ...olderReactNative,
    dependencies: {
      ...olderReactNative.dependencies,
      'extra-dependency': '0.0.5',
    },
    packageName: '@office-iss/react-native-win32',
    outOfTreePlatform: true,
  };

  const deps = calcPackageDependencies('0.63.0', rnDiff, repoConfigDiff, [
    outOfTreeDeps1,
    outOfTreeDeps2,
  ]);

  expect(deps.length).toEqual(2);
  expect(deps[0]).toEqual({
    ...outOfTreeDeps1,
    dependencies: {
      ...newerReactNative.dependencies,
      'extra-dependency': '0.0.1',
    },
    peerDependencies: newerReactNative.peerDependencies,
    devDependencies: newerReactNative.devDependencies,
  });
  expect(deps[1]).toEqual({
    ...outOfTreeDeps2,
    dependencies: {
      ...newerReactNative.dependencies,
      'extra-dependency': '0.0.5',
    },
    peerDependencies: newerReactNative.peerDependencies,
    devDependencies: newerReactNative.devDependencies,
  });

  expectSortedDeps(deps);
});

test('Out-of-tree platform (RN peerDependency satisfied)', () => {
  const outOfTreeDeps: LocalPackageDeps = {
    ...olderReactNative,
    peerDependencies: {
      ...olderReactNative.peerDependencies,
      'react-native': '^0.63.0',
    },
    packageName: 'react-native-windows',
    outOfTreePlatform: true,
  };

  const deps = calcPackageDependencies('0.63.5', rnDiff, repoConfigDiff, [
    outOfTreeDeps,
  ]);

  expect(deps.length).toEqual(1);
  expect(deps[0]).toEqual({
    ...outOfTreeDeps,
    dependencies: newerReactNative.dependencies,
    peerDependencies: {
      ...newerReactNative.peerDependencies,
      'react-native': '^0.63.0',
    },
    devDependencies: newerReactNative.devDependencies,
  });

  expectSortedDeps(deps);
});

test('Out-of-tree platform (RN peerDependency version unsatisfied)', () => {
  const outOfTreeDeps: LocalPackageDeps = {
    ...olderReactNative,
    peerDependencies: {
      ...olderReactNative.peerDependencies,
      'react-native': '0.63.1',
    },
    packageName: 'react-native-windows',
    outOfTreePlatform: true,
  };

  const deps = calcPackageDependencies('0.63.5', rnDiff, repoConfigDiff, [
    outOfTreeDeps,
  ]);

  expect(deps.length).toEqual(1);
  expect(deps[0]).toEqual({
    ...outOfTreeDeps,
    dependencies: newerReactNative.dependencies,
    peerDependencies: {
      ...newerReactNative.peerDependencies,
      'react-native': '0.63.5',
    },
    devDependencies: newerReactNative.devDependencies,
  });

  expectSortedDeps(deps);
});

test('Out-of-tree platform (RN peerDependency range unsatisfied)', () => {
  const outOfTreeDeps: LocalPackageDeps = {
    ...olderReactNative,
    peerDependencies: {
      ...olderReactNative.peerDependencies,
      'react-native': '^0.62.0',
    },
    packageName: 'react-native-windows',
    outOfTreePlatform: true,
  };

  const deps = calcPackageDependencies('0.63.0', rnDiff, repoConfigDiff, [
    outOfTreeDeps,
  ]);

  expect(deps.length).toEqual(1);
  expect(deps[0]).toEqual({
    ...outOfTreeDeps,
    dependencies: newerReactNative.dependencies,
    peerDependencies: {
      ...newerReactNative.peerDependencies,
      'react-native': '^0.63.0',
    },
    devDependencies: newerReactNative.devDependencies,
  });

  expectSortedDeps(deps);
});

test('RN consumer (No updates)', () => {
  const consumerDeps: LocalPackageDeps = {
    packageName: 'e2e-test',
    dependencies: {
      'extra-dep': '0.0.1',
      'react-native': '^0.63.0',
      react: '^16.13.1',
    },
    outOfTreePlatform: false,
  };

  const deps = calcPackageDependencies('0.63.0', rnDiff, repoConfigDiff, [
    consumerDeps,
  ]);

  expect(deps.length).toEqual(1);
  expect(deps[0]).toEqual(consumerDeps);

  expectSortedDeps(deps);
});

test('RN consumer (Bump RN range)', () => {
  const consumerDeps: LocalPackageDeps = {
    packageName: 'e2e-test',
    dependencies: {
      'extra-dep': '0.0.1',
      'react-native': '^0.62.0',
      react: '^16.13.1',
    },
    outOfTreePlatform: false,
  };

  const deps = calcPackageDependencies('0.63.0', rnDiff, repoConfigDiff, [
    consumerDeps,
  ]);

  expect(deps.length).toEqual(1);
  expect(deps[0]).toEqual({
    ...consumerDeps,
    dependencies: {
      'extra-dep': '0.0.1',
      'react-native': '^0.63.0',
      react: '^16.13.1',
    },
  });

  expectSortedDeps(deps);
});

test('RN consumer (Bump RN version)', () => {
  const consumerDeps: LocalPackageDeps = {
    packageName: 'e2e-test',
    dependencies: {
      'extra-dep': '0.0.1',
      'react-native': '0.62.0',
      react: '^16.13.1',
    },
    outOfTreePlatform: false,
  };

  const deps = calcPackageDependencies('0.63.0', rnDiff, repoConfigDiff, [
    consumerDeps,
  ]);

  expect(deps.length).toEqual(1);
  expect(deps[0]).toEqual({
    ...consumerDeps,
    dependencies: {
      'extra-dep': '0.0.1',
      'react-native': '0.63.0',
      react: '^16.13.1',
    },
  });

  expectSortedDeps(deps);
});

test('RN consumer (Fix missing/unsatisfied peerDependency)', () => {
  const consumerDeps: LocalPackageDeps = {
    packageName: 'e2e-test',
    dependencies: {
      'extra-dep': '0.0.1',
      'react-native': '^0.63.0',
    },
    outOfTreePlatform: false,
  };

  const deps = calcPackageDependencies('0.63.0', rnDiff, repoConfigDiff, [
    consumerDeps,
  ]);

  expect(deps.length).toEqual(1);
  expect(deps[0]).toEqual({
    ...consumerDeps,
    dependencies: {
      'extra-dep': '0.0.1',
      'react-native': '^0.63.0',
      react: '16.13.1',
    },
  });
});

test('RN consumer (Fix old/unsatisfied peerDependency)', () => {
  const consumerDeps: LocalPackageDeps = {
    packageName: 'e2e-test',
    dependencies: {
      'extra-dep': '0.0.1',
      'react-native': '^0.63.0',
      react: '16.13.0',
    },
    outOfTreePlatform: false,
  };

  const deps = calcPackageDependencies('0.63.0', rnDiff, repoConfigDiff, [
    consumerDeps,
  ]);

  expect(deps.length).toEqual(1);
  expect(deps[0]).toEqual({
    ...consumerDeps,
    dependencies: {
      'extra-dep': '0.0.1',
      'react-native': '^0.63.0',
      react: '16.13.1',
    },
  });

  expectSortedDeps(deps);
});

test('RN consumer (Newer repo-config devDependency)', () => {
  const consumerDeps: LocalPackageDeps = {
    packageName: 'e2e-test',
    dependencies: {
      'extra-dep': '0.0.1',
      'react-native': '^0.63.0',
      react: '^16.13.1',
    },
    devDependencies: {
      'flow-bin': '^0.113.0',
    },
    outOfTreePlatform: false,
  };

  const deps = calcPackageDependencies('0.63.0', rnDiff, repoConfigDiff, [
    consumerDeps,
  ]);

  expect(deps.length).toEqual(1);
  expect(deps[0]).toEqual({
    ...consumerDeps,
    devDependencies: {
      'flow-bin': '^0.136.0',
    },
  });

  expectSortedDeps(deps);
});

test('Generic package (No updates)', () => {
  const genericDeps: LocalPackageDeps = {
    packageName: 'cli-thing',
    dependencies: {
      'extra-dep': '0.0.1',
    },
    outOfTreePlatform: false,
  };

  const deps = calcPackageDependencies('0.63.0', rnDiff, repoConfigDiff, [
    genericDeps,
  ]);

  expect(deps.length).toEqual(1);
  expect(deps[0]).toEqual(genericDeps);

  expectSortedDeps(deps);
});

test('Generic package (Newer repo-config devDependency)', () => {
  const genericDeps: LocalPackageDeps = {
    packageName: 'cli-thing',
    dependencies: {
      'extra-dep': '0.0.1',
    },
    devDependencies: {
      'flow-bin': '^0.113.0',
    },
    outOfTreePlatform: false,
  };

  const deps = calcPackageDependencies('0.63.0', rnDiff, repoConfigDiff, [
    genericDeps,
  ]);

  expect(deps.length).toEqual(1);
  expect(deps[0]).toEqual({
    ...genericDeps,
    devDependencies: {
      'flow-bin': '^0.136.0',
    },
  });

  expectSortedDeps(deps);
});

test('RN library (Wildcard peer dependency unchanged)', () => {
  const libraryDeps: LocalPackageDeps = {
    packageName: '@react-native/tester',
    peerDependencies: {
      'react-native': '*',
    },
    outOfTreePlatform: false,
  };

  const deps = calcPackageDependencies('0.63.0', rnDiff, repoConfigDiff, [
    libraryDeps,
  ]);

  expect(deps.length).toEqual(1);
  expect(deps[0]).toEqual(libraryDeps);
});

test('Mixed types', () => {
  const outOfTreeDeps: LocalPackageDeps = {
    ...olderReactNative,
    dependencies: {
      ...olderReactNative.dependencies,
      'extra-dependency': '0.0.1',
    },
    packageName: 'react-native-windows',
    outOfTreePlatform: true,
  };

  const consumerDeps: LocalPackageDeps = {
    packageName: 'e2e-test',
    dependencies: {
      'extra-dep': '0.0.1',
      'react-native': '^0.63.0',
      react: '16.13.0',
    },
    outOfTreePlatform: false,
  };

  const genericDeps: LocalPackageDeps = {
    packageName: 'cli-thing',
    dependencies: {
      'extra-dep': '0.0.1',
    },
    devDependencies: {
      'flow-bin': '^0.113.0',
    },
    outOfTreePlatform: false,
  };

  const deps = calcPackageDependencies('0.63.0', rnDiff, repoConfigDiff, [
    outOfTreeDeps,
    consumerDeps,
    genericDeps,
  ]);

  expect(deps.length).toEqual(3);
  expect(deps[0]).toEqual({
    ...outOfTreeDeps,
    dependencies: {
      ...newerReactNative.dependencies,
      'extra-dependency': '0.0.1',
    },
    peerDependencies: newerReactNative.peerDependencies,
    devDependencies: newerReactNative.devDependencies,
  });

  expect(deps[1]).toEqual({
    ...consumerDeps,
    dependencies: {
      'extra-dep': '0.0.1',
      'react-native': '^0.63.0',
      react: '16.13.1',
    },
  });

  expect(deps[2]).toEqual({
    ...genericDeps,
    devDependencies: {
      'flow-bin': '^0.136.0',
    },
  });

  expectSortedDeps(deps);
});

function expectSortedDeps(packages: LocalPackageDeps[]) {
  packages.forEach(pkg => {
    expectSortedArray(Object.keys(pkg.dependencies || {}));
    expectSortedArray(Object.keys(pkg.peerDependencies || {}));
    expectSortedArray(Object.keys(pkg.devDependencies || {}));
  });
}

function expectSortedArray<T>(arr: T[]) {
  expect(arr).toEqual([...arr].sort());
}
