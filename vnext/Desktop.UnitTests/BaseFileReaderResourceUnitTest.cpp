// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include <CppUnitTest.h>

#include <BaseFileReaderResource.h>

// Windows Libraries
#include <winrt/Windows.Security.Cryptography.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using std::shared_ptr;
using std::string;
using std::vector;
using winrt::array_view;

namespace Microsoft::React::Test {

TEST_CLASS (BaseFileReaderResourceUnitTest) {
  TEST_METHOD(Base64EncodesCorrectly) {
    class DummyBlobPersistor final : public IBlobPersistor {
      std::unordered_map<string, vector<uint8_t>> m_blobs;

     public:
#pragma region IBlobPersistor

      array_view<uint8_t const> ResolveMessage(string &&blobId, int64_t offset, int64_t size) override {
        auto dataItr = m_blobs.find(std::move(blobId));
        // Not found.
        if (dataItr == m_blobs.cend())
          throw std::invalid_argument("Blob object not found");

        auto &bytes = (*dataItr).second;
        auto endBound = static_cast<size_t>(offset + size);
        // Out of bounds.
        if (endBound > bytes.size() || offset >= static_cast<int64_t>(bytes.size()) || offset < 0)
          throw std::out_of_range("Offset or size out of range");

        return array_view<uint8_t const>(bytes.data() + offset, bytes.data() + endBound);
      }

      void RemoveMessage(string && /*blobId*/) noexcept override {
        // Not implemented
      }

      void StoreMessage(vector<uint8_t> &&message, string &&blobId) noexcept override {
        m_blobs.insert_or_assign(std::move(blobId), std::move(message));
      }

      string StoreMessage(vector<uint8_t> && /*message*/) noexcept override {
        return "Not implemented";
      }

#pragma endregion IBlobPersistor
    };

    // Base64 expected value coputed with [System.Convert]::ToBase64String('abcd'.ToCharArray())
    string messageStr = "abcde";
    constexpr char expected[] = "data:string;base64,YWJjZGU=";
    constexpr char guid[] = "93252b5d-a419-4d98-a928-c3ef386f2445";

    vector<uint8_t> message{};
    message.reserve(messageStr.size());
    message.insert(message.end(), messageStr.begin(), messageStr.end());

    shared_ptr<IBlobPersistor> persistor = std::make_shared<DummyBlobPersistor>();
    shared_ptr<IFileReaderResource> reader = std::make_shared<BaseFileReaderResource>(persistor);
    string result;
    auto resolver = [&result](string &&value) { result = std::move(value); };
    auto rejecter = [&result](string &&) { result = "ERROR"; };

    persistor->StoreMessage(std::move(message), string{guid});

    reader->ReadAsDataUrl(guid, 0 /*offset*/, messageStr.size(), "string", std::move(resolver), std::move(rejecter));

    Assert::AreEqual(expected, result.c_str());
  }
};

} // namespace Microsoft::React::Test
