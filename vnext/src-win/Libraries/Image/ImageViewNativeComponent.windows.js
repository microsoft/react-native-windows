
});

export const __INTERNAL_VIEW_CONFIG: PartialViewConfig =
  Platform.OS === 'android'
    ? {
        uiViewClassName: 'RCTImageView',
        bubblingEventTypes: {},
        directEventTypes: {
          topLoadStart: {
            registrationName: 'onLoadStart',
          },
          topProgress: {
            registrationName: 'onProgress',
          },
          topError: {
            registrationName: 'onError',
          },
          topLoad: {
            registrationName: 'onLoad',
          },
          topLoadEnd: {
            registrationName: 'onLoadEnd',
          },
        },
        validAttributes: {
          blurRadius: true,
          defaultSource: {
            process: require('./resolveAssetSource'),
          },
          internal_analyticTag: true,
          resizeMethod: true,
          resizeMode: true,
          resizeMultiplier: true,
          tintColor: {
            process: require('../StyleSheet/processColor').default,
          },
          borderBottomLeftRadius: true,
          borderTopLeftRadius: true,
          src: true,
          // NOTE: New Architecture expects this to be called `source`,
          // regardless of the platform, therefore propagate it as well.
          // For the backwards compatibility reasons, we keep both `src`
          // and `source`, which will be identical at this stage.
          source: true,
          borderRadius: true,
          headers: true,
          shouldNotifyLoadEvents: true,
          overlayColor: {
            process: require('../StyleSheet/processColor').default,
          },
          borderColor: {
            process: require('../StyleSheet/processColor').default,
          },
          accessible: true,
          progressiveRenderingEnabled: true,
          fadeDuration: true,
          borderBottomRightRadius: true,
          borderTopRightRadius: true,
          loadingIndicatorSrc: true,
        },
      }
    : {
        uiViewClassName: 'RCTImageView',
        bubblingEventTypes: {},
        directEventTypes: {
          topLoadStart: {
            registrationName: 'onLoadStart',
          },
          topProgress: {
            registrationName: 'onProgress',
          },
          topError: {
            registrationName: 'onError',
          },
          topPartialLoad: {
            registrationName: 'onPartialLoad',
          },
          topLoad: {
            registrationName: 'onLoad',
          },
          topLoadEnd: {
            registrationName: 'onLoadEnd',
          },
        },
        validAttributes: {
          blurRadius: true,
          capInsets: {
            diff: require('../Utilities/differ/insetsDiffer'),
          },
          defaultSource: {
            process: require('./resolveAssetSource'),
          },
          internal_analyticTag: true,
          resizeMode: true,
          source: true,
          tintColor: {
            process: require('../StyleSheet/processColor').default,
          },
          ...ConditionallyIgnoredEventHandlers({
            onLoadStart: true,
            onLoad: true,
            onLoadEnd: true,
            onProgress: true,
            onError: true,
            onPartialLoad: true,
          }),
        },
      };

const ImageViewNativeComponent: HostComponent<Props> =
  NativeComponentRegistry.get<Props>(
    'RCTImageView',
    () => __INTERNAL_VIEW_CONFIG,
  );

export default ImageViewNativeComponent;