/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<d0064a285cd1e7485aad687e93a94b5a>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\Blob\BlobTypes.js
 */

export declare type BlobCollector = symbol & {
  __BlobCollector__: string;
};
export type BlobData = {
  blobId: string;
  offset: number;
  size: number;
  name?: string | undefined;
  type?: string | undefined;
  lastModified?: number | undefined;
};
export type BlobOptions = {
  type: string;
  lastModified: number;
};
