// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

'use strict';

let constants = {

  /**
  * Personal access token
  */
  paToken: process.env.VSO_AUTHTOKEN,

  /**
  * This is the repo id for ISS\react-native
  */
  repoId: 'b8bb01d6-6226-4852-a03d-cd1cbc9efc48',

  /**
  * Environment variable set during the build that data will be POST to.
  * @type string
  */
  vsoApiUrl: process.env.VSO_API_URL ? process.env.VSO_API_URL : 'https://office.visualstudio.com/DefaultCollection/_apis',

};


if (!constants.paToken)
	{throw new Error('Personal access token must be set to VSO_AUTHTOKEN before running this script');}

module.exports = constants;

