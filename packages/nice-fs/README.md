# nice-fs

`nice-fs` is a minimal-dependency drop-in replacement to `fs` with changes for resiliency, promises, and convenience. It has several opinionated changes, targeted towards cross-platform CLI apps.

![Usage Thumbnail](./assets/logo.png)

## Async Usage (Default)

`nice-fs` exposes a Promise-based API, mostly matching that of `fs.promises`, with several methods added [extra methods](#Extra-Methods).

```ts
// import {promises as fs} from 'fs'
import fs from 'nice-fs';

const fileContent = await fs.readFile('foo.txt');
```

## Sync Usage

`nice-fs` exports all `fs.*Sync` Where an async version has a graceful implementation, and the synchronous version does not, the method is marked as deprecated.

```ts
// import fs from 'fs'
import fs from 'nice-fs';

const fileContent = await fs.readFileSync('foo.txt');
```

## Extra Methods

### `exists`
NodeJS deprecated `fs.exists`, and removed `fs.promises.exists`. The recommendation is to instead acquire a lock to the file via `fs.open` for the duration of file-use. One-shot existence checks are still useful, and because `fs.existsSync` is not deprecated, more likely means usage of blocking synchronous APIs.

```ts
import fs from 'nice-fs';

const fooExists = await fs.exists('foo.txt');
```

### `readFile.asJson`

`nice-fs` provides convenience methods to handle JSON files. These are implemented as methods under `fs.ReadFile`. The following conversions are added:

| Method | Return type |
|-|-|
| `readFile.asJson<T>` | `Promise<T>` or `Promise<Record<string, unknown> \| Array<unknown>>` |
| `readFileSync.asJson<T>` | `T` or `Record<string, unknown> \| Array<unknown>` |

```ts
import fs from 'nice-fs';

// foo is type: Record<string, unknown>
const foo = await fs.readFile.asJson('foo.json');

// foo is type: FooProps
type FooProps = { name: string, version: string };
const foo = await fs.readFile.asJson<FooProps>('foo.json');
```

## Resiliency

`nice-fs` uses [`graceful-fs`](https://github.com/isaacs/node-graceful-fs) to gracefully handle transient filesystem conditions, at the cost of extra latency. This includes transient `EPERM`, `EACCESS`, `EMFILE`, `ENFILE`. This can be important when handling files that a subject to antivirus, which may temporarily lock mutation of files on Windows.

## eslint
We reccomend adding the following rules to your eslint config if you would like to use `nice-fs` everywhere:
```js
module.exports = {
  rules: {
    'no-restricted-imports': [
      'error', {
        name: 'fs',
        message: 'Please us `nice-fs` instead of `fs`'
      }
    ],
  }
}
```
