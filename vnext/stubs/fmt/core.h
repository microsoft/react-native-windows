// stub, not used by RNW
// Remove with https://github.com/microsoft/react-native-windows/issues/7634

namespace fmt {
    std::basic_string<char> format(std::string s, size_t arg1, size_t arg2) {
        assert(false);
        std::terminate();
    }
}