/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as React from 'react';

/**
 * Result from a single profiler measurement.
 */
export interface ProfilerResult {
  phase: 'mount' | 'update' | 'nested-update';
  actualDuration: number;
  baseDuration: number;
  startTime: number;
  commitTime: number;
}

export type OnRenderCallback = (result: ProfilerResult) => void;

interface PerfProfilerProps {
  id: string;
  onResult: OnRenderCallback;
  children: React.ReactNode;
}

/**
 * Wrapper around React.Profiler that collects structured performance data.
 */
export function PerfProfiler({
  id,
  onResult,
  children,
}: PerfProfilerProps): React.ReactElement {
  const onRender = React.useCallback(
    (
      _id: string,
      phase: 'mount' | 'update' | 'nested-update',
      actualDuration: number,
      baseDuration: number,
      startTime: number,
      commitTime: number,
    ) => {
      onResult({
        phase,
        actualDuration,
        baseDuration,
        startTime,
        commitTime,
      });
    },
    [onResult],
  );

  return (
    <React.Profiler id={id} onRender={onRender}>
      {children}
    </React.Profiler>
  );
}
