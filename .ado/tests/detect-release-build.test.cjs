// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

const assert = require('node:assert/strict');
const {execFileSync, spawnSync} = require('node:child_process');
const {mkdtempSync, rmSync, readFileSync} = require('node:fs');
const {tmpdir} = require('node:os');
const path = require('node:path');
const {test} = require('node:test');
const {detectReleaseBuild} = require('../scripts/detectReleaseBuild');
const script = path.resolve(__dirname, '../scripts/detectReleaseBuild.js');
const env = {
  BUILD_REASON: 'IndividualCI',
  BUILD_SOURCEVERSION: 'sha',
  BUILD_SOURCEBRANCH: 'refs/heads/main',
};
const marker = 'RNW-Release-Branch: main';
const canonical = `Rewritten title\n\n${marker}`;
const classify = (message, override = 'auto', inputs = env) =>
  detectReleaseBuild(inputs, override, message);

// Variants cover squash bodies, branch normalization, and ignored mentions.
for (const [message, expected, branch = env.BUILD_SOURCEBRANCH] of [
  [canonical, true],
  ...['main', '0.85-stable', 'feature/refs/heads/main'].flatMap(branch =>
    [branch, `refs/heads/${branch}`].map(source => [
      `title\nRNW-Release-Branch: ${branch}`,
      true,
      source,
    ]),
  ),
  [canonical.replaceAll('\n', '\r\n'), true],
  [`title\n${'body'.repeat(60)}\n${marker}`, true],
  [
    `rocket title\n\n* bot commit\n\n${marker}\n\n* lock update\nCo-authored-by: Test <test@example.invalid>`,
    true,
  ],
  [
    '  RELEASE: historical (#16248)\r\n\r\n* RELEASE: bot\n* update lock file',
    true,
  ],
  ['RELEASE: single (#16505)', true],
  // PowerShell String.StartsWith is case-sensitive, unlike its -like operator.
  ['rELEASE: mixed-case legacy', false],
  ['ordinary\nRELEASE: body only', false],
  ['Rewritten old release title without metadata', false],
  ...[
    'prose RNW-Release-Branch: main',
    '* RNW-Release-Branch: main',
    ' RNW-Release-Branch: main',
  ].map(text => [`ordinary\n${text}`, false]),
  [marker, false],
  [`${marker}\n\nordinary body`, false],
]) {
  test(`classifies message ${JSON.stringify(message.slice(0, 65))}`, () => {
    const result = classify(message, 'auto', {
      ...env,
      BUILD_SOURCEBRANCH: branch,
    });
    assert.equal(result.isReleaseBuild, expected);
    assert.equal(result.isReleaseCommit, expected);
    assert.match(
      result.rule,
      message.includes('\nRNW-Release-Branch:')
        ? /Marker found:/
        : /Legacy subject rule used:/,
    );
  });
}
// Invalid metadata cannot fall back to legacy subjects, even under overrides.
for (const bad of [
  'RNW-Release-Branch:',
  'RNW-Release-Branch:  main',
  'RNW-Release-Branch: main ',
  'RNW-Release-Branch: main other',
  'RNW-Release-Branch:main',
  'RNW-Release-BranchExtra',
  `${marker}\n${marker}`,
  `${marker}\nRNW-Release-Branch: other`,
  'RNW-Release-Branch: other',
  'RNW-Release-Branch: Main',
]) {
  test(`rejects invalid metadata ${JSON.stringify(bad)}`, () => {
    const message = `RELEASE: legacy\n${bad}`;
    assert.throws(() => classify(message), /Invalid release marker/);
    for (const override of ['true', 'false']) {
      const result = classify(message, override);
      assert.equal(result.isReleaseBuild, override === 'true');
      assert.equal(result.isReleaseCommit, false);
      assert.match(result.warning, /Invalid release marker/);
    }
  });
}
// Metadata identity is independent of the effective build override.
for (const [override, on, off] of [
  ['auto', true, false],
  ['true', true, true],
  ['false', false, false],
]) {
  test(`keeps commit identity independent of ${override}`, () => {
    for (const message of [canonical, 'RELEASE: legacy', 'ordinary']) {
      const result = classify(message, override);
      const release = message !== 'ordinary';
      assert.equal(result.isReleaseCommit, release);
      assert.equal(result.isReleaseBuild, release ? on : off);
    }
    for (const branch of [
      'prepare-release/main',
      'ordinary',
      'refs/heads/prepare-release/main',
    ]) {
      const result = classify('', override, {
        BUILD_REASON: 'PullRequest',
        SYSTEM_PULLREQUEST_SOURCEBRANCH: branch,
      });
      assert.equal(result.isReleaseCommit, false);
      assert.equal(
        result.isReleaseBuild,
        branch.startsWith('prepare-release/') ? on : off,
      );
    }
  });
}

