/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {AutomationClient} from '@react-native-windows/automation-channel';

/**
 * Schema of tree dumped node
 */
export type UIElement = {
  XamlType: string;
  Foreground?: string | null;
  Background?: string | null;
  Padding?: string | null;
  Margin?: string | null;
  RenderSize?: number[] | null;
  Visibility?: 'Collapsed' | 'Visible' | null;
  CornerRadius?: string | null;
  BorderThickness?: string | null;
  Width?: number | null;
  Height?: number | null;
  BorderBrush?: string | null;
  VerticalAlignment?: string | null;
  HorizontalAlignment?: string | null;
  Clip?: string | null;
  FlowDirection?: string | null;
  Name?: string | null;
  Text?: string | null;
  children?: UIElement[];
  [index: string]: unknown;
};

export type AutomationNode = {
  AutomationId?: string;
  ControlType?: number;
  LocalizedControlType?: string;
  __Children?: [AutomationNode];
};

export type ComponentNode = {
  Type: string;
  _Props?: {
    TestId?: string;
    Sources?: [{Uri?: string}];
  };
  __Children?: [ComponentNode];
};

export type VisualNode = {
  Comment?: string;
  Offset?: `${number} ${number} ${number}`;
  Size?: `${number} ${number}`;
  'Visual Type'?: string;
  __Children?: [VisualNode];
};

export type VisualTree = {
  'Automation Tree': AutomationNode;
  'Component Tree': ComponentNode;
  'Visual Tree': VisualNode;
};

declare global {
  const automationClient: AutomationClient | undefined;
}

/**
 * Dump a section of the native visual tree.
 */
export default async function dumpVisualTree(
  accessibilityId: string,
  opts?: {
    pruneCollapsed?: boolean;
    deterministicOnly?: boolean;
    removeDefaultProps?: boolean;
    removeGuidsFromImageSources?: boolean;
    additionalProperties?: string[];
  },
): Promise<UIElement | VisualTree> {
  if (!automationClient) {
    throw new Error('RPC client is not enabled');
  }

  const dumpResponse = await automationClient.invoke('DumpVisualTree', {
    accessibilityId,
    ...opts,
  });

  if (dumpResponse.type === 'error') {
    throw new Error(dumpResponse.message);
  }

  const element: UIElement | VisualTree = dumpResponse.result;

  if ('XamlType' in element && opts?.pruneCollapsed !== false) {
    pruneCollapsedElements(element);
  }

  if ('XamlType' in element && opts?.deterministicOnly !== false) {
    removeNonDeterministicProps(element);
  }

  if ('XamlType' in element && opts?.removeDefaultProps !== false) {
    removeDefaultProps(element);
  }

  if (!('XamlType' in element) && opts?.removeGuidsFromImageSources !== false) {
    removeGuidsFromImageSources(element);
  }

  return element;
}

function removeGuidsFromImageSourcesHelper(node: ComponentNode) {
  if (node._Props && node._Props.Sources) {
    node._Props.Sources.forEach((source: any) => {
      if (source.Uri) {
        if (source.Uri.startsWith('blob:')) {
          source.Uri = source.Uri.replace(
            /blob:[a-f0-9]+-[a-f0-9]+-[a-f0-9]+-[a-f0-9]+-[a-f0-9]+/,
            'blob:<some_guid_here>',
          );
          source.Uri = source.Uri.replace(/size=\d{5}/, 'size=<size>');
        } else if (
          source.Uri.startsWith(
            'https://www.facebook.com/assets/fb_lite_messaging/E2EE-settings@3x.png?r=1&t=',
          )
        ) {
          source.Uri =
            'https://www.facebook.com/assets/fb_lite_messaging/E2EE-settings@3x.png?r=1&t=<some_hash_here>';
        } else if (
          source.Uri.startsWith(
            'https://www.facebook.com/ar_effect/external_textures/648609739826677.png?hash=',
          )
        ) {
          source.Uri =
            'https://www.facebook.com/ar_effect/external_textures/648609739826677.png?hash=<some_hash_here>';
        } else if (
          source.Uri.startsWith(
            'https://www.facebook.com/ads/pics/successstories.png?hash=',
          )
        ) {
          source.Uri =
            'https://www.facebook.com/ads/pics/successstories.png?hash=<some_hash_here>';
        } else {
          // When getting files from a prebuilt bundle the uri is going to include a local path, which would make snapshots inconsistent,
          // This logic replaces the local path so that we get consistent results.
          // file://E:\\repos\\react-native-windows\\packages\\e2e-test-app-fabric\\windows\\RNTesterApp-Fabric.Package\\bin\\x64\\Release\\AppX\\RNTesterApp-Fabric\\Bundle\\@react-native-windows/tester/js/assets/uie_thumb_normal@2x.png
          // becomes
          // <localOrBundlerUri>@react-native-windows/tester/js/assets/uie_thumb_normal@2x.png
          const packagesPath = require('path')
            .resolve(__dirname, '../../..')
            .replace(/\\/g, '\\\\');
          source.Uri = source.Uri.replace(
            new RegExp(`file://${packagesPath}.*\\\\Bundle\\\\assets/_+`),
            '<localOrBundlerUri>',
          );

          // When loading the bundle from metro local paths will be replaced with paths to localhost, which will not align with snapshots made with prebuilt bundles.
          // This logic replaces the localhost uri, with the same uri that we would have gotten from a prebuild bundle.  This makes it easier to debug without breaking snapshots
          // http://localhost:8081/assets/@@/@react-native-windows/tester/js/assets/uie_thumb_normal@2x.png?platform=windows&hash=c6f5aec4d9e0aa47c0887e4266796224
          // becomes
          // "<localOrBundlerUri>@react-native-windows/tester/js/assets/uie_thumb_normal@2x.png"
          source.Uri = source.Uri.replace(
            /http:\/\/localhost:8081\/assets\/(@@\/)+/,
            '<localOrBundlerUri>',
          );
          source.Uri = source.Uri.replace(/\?platform=windows&hash=[^=]$/, '');
        }
      }
    });
  }
  if (node.__Children) {
    node.__Children.forEach((child: any) =>
      removeGuidsFromImageSourcesHelper(child),
    );
  }
}

function removeGuidsFromImageSources(visualTree: VisualTree) {
  removeGuidsFromImageSourcesHelper(visualTree['Component Tree']);
}

/**
 * Removes trees of XAML that are not visible.
 */
function pruneCollapsedElements(element: UIElement) {
  if (!element.children) {
    return;
  }

  element.children = element.children.filter(
    child => child.Visibility !== 'Collapsed',
  );

  element.children.forEach(pruneCollapsedElements);
}

/**
 * Removes trees of properties that are not deterministic
 */
function removeNonDeterministicProps(element: UIElement) {
  if (element.RenderSize) {
    // RenderSize is subject to rounding, etc and should mostly be derived from
    // other deterministic properties in the tree.
    delete element.RenderSize;
  }

  if (element.children) {
    element.children.forEach(removeNonDeterministicProps);
  }
}

/**
 * Removes noise from snapshot by removing properties with the default value
 */
function removeDefaultProps(element: UIElement) {
  const defaultValues: [string, unknown][] = [['Tooltip', null]];

  defaultValues.forEach(([propname, defaultValue]) => {
    if (element[propname] === defaultValue) {
      delete element[propname];
    }
  });

  if (element.children) {
    element.children.forEach(removeDefaultProps);
  }
}
