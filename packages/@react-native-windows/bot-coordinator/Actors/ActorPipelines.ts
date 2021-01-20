/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

/**
 * Azure pipelines that may be triggered by an actor
 */
export type ActorPipelines = {
  'integrate-react-native': () => Promise<void>;
};
