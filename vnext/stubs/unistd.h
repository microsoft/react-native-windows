// react-native includes unistd.h, which isn't available in windows.
// This shouldn't be needed after
// https://github.com/facebook/react-native/pull/25107 is merged
#if !defined(OSS_RN)
#error This stub should not be used unless building against the non-microsoft version of react-native
#endif