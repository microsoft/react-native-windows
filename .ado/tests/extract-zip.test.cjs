// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

const assert = require('node:assert/strict');
const {promises: fs} = require('node:fs');
const os = require('node:os');
const path = require('node:path');
const {test} = require('node:test');
const {deflateRawSync} = require('node:zlib');
const extract = require('extract-zip');

function crc32(data) {
  let crc = 0xffffffff;
  for (const byte of data) {
    crc ^= byte;
    for (let bit = 0; bit < 8; bit++) {
      crc = (crc >>> 1) ^ (crc & 1 ? 0xedb88320 : 0);
    }
  }
  return (crc ^ 0xffffffff) >>> 0;
}

// Generate small ZIPs in memory, including duplicate names and Unix symlinks.
function zip(entries) {
  const files = [];
  const directory = [];
  let offset = 0;
  for (const {name, text = '', mode = 0o100644, compress = false} of entries) {
    const filename = Buffer.from(name);
    const data = Buffer.from(text);
    const contents = compress ? deflateRawSync(data) : data;
    const local = Buffer.alloc(30);
    local.writeUInt32LE(0x04034b50);
    local.writeUInt16LE(20, 4);
    local.writeUInt16LE(compress ? 8 : 0, 8);
    local.writeUInt32LE(crc32(data), 14);
    local.writeUInt32LE(contents.length, 18);
    local.writeUInt32LE(data.length, 22);
    local.writeUInt16LE(filename.length, 26);
    files.push(local, filename, contents);

    const central = Buffer.alloc(46);
    central.writeUInt32LE(0x02014b50);
    central.writeUInt16LE((3 << 8) | 20, 4);
    central.writeUInt16LE(20, 6);
    central.writeUInt16LE(compress ? 8 : 0, 10);
    central.writeUInt32LE(crc32(data), 16);
    central.writeUInt32LE(contents.length, 20);
    central.writeUInt32LE(data.length, 24);
    central.writeUInt16LE(filename.length, 28);
    central.writeUInt32LE((mode << 16) >>> 0, 38);
    central.writeUInt32LE(offset, 42);
    directory.push(central, filename);
    offset += local.length + filename.length + contents.length;
  }
  const central = Buffer.concat(directory);
  const end = Buffer.alloc(22);
  end.writeUInt32LE(0x06054b50);
  end.writeUInt16LE(entries.length, 8);
  end.writeUInt16LE(entries.length, 10);
  end.writeUInt32LE(central.length, 12);
  end.writeUInt32LE(offset, 16);
  return Buffer.concat([...files, central, end]);
}

async function fixture(t, entries) {
  const root = await fs.mkdtemp(path.join(os.tmpdir(), 'rnw-extract-zip-'));
  t.after(() => fs.rm(root, {recursive: true, force: true}));
  const dir = path.join(await fs.realpath(root), 'destination');
  const archive = path.join(root, 'archive.zip');
  await fs.mkdir(dir);
  await fs.writeFile(archive, zip(entries));
  return {root, dir, archive};
}

async function symlink(t, target, link, type = 'file') {
  try {
    await fs.symlink(target, link, type);
    return true;
  } catch (error) {
    if (process.platform !== 'win32' || error.code !== 'EPERM') {
      throw error;
    }
    t.skip('Windows runner does not grant symbolic-link creation privileges');
    return false;
  }
}

test('extracts stored/deflated files, directories, and invokes onEntry', async t => {
  const entries = [
    {name: 'folder/', mode: 0o40755},
    {name: 'folder/plain.txt', text: 'plain'},
    {name: 'nested/compressed.txt', text: 'compressed'.repeat(100), compress: true},
  ];
  const {dir, archive} = await fixture(t, entries);
  const visited = [];
  await extract(archive, {
    dir,
    onEntry: entry => visited.push(entry.fileName),
  });
  assert.deepEqual(visited, entries.map(entry => entry.name));
  assert.equal(await fs.readFile(path.join(dir, 'folder/plain.txt'), 'utf8'), 'plain');
  assert.equal(
    await fs.readFile(path.join(dir, 'nested/compressed.txt'), 'utf8'),
    'compressed'.repeat(100),
  );
});

test('preserves overwrites of regular files and duplicate regular entries', async t => {
  const {dir, archive} = await fixture(t, [
    {name: 'file.txt', text: 'first'},
    {name: 'file.txt', text: 'last'},
  ]);
  await fs.writeFile(path.join(dir, 'file.txt'), 'original');
  await extract(archive, {dir});
  assert.equal(await fs.readFile(path.join(dir, 'file.txt'), 'utf8'), 'last');
});

test('rejects a symbolic-link leaf before writing on every platform', async t => {
  const {dir, archive} = await fixture(t, [{name: 'file.txt', text: 'replacement'}]);
  const destination = path.join(dir, 'file.txt');
  await fs.writeFile(destination, 'original');
  const lstat = fs.lstat.bind(fs);
  t.mock.method(fs, 'lstat', async (...args) =>
    args[0] === destination ? {isSymbolicLink: () => true} : lstat(...args),
  );
  await assert.rejects(extract(archive, {dir}), /Refusing to overwrite symbolic link/);
  assert.equal(await fs.readFile(destination, 'utf8'), 'original');
});

test('does not swallow lstat errors other than ENOENT', async t => {
  const {dir, archive} = await fixture(t, [{name: 'file.txt', text: 'replacement'}]);
  const destination = path.join(dir, 'file.txt');
  const error = Object.assign(new Error('permission denied'), {code: 'EACCES'});
  const lstat = fs.lstat.bind(fs);
  t.mock.method(fs, 'lstat', async (...args) => {
    if (args[0] === destination) {
      throw error;
    }
    return lstat(...args);
  });
  await assert.rejects(extract(archive, {dir}), value => value === error);
  await assert.rejects(fs.stat(destination), {code: 'ENOENT'});
});

test('rejects a pre-existing symlink to a file outside the destination', async t => {
  const {root, dir, archive} = await fixture(t, [{name: 'link', text: 'replacement'}]);
  const target = path.join(root, 'canary.txt');
  await fs.writeFile(target, 'original');
  if (!(await symlink(t, target, path.join(dir, 'link')))) {
    return;
  }
  await assert.rejects(extract(archive, {dir}), /Refusing to overwrite symbolic link/);
  assert.equal(await fs.readFile(target, 'utf8'), 'original');
});

test('rejects a dangling symlink without creating its outside target', async t => {
  const {root, dir, archive} = await fixture(t, [{name: 'link', text: 'replacement'}]);
  const target = path.join(root, 'missing.txt');
  if (!(await symlink(t, target, path.join(dir, 'link')))) {
    return;
  }
  await assert.rejects(extract(archive, {dir}), /Refusing to overwrite symbolic link/);
  await assert.rejects(fs.stat(target), {code: 'ENOENT'});
});

test('rejects a symlink archive entry followed by a same-named regular file', async t => {
  const {root, dir, archive} = await fixture(t, [
    {name: 'link', text: '../canary.txt', mode: 0o120777},
    {name: 'link', text: 'replacement'},
  ]);
  const target = path.join(root, 'canary.txt');
  await fs.writeFile(target, 'original');
  const probe = path.join(root, 'symlink-probe');
  if (!(await symlink(t, target, probe))) {
    return;
  }
  await fs.unlink(probe);
  await assert.rejects(extract(archive, {dir}), /Refusing to overwrite symbolic link/);
  assert.equal(await fs.readFile(target, 'utf8'), 'original');
  assert.equal((await fs.lstat(path.join(dir, 'link'))).isSymbolicLink(), true);
});

test('preserves legitimate symbolic-link archive entries', async t => {
  const {root, dir, archive} = await fixture(t, [
    {name: 'file.txt', text: 'original'},
    {name: 'link', text: 'file.txt', mode: 0o120777},
  ]);
  const probe = path.join(root, 'symlink-probe');
  if (!(await symlink(t, archive, probe))) {
    return;
  }
  await fs.unlink(probe);
  await extract(archive, {dir});
  assert.equal((await fs.lstat(path.join(dir, 'link'))).isSymbolicLink(), true);
  assert.equal(await fs.readFile(path.join(dir, 'link'), 'utf8'), 'original');
});

test('does not follow a symlink leaf even when its target is inside the destination', async t => {
  const {dir, archive} = await fixture(t, [{name: 'link', text: 'replacement'}]);
  const target = path.join(dir, 'file.txt');
  await fs.writeFile(target, 'original');
  if (!(await symlink(t, target, path.join(dir, 'link')))) {
    return;
  }
  await assert.rejects(extract(archive, {dir}), /Refusing to overwrite symbolic link/);
  assert.equal(await fs.readFile(target, 'utf8'), 'original');
});
