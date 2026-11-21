/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<93d7ebeb343c4ef6fb68f1de25ad5812>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\Network\FormData.js
 */

type FormDataValue = string | {
  name?: string | undefined;
  type?: string | undefined;
  uri: string;
};
type Headers = {
  [name: string]: string;
};
export type FormDataPart = {
  string: string;
  headers: Headers;
} | {
  uri: string;
  headers: Headers;
  name?: string | undefined;
  type?: string | undefined;
};
/**
 * Polyfill for XMLHttpRequest2 FormData API, allowing multipart POST requests
 * with mixed data (string, native files) to be submitted via XMLHttpRequest.
 *
 * Example:
 *
 *   var photo = {
 *     uri: uriFromCameraRoll,
 *     type: 'image/jpeg',
 *     name: 'photo.jpg',
 *   };
 *
 *   var body = new FormData();
 *   body.append('authToken', 'secret');
 *   body.append('photo', photo);
 *   body.append('title', 'A beautiful photo!');
 *
 *   xhr.open('POST', serverURL);
 *   xhr.send(body);
 */
declare class FormData {
  constructor();
  append(key: string, value: FormDataValue): void;
  getAll(key: string): Array<FormDataValue>;
  getParts(): Array<FormDataPart>;
}
export default FormData;