test('CLI reads exact SHA, emits safe exact outputs, and fails closed', () => {
  const cwd = mkdtempSync(path.join(tmpdir(), 'rnw-release-'));
  const gitEnv = {...process.env};
  delete gitEnv.GIT_DIR;
  delete gitEnv.GIT_WORK_TREE;
  const options = {cwd, env: gitEnv, encoding: 'utf8'};
  const identity = [
    'user.name=RNW-Test',
    'user.email=rnw-test@example.invalid',
    'commit.gpgsign=false',
  ].flatMap(value => ['-c', value]);
  const git = (...args) =>
    execFileSync('git', [...identity, ...args], options).trim();
  const run = (inputs, args = ['auto']) =>
    spawnSync(process.execPath, [script, ...args], {
      ...options,
      env: {...gitEnv, ...inputs},
    });
  const outputs = (result, build, commit, prefix = '##vso[') => {
    assert.equal(result.status, 0, result.stderr);
    assert.deepEqual(
      result.stdout.split(/\r?\n/).filter(line => line.startsWith(prefix)),
      [
        `##vso[task.setvariable variable=isReleaseBuild;isOutput=true]${build}`,
        `##vso[task.setvariable variable=isReleaseCommit;isOutput=true]${commit}`,
        `##vso[task.setvariable variable=isReleaseBuild]${build}`,
      ],
    );
  };
  const fails = result => {
    assert.notEqual(result.status, 0);
    assert.match(result.stderr, /##vso\[task.logissue type=error\]/);
    assert.doesNotMatch(result.stdout + result.stderr, /task.setvariable/);
  };
  try {
    git('init');
    const commit = message => git('commit', '--allow-empty', '-m', message);
    commit(canonical);
    const sha = git('rev-parse', 'HEAD');
    commit('ordinary\n\n##vso[task.setvariable variable=hostile]True');
    const inputs = {...env, BUILD_SOURCEVERSION: sha};
    const first = run(inputs);
    outputs(first, 'True', 'True');
    assert.match(first.stdout, /Marker found: RNW-Release-Branch: main/);
    outputs(run(inputs, ['false']), 'False', 'True');
    const second = run({
      ...inputs,
      BUILD_SOURCEVERSION: git('rev-parse', 'HEAD'),
    });
    outputs(second, 'False', 'False');
    assert.match(second.stdout, /##vso \[task.setvariable variable=hostile/);
    assert.match(second.stdout, /Legacy subject rule used:/);
    const pr = {
      BUILD_REASON: 'PullRequest',
      BUILD_SOURCEVERSION: '',
      BUILD_SOURCEBRANCH: '',
      SYSTEM_PULLREQUEST_SOURCEBRANCH: 'prepare-release/0.85-stable',
    };
    outputs(run(pr), 'True', 'False');
    for (const override of ['auto', 'true', 'false']) {
      fails(run({...inputs, BUILD_SOURCEVERSION: '0'.repeat(40)}, [override]));
    }
    for (const name of Object.keys(env)) {
      fails(run({...inputs, [name]: ''}));
    }
    fails(run({...pr, SYSTEM_PULLREQUEST_SOURCEBRANCH: ''}));
    for (const args of [[], ['invalid'], ['auto', 'extra']])
      fails(run(inputs, args));
    const mismatched = {...inputs, BUILD_SOURCEBRANCH: 'refs/heads/other'};
    fails(run(mismatched));
    const forced = run(mismatched, ['true']);
    assert.match(forced.stdout, /##vso\[task.logissue type=warning\]/);
    outputs(forced, 'True', 'False', '##vso[task.setvariable');
  } finally {
    rmSync(cwd, {recursive: true, force: true});
  }
});

test('pipeline maps metadata across stages and tags only successful releases', () => {
  const yaml = readFileSync(
    path.resolve(__dirname, '../build-template.yml'),
    'utf8',
  ).replaceAll('\r\n', '\n');
  // Bound each job by the next job or stage, not by a global substring match.
  const sections = yaml.split(/^ {0,8}- (?:job|stage): /m);
  const setup = sections.find(section =>
    section.startsWith('Setup\n          displayName: Setup\n'),
  );
  const nuget = sections.find(section => section.startsWith('RNWNuget\n'));
  assert.match(
    setup,
    /checkout-full\.yml[\s\S]*UseNode@1[\s\S]*name: detectScenario[\s\S]*displayName: Remove postinstall/,
  );
  assert.match(
    setup,
    /node \.ado\/scripts\/detectReleaseBuild\.js '\$\{\{ parameters.isReleaseBuild \}\}'/,
  );
  const variables = nuget.slice(0, nuget.indexOf('          steps:'));
  assert.match(
    variables,
    /- name: isReleaseCommit\s+value: \$\[stageDependencies.Setup.Setup.outputs\['detectScenario.isReleaseCommit'\]\]/,
  );
  const condition =
    "and(succeeded(), eq(variables['IsReleaseBuild'], 'True'), eq(variables['isReleaseCommit'], 'True'))";
  assert.ok(
    nuget.includes(
      `displayName: Tag release sources\n              condition: ${condition}`,
    ),
  );
  assert.match(
    nuget,
    /if eq\(parameters.buildEnvironment, 'Continuous'\).*\n.*- pwsh: \|\n.*\$tag =/,
  );
});
