/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<02ab6dc1eba7fa208ad546f4a60e5059>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\Network\convertRequestBody.js
 */

import type { BlobData } from "../Blob/BlobTypes";
import type { FormDataPart } from "./FormData";
import Blob from "../Blob/Blob";
import FormData from "./FormData";
type URIRequestBody = Readonly<{
  uri: string;
  string?: string | undefined;
  blob?: BlobData | undefined;
  formData?: Array<FormDataPart> | undefined;
  base64?: string | undefined;
}>;
export type RequestBody = string | Blob | FormData | URIRequestBody | ArrayBuffer | ArrayBufferView;
type RequestBodyResult = Readonly<{
  string?: string | undefined;
  blob?: BlobData | undefined;
  formData?: Array<FormDataPart> | undefined;
  base64?: string | undefined;
  uri?: string | undefined;
}>;
declare function convertRequestBody(body: null | undefined | RequestBody): null | undefined | RequestBodyResult;
declare const $$convertRequestBody: typeof convertRequestBody;
declare type $$convertRequestBody = typeof $$convertRequestBody;
export default $$convertRequestBody;
