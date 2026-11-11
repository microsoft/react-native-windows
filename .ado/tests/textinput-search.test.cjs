// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

const assert = require('node:assert/strict');
const {readFileSync} = require('node:fs');
const path = require('node:path');
const {test} = require('node:test');
const vm = require('node:vm');
const ts = require('typescript');

const file = path.resolve(
  __dirname,
  '../../packages/e2e-test-app-fabric/test/TextInputComponentTest.test.ts',
);
const source = ts.createSourceFile(
  file,
  readFileSync(file, 'utf8'),
  ts.ScriptTarget.Latest,
);
const helper = source.statements
  .filter(ts.isVariableStatement)
  .flatMap(statement => [...statement.declarationList.declarations])
  .find(declaration => declaration.name.getText(source) === 'searchBox');
assert.ok(helper?.initializer, 'Expected the actual TextInput search helper');
const compiled = ts.transpileModule(
  `globalThis.searchBox = ${helper.initializer.getText(source)};`,
  {compilerOptions: {target: ts.ScriptTarget.ES2020}},
).outputText;

async function runSearch({
  partialFirstAttempt = false,
  rejectInput = false,
  diagnostics = [],
  inputProgress,
} = {}) {
  let text = 'Submit behavior';
  let focused = false;
  let attempts = 0;
  let pendingText = [];
  const calls = [];
  const element = {
    async clearValue() {
      calls.push('clear');
      text = '';
      focused = false;
      pendingText = [];
    },
    async click() {
      calls.push('click');
      focused = true;
    },
    async addValue(value) {
      calls.push('type');
      attempts++;
      if (focused && !rejectInput) {
        if (inputProgress) {
          pendingText = [...inputProgress];
        } else {
          text +=
            partialFirstAttempt && attempts === 1 ? value.slice(1) : value;
        }
      }
    },
    async setValue(value) {
      // WebdriverIO implements setValue as clearValue followed by addValue.
      await this.clearValue();
      await this.addValue(value);
    },
    async getText() {
      if (pendingText.length) {
        text = pendingText.shift();
      }
      return text;
    },
    async getAttribute(name) {
      assert.equal(name, 'HasKeyboardFocus');
      return String(focused);
    },
  };
  const context = {
    process: {stderr: {write: message => diagnostics.push(message)}},
    app: {
      async findElementByTestID(id) {
        assert.equal(id, 'example_search');
        return element;
      },
      async waitUntil(condition, options) {
        for (let retry = 0; retry < 3; retry++) {
          if (await condition()) {
            return;
          }
        }
        throw new Error(options.timeoutMsg);
      },
    },
    setTimeout(callback) {
      // Clearing the filter remounts an autoFocus input before typing resumes.
      focused = false;
      callback();
    },
  };
  vm.createContext(context);
  vm.runInContext(compiled, context);
  await context.searchBox('onPressIn');
  assert.equal(text, 'onPressIn');
  return calls;
}

test('TextInput search restores focus after autoFocus remounts without clearing again', async () => {
  assert.deepEqual(await runSearch(), ['clear', 'click', 'type']);
});

test('TextInput search clears incorrect text before retrying instead of appending', async () => {
  assert.deepEqual(await runSearch({partialFirstAttempt: true}), [
    'clear',
    'click',
    'type',
    'clear',
    'click',
    'type',
  ]);
});

for (const inputProgress of [
  ['onPr', 'onPress', 'onPressIn'],
  ['', 'onPr', 'onPressIn'],
]) {
  test(`TextInput search does not interrupt pending input starting with ${JSON.stringify(inputProgress[0])}`, async () => {
    assert.deepEqual(await runSearch({inputProgress}), [
      'clear',
      'click',
      'type',
    ]);
  });
}

test('TextInput search does not accept a permanently truncated prefix', async () => {
  await assert.rejects(
    runSearch({inputProgress: ['onPr']}),
    /Unable to enter correct search text/,
  );
});

test('TextInput search still fails when the requested text cannot be entered', async () => {
  const diagnostics = [];
  await assert.rejects(
    runSearch({rejectInput: true, diagnostics}),
    /Unable to enter correct search text/,
  );
  assert.equal(diagnostics.length, 1);
  assert.deepEqual(
    JSON.parse(diagnostics[0].slice('TextInput search failed: '.length)),
    {
      input: 'onPressIn',
      attempts: Array.from({length: 3}, () => ({text: '', focused: 'true'})),
    },
  );
});
