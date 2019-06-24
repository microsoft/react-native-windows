#pragma once

#include <wrl.h>
#include <d2d1_1.h>
#include <d2d1effects_2.h>
#include <functional>

#include <windows.graphics.effects.interop.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.h>

#define CATCH_RETURN \
        return S_OK; \
    } catch (...) { \
        auto hr = winrt::to_hresult(); \
        __analysis_assume(FAILED(hr)); \
        return hr; 

namespace abi
{
    using namespace ABI::Windows::Foundation;
    using namespace ABI::Windows::Graphics::Effects;
}

namespace winrt
{
    using namespace winrt::Microsoft::UI::Composition::Effects;
    using namespace winrt::Windows::Foundation;
    using namespace winrt::Windows::Graphics::Effects;
    using namespace winrt::Windows::UI;
}

inline winrt::IGraphicsEffectSource& to_winrt(abi::IGraphicsEffectSource*& instance)
{
    return reinterpret_cast<winrt::IGraphicsEffectSource&>(instance);
}

inline winrt::IPropertyValue& to_winrt(abi::IPropertyValue*& instance)
{
    return reinterpret_cast<winrt::IPropertyValue&>(instance);
}

namespace winrt::Microsoft::UI::Composition::Effects::implementation
{
    // Base class for Win2D-like effect descriptions
    class EffectBase :
        public winrt::implements<
        EffectBase,
        abi::IGraphicsEffectD2D1Interop>
    {
    protected:
        // This is a header file so we can't use "using namespace", but we can do this:
        typedef winrt::Color UIColor; // Renamed because we use "Color" as a field name
        typedef winrt::PropertyValue PropertyValue;
        typedef abi::GRAPHICS_EFFECT_PROPERTY_MAPPING PropertyMapping;

    public:
        // IGraphicsEffect
        winrt::hstring Name() { return m_Name; }
        void Name(winrt::hstring const& value) { m_Name = value; }

        // IGraphicsEffectD2D1Interop
        IFACEMETHODIMP GetSourceCount(_Out_ UINT* count) override { *count = 0; return S_OK; }
        IFACEMETHODIMP GetPropertyCount(_Out_ UINT* count) override { *count = 0; return S_OK; }

        IFACEMETHODIMP GetSource(UINT, _Outptr_ abi::IGraphicsEffectSource**) override
        {
            return E_INVALIDARG;
        }

        IFACEMETHODIMP GetProperty(UINT, _Outptr_ abi::IPropertyValue**) override
        {
            return E_INVALIDARG;
        }

        IFACEMETHODIMP GetNamedPropertyMapping(LPCWSTR, _Out_ UINT*,
            _Out_ abi::GRAPHICS_EFFECT_PROPERTY_MAPPING*) override
        {
            return E_INVALIDARG;
        }

    protected:
        // Invokes a functor with the pointer to the property factory
        static HRESULT UsePropertyFactory(_Outptr_ abi::IPropertyValue** value, std::function<winrt::IInspectable()> const& func) try
        {
            auto ret = func();
            auto propertyValue = ret.as<winrt::IPropertyValue>();
            to_winrt(*value) = propertyValue;
            CATCH_RETURN;
        }

        template<UINT32 ComponentCount>
        static winrt::IInspectable CreateColor(UIColor color)
        {
            static_assert(ComponentCount == 3 || ComponentCount == 4, "Unexpected color component count.");
            float values[] = { color.R / 255.0f, color.G / 255.0f, color.B / 255.0f, color.A / 255.0f };
            return winrt::PropertyValue::CreateSingleArray(reinterpret_cast<std::array<float, ComponentCount>&>(values));
        }

        // Helpers to implement GetNamedPropertyMapping more succintly
        struct NamedProperty
        {
            const wchar_t* Name; // Compile-time constant
            UINT Index; // Property index
            abi::GRAPHICS_EFFECT_PROPERTY_MAPPING Mapping;
        };

        HRESULT GetNamedPropertyMappingImpl(
            _In_count_(mappingCount) const NamedProperty* namedProperties,
            UINT namedPropertyCount,
            LPCWSTR name,
            _Out_ UINT* index,
            _Out_ abi::GRAPHICS_EFFECT_PROPERTY_MAPPING* mapping)
        {
            for (UINT i = 0; i < namedPropertyCount; ++i)
            {
                const auto& prop = namedProperties[i];
                if (_wcsicmp(name, prop.Name) == 0)
                {
                    *index = prop.Index;
                    *mapping = prop.Mapping;
                    return S_OK;
                }
            }
            return E_INVALIDARG;
        }

        // M_PI requires us to be the first to include math.h, not worth it
        static constexpr float k_PI = 3.14159265358979f;
        static constexpr float k_DegreesPerRadian = 180.0f / k_PI;

    public:
        winrt::hstring m_Name;
    };

    //-----------------------------------------------------------------------------------------------------------------
    // Helper macros to make implementation more succint
    //-----------------------------------------------------------------------------------------------------------------

#pragma push_macro("DECLARE_D2D_GUID")
#undef DECLARE_D2D_GUID
#define DECLARE_D2D_GUID(Guid) \
    IFACEMETHODIMP GetEffectId(_Out_ GUID * id) override { *id = Guid; return S_OK; }

#pragma push_macro("DECLARE_SOURCE")
#undef DECLARE_SOURCE
#define DECLARE_SOURCE(Name) \
    winrt::IGraphicsEffectSource m_##Name; \
    winrt::IGraphicsEffectSource Name() { return m_##Name; } \
    void Name(winrt::IGraphicsEffectSource const& value) { m_##Name = value; }

#pragma push_macro("DECLARE_SINGLE_SOURCE")
#undef DECLARE_SINGLE_SOURCE
#define DECLARE_SINGLE_SOURCE(Name) \
    DECLARE_SOURCE(Name) \
    IFACEMETHODIMP GetSourceCount(_Out_ UINT * count) override { *count = 1; return S_OK; } \
    IFACEMETHODIMP GetSource(UINT index, _Outptr_ abi::IGraphicsEffectSource ** source) override try \
    { \
        if (index == 0) to_winrt(*source) = m_##Name; \
        else throw winrt::hresult_invalid_argument(); \
        CATCH_RETURN; \
    }

#pragma push_macro("DECLARE_POD_PROPERTY")
#undef DECLARE_POD_PROPERTY
#define DECLARE_POD_PROPERTY(Name, Type, InitialValue, Condition) \
    private: \
    Type m_##Name = InitialValue; \
    public: \
    Type Name() { return m_##Name; } \
    void Name(Type const& value) \
    { \
        if (!(0, Condition)) { throw winrt::hresult_invalid_argument(); } \
        m_##Name = value; \
    }

#pragma push_macro("DECLARE_NAMED_PROPERTY_MAPPING")
#undef DECLARE_NAMED_PROPERTY_MAPPING
#define DECLARE_NAMED_PROPERTY_MAPPING(...) \
    IFACEMETHODIMP GetNamedPropertyMapping(LPCWSTR name, _Out_ UINT * index, \
        _Out_ abi::GRAPHICS_EFFECT_PROPERTY_MAPPING * mapping) override \
    { \
        static const NamedProperty s_Properties[] = { __VA_ARGS__ }; \
        return GetNamedPropertyMappingImpl(s_Properties, _countof(s_Properties), name, index, mapping); \
    }
}
