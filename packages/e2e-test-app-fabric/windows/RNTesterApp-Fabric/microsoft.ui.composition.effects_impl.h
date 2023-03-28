// Copyright (c) Microsoft Corporation. All rights reserved.

// This is a header-only implementation of Win2D-like effects descriptions,
// which can be use to describe effects graph in the Windows::UI::Composition API.

#pragma once

#include <cstring>
#include <wrl.h>
#include <d2d1_1.h>
#include <d2d1effects_2.h>
#include <Windows.Foundation.h>
#include <Windows.Foundation.Numerics.h>
#include <Windows.Graphics.Effects.h>
#include <Windows.Graphics.Effects.Interop.h>
#include <Windows.UI.h>

namespace Microsoft {
	namespace UI {
		namespace Composition {
			namespace Effects
			{
#ifndef BUILD_WINDOWS
				namespace Windows = ::ABI::Windows;
#endif

				//-----------------------------------------------------------------------------------------------------------------
				typedef enum EffectBorderMode
				{
					EffectBorderMode_Soft = 0,
					EffectBorderMode_Hard = 1
				} EffectBorderMode;

				typedef enum EffectOptimization
				{
					EffectOptimization_Speed = 0,
					EffectOptimization_Balanced = 1,
					EffectOptimization_Quality = 2
				} EffectOptimization;

				typedef enum BlendEffectMode
				{
					BlendEffectMode_Multiply = 0,
					BlendEffectMode_Screen = 1,
					BlendEffectMode_Darken = 2,
					BlendEffectMode_Lighten = 3,
					BlendEffectMode_Dissolve = 4,
					BlendEffectMode_ColorBurn = 5,
					BlendEffectMode_LinearBurn = 6,
					BlendEffectMode_DarkerColor = 7,
					BlendEffectMode_LighterColor = 8,
					BlendEffectMode_ColorDodge = 9,
					BlendEffectMode_LinearDodge = 10,
					BlendEffectMode_Overlay = 11,
					BlendEffectMode_SoftLight = 12,
					BlendEffectMode_HardLight = 13,
					BlendEffectMode_VividLight = 14,
					BlendEffectMode_LinearLight = 15,
					BlendEffectMode_PinLight = 16,
					BlendEffectMode_HardMix = 17,
					BlendEffectMode_Difference = 18,
					BlendEffectMode_Exclusion = 19,
					BlendEffectMode_Hue = 20,
					BlendEffectMode_Saturation = 21,
					BlendEffectMode_Color = 22,
					BlendEffectMode_Luminosity = 23,
					BlendEffectMode_Subtract = 24,
					BlendEffectMode_Division = 25
				} BlendEffectMode;

				typedef enum CanvasComposite
				{
					CanvasComposite_SourceOver = (int)0,
					CanvasComposite_DestinationOver = (int)1,
					CanvasComposite_SourceIn = (int)2,
					CanvasComposite_DestinationIn = (int)3,
					CanvasComposite_SourceOut = (int)4,
					CanvasComposite_DestinationOut = (int)5,
					CanvasComposite_SourceAtop = (int)6,
					CanvasComposite_DestinationAtop = (int)7,
					CanvasComposite_Xor = (int)8,
					CanvasComposite_Add = (int)9,
					CanvasComposite_Copy = (int)10,
					CanvasComposite_BoundedCopy = (int)11,
					CanvasComposite_MaskInvert = (int)12
				} CanvasComposite;

				MIDL_INTERFACE("5673248E-7266-5E49-B2AB-2589D5D875C3")
					IBlendEffect : IInspectable
				{
					virtual HRESULT STDMETHODCALLTYPE get_Mode(BlendEffectMode* value) = 0;
				virtual HRESULT STDMETHODCALLTYPE put_Mode(BlendEffectMode value) = 0;
				virtual HRESULT STDMETHODCALLTYPE get_Background(Windows::Graphics::Effects::IGraphicsEffectSource** source) = 0;
				virtual HRESULT STDMETHODCALLTYPE put_Background(Windows::Graphics::Effects::IGraphicsEffectSource* source) = 0;
				virtual HRESULT STDMETHODCALLTYPE get_Foreground(Windows::Graphics::Effects::IGraphicsEffectSource** source) = 0;
				virtual HRESULT STDMETHODCALLTYPE put_Foreground(Windows::Graphics::Effects::IGraphicsEffectSource* source) = 0;
				};

				MIDL_INTERFACE("25F942C7-7FEE-518A-BA7B-22A0060AF7F6")
					IColorSourceEffect : IInspectable
				{
					virtual HRESULT STDMETHODCALLTYPE get_Color(Windows::UI::Color* value) = 0;
				virtual HRESULT STDMETHODCALLTYPE put_Color(Windows::UI::Color value) = 0;
				};

				MIDL_INTERFACE("58360908-1B6B-4302-8ECD-CC24B26F27B1")
					ICompositeStepEffect : IInspectable
				{
					virtual HRESULT STDMETHODCALLTYPE get_Mode(CanvasComposite* value) = 0;
				virtual HRESULT STDMETHODCALLTYPE put_Mode(CanvasComposite value) = 0;
				virtual HRESULT STDMETHODCALLTYPE get_Destination(Windows::Graphics::Effects::IGraphicsEffectSource** source) = 0;
				virtual HRESULT STDMETHODCALLTYPE put_Destination(Windows::Graphics::Effects::IGraphicsEffectSource* source) = 0;
				virtual HRESULT STDMETHODCALLTYPE get_Source(Windows::Graphics::Effects::IGraphicsEffectSource** source) = 0;
				virtual HRESULT STDMETHODCALLTYPE put_Source(Windows::Graphics::Effects::IGraphicsEffectSource* source) = 0;
				};

				MIDL_INTERFACE("A82EC394-6734-5830-9123-2C82B27DD3C0")
					IGaussianBlurEffect : IInspectable
				{
					virtual HRESULT STDMETHODCALLTYPE get_BlurAmount(float* value) = 0;
				virtual HRESULT STDMETHODCALLTYPE put_BlurAmount(float value) = 0;
				virtual HRESULT STDMETHODCALLTYPE get_Optimization(EffectOptimization* value) = 0;
				virtual HRESULT STDMETHODCALLTYPE put_Optimization(EffectOptimization value) = 0;
				virtual HRESULT STDMETHODCALLTYPE get_BorderMode(EffectBorderMode* value) = 0;
				virtual HRESULT STDMETHODCALLTYPE put_BorderMode(EffectBorderMode value) = 0;
				virtual HRESULT STDMETHODCALLTYPE get_Source(Windows::Graphics::Effects::IGraphicsEffectSource** source) = 0;
				virtual HRESULT STDMETHODCALLTYPE put_Source(Windows::Graphics::Effects::IGraphicsEffectSource* source) = 0;
				};

				MIDL_INTERFACE("94B6AD75-C540-51B8-A9D1-544174ADC68D")
					IOpacityEffect : IInspectable
				{
					virtual HRESULT STDMETHODCALLTYPE get_Opacity(float* value) = 0;
				virtual HRESULT STDMETHODCALLTYPE put_Opacity(float value) = 0;
				virtual HRESULT STDMETHODCALLTYPE get_Source(Windows::Graphics::Effects::IGraphicsEffectSource** source) = 0;
				virtual HRESULT STDMETHODCALLTYPE put_Source(Windows::Graphics::Effects::IGraphicsEffectSource* source) = 0;
				};

				//-----------------------------------------------------------------------------------------------------------------

				// Base class for Win2D-like effect descriptions
				template<typename TEffectInterface>
				class EffectBase abstract : public Microsoft::WRL::RuntimeClass<
					Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::WinRtClassicComMix>,
					Windows::Graphics::Effects::IGraphicsEffect,
					Windows::Graphics::Effects::IGraphicsEffectSource,
					Windows::Graphics::Effects::IGraphicsEffectD2D1Interop,
					TEffectInterface>
				{
				protected:
					// This is a header file so we can't use "using namespace", but we can do this:
					typedef Windows::UI::Color UIColor; // Renamed because we use "Color" as a field name
					typedef Windows::Foundation::IPropertyValue IPropertyValue;
					typedef Windows::Foundation::IPropertyValueStatics IPropertyValueStatics;
					typedef Windows::Foundation::Numerics::Vector2 Vector2;
					typedef Windows::Foundation::Numerics::Vector3 Vector3;
					typedef Windows::Foundation::Numerics::Matrix3x2 Matrix3x2;
					typedef Windows::Graphics::Effects::GRAPHICS_EFFECT_PROPERTY_MAPPING GRAPHICS_EFFECT_PROPERTY_MAPPING;
					typedef Windows::Graphics::Effects::GRAPHICS_EFFECT_PROPERTY_MAPPING PropertyMapping;
					typedef Windows::Graphics::Effects::IGraphicsEffectSource IGraphicsEffectSource;

				public:
					// IGraphicsEffect
					IFACEMETHODIMP get_Name(_Out_ HSTRING* name) override { return Name.CopyTo(name); }
					IFACEMETHODIMP put_Name(_In_ HSTRING name) override { return Name.Set(name); }

					// IGraphicsEffectD2D1Interop
					IFACEMETHODIMP GetSourceCount(_Out_ UINT* count) override { *count = 0; return S_OK; }
					IFACEMETHODIMP GetPropertyCount(_Out_ UINT* count) override { *count = 0; return S_OK; }

					IFACEMETHODIMP GetSource(UINT, _Outptr_result_maybenull_ IGraphicsEffectSource**) override
					{
						return E_INVALIDARG;
					}

					IFACEMETHODIMP GetProperty(UINT, _Outptr_ IPropertyValue**) override
					{
						return E_INVALIDARG;
					}

					IFACEMETHODIMP GetNamedPropertyMapping(_In_z_ LPCWSTR, _Out_ UINT*,
						_Out_ GRAPHICS_EFFECT_PROPERTY_MAPPING*) override
					{
						return E_INVALIDARG;
					}

				protected:
					// Invokes a functor with the pointer to the property factory
					template<typename TFunc>
					static HRESULT UsePropertyFactory(const TFunc& func)
					{
						Microsoft::WRL::ComPtr<IPropertyValueStatics> propertyValueFactory;
						Microsoft::WRL::Wrappers::HStringReference activatableClassId{ RuntimeClass_Windows_Foundation_PropertyValue };
						HRESULT hr = GetActivationFactory(activatableClassId.Get(), &propertyValueFactory);
						return FAILED(hr) ? hr : func(propertyValueFactory.Get());
					}

					template<UINT32 ComponentCount>
					static HRESULT CreateColor(
						_In_ IPropertyValueStatics* statics,
						UIColor color,
						_Outptr_ IPropertyValue ** value)
					{
						static_assert(ComponentCount == 3 || ComponentCount == 4, "Unexpected color component count.");
						float values[] = { color.R / 255.0f, color.G / 255.0f, color.B / 255.0f, color.A / 255.0f };
						Microsoft::WRL::ComPtr<IInspectable> valueInspectable;
						return statics->CreateSingleArray(ComponentCount, values, (IInspectable**)value);
					}

					// Make a bool not a compile-time constant to avoid compiler/OACR warnings
					static bool Passthrough(bool value) { return value; }

					// Helpers to implement GetNamedPropertyMapping more succintly
					struct NamedProperty
					{
						const wchar_t* Name; // Compile-time constant
						UINT Index; // Property index
						GRAPHICS_EFFECT_PROPERTY_MAPPING Mapping;
					};

					HRESULT GetNamedPropertyMappingImpl(
						_In_count_(namedPropertyCount) const NamedProperty* namedProperties,
						UINT namedPropertyCount,
						_In_z_ LPCWSTR name,
						_Out_ UINT* index,
						_Out_ GRAPHICS_EFFECT_PROPERTY_MAPPING* mapping)
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
					Microsoft::WRL::Wrappers::HString Name;
				};

				//-----------------------------------------------------------------------------------------------------------------
				// Helper macros to make implementation more succint

#pragma push_macro("DECLARE_D2D_GUID")
#undef DECLARE_D2D_GUID
#define DECLARE_D2D_GUID(Guid) \
    IFACEMETHODIMP GetEffectId(_Out_ GUID* id) override { *id = Guid; return S_OK; }

#pragma push_macro("DECLARE_POD_PROPERTY")
#undef DECLARE_POD_PROPERTY
#define DECLARE_POD_PROPERTY(Name, Type, InitialValue, Condition) \
    private: \
    Type Name = InitialValue; \
    public: \
    IFACEMETHODIMP get_##Name(_Out_ Type* value) override { *value = Name; return S_OK; } \
    IFACEMETHODIMP put_##Name(Type value) override \
    { \
        if (!Passthrough(Condition)) { return E_INVALIDARG; } \
        Name = value; \
        return S_OK; \
    }

#pragma push_macro("DECLARE_SOURCE")
#undef DECLARE_SOURCE
#define DECLARE_SOURCE(Name) \
    Microsoft::WRL::ComPtr<IGraphicsEffectSource> Name; \
    IFACEMETHODIMP get_##Name(_Outptr_result_maybenull_ IGraphicsEffectSource** value) override { return Name.CopyTo(value); } \
    IFACEMETHODIMP put_##Name(_In_ IGraphicsEffectSource* value) override { Name = value; return S_OK; }

#pragma push_macro("DECLARE_SINGLE_SOURCE")
#undef DECLARE_SINGLE_SOURCE
#define DECLARE_SINGLE_SOURCE(Name) \
    DECLARE_SOURCE(Name) \
    IFACEMETHODIMP GetSourceCount(_Out_ UINT* count) override { *count = 1; return S_OK; } \
    IFACEMETHODIMP GetSource(UINT index, _Outptr_result_maybenull_ IGraphicsEffectSource** source) override \
    { \
        return index == 0 ? Name.CopyTo(source) : E_INVALIDARG; \
    }

#pragma push_macro("DECLARE_DUAL_SOURCES")
#undef DECLARE_DUAL_SOURCES
#define DECLARE_DUAL_SOURCES(Name1, Name2) \
    DECLARE_SOURCE(Name1) \
    DECLARE_SOURCE(Name2) \
    IFACEMETHODIMP GetSourceCount(_Out_ UINT* count) override { *count = 2; return S_OK; } \
    IFACEMETHODIMP GetSource(UINT index, _Outptr_result_maybenull_ IGraphicsEffectSource** source) override \
    { \
        return index == 0 ? Name1.CopyTo(source) : index == 1 ? Name2.CopyTo(source) : E_INVALIDARG; \
    }

#pragma push_macro("DECLARE_NAMED_PROPERTY_MAPPING")
#undef DECLARE_NAMED_PROPERTY_MAPPING
#define DECLARE_NAMED_PROPERTY_MAPPING(...) \
    IFACEMETHODIMP GetNamedPropertyMapping(_In_z_ LPCWSTR name, _Out_ UINT* index, \
        _Out_ GRAPHICS_EFFECT_PROPERTY_MAPPING* mapping) override \
    { \
        static const NamedProperty s_Properties[] = { __VA_ARGS__ }; \
        return GetNamedPropertyMappingImpl(s_Properties, _countof(s_Properties), name, index, mapping); \
    }

				//-----------------------------------------------------------------------------------------------------------------

				//class AlphaMaskEffect WrlFinal : public EffectBase<IAlphaMaskEffect>
				//{
				//    InspectableClass(RuntimeClass_Microsoft_UI_Composition_Effects_AlphaMaskEffect, BaseTrust);

				//public:
				//    DECLARE_D2D_GUID(CLSID_D2D1AlphaMask);
				//    DECLARE_DUAL_SOURCES(Source, Mask);
				//};

				////-----------------------------------------------------------------------------------------------------------------

				//class ArithmeticCompositeEffect WrlFinal : public EffectBase<IArithmeticCompositeEffect>
				//{
				//    InspectableClass(RuntimeClass_Microsoft_UI_Composition_Effects_ArithmeticCompositeEffect, BaseTrust);

				//public:
				//    DECLARE_D2D_GUID(CLSID_D2D1ArithmeticComposite);
				//    DECLARE_DUAL_SOURCES(Source1, Source2);
				//    DECLARE_POD_PROPERTY(MultiplyAmount, float, 1.0f, true);
				//    DECLARE_POD_PROPERTY(Source1Amount, float, 0.0f, true);
				//    DECLARE_POD_PROPERTY(Source2Amount, float, 0.0f, true);
				//    DECLARE_POD_PROPERTY(Offset, float, 0.0f, true);
				//    DECLARE_POD_PROPERTY(ClampOutput, boolean, static_cast<boolean>(false), true);
				//    DECLARE_NAMED_PROPERTY_MAPPING(
				//        { L"MultiplyAmount", D2D1_ARITHMETICCOMPOSITE_PROP_COEFFICIENTS, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_VECTORX },
				//        { L"Source1Amount", D2D1_ARITHMETICCOMPOSITE_PROP_COEFFICIENTS, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_VECTORY },
				//        { L"Source2Amount", D2D1_ARITHMETICCOMPOSITE_PROP_COEFFICIENTS, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_VECTORZ },
				//        { L"Offset", D2D1_ARITHMETICCOMPOSITE_PROP_COEFFICIENTS, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_VECTORW },
				//        { L"ClampOutput", D2D1_ARITHMETICCOMPOSITE_PROP_CLAMP_OUTPUT, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT });

				//public:
				//    IFACEMETHODIMP GetPropertyCount(_Out_ UINT * count) override { *count = 2; return S_OK; }

				//    IFACEMETHODIMP GetProperty(UINT index, _Outptr_ IPropertyValue ** value) override
				//    {
				//        return UsePropertyFactory([=](IPropertyValueStatics* statics)
				//        {
				//            switch (index)
				//            {
				//                case D2D1_ARITHMETICCOMPOSITE_PROP_COEFFICIENTS:
				//                {
				//                    float coefficients[4] = { MultiplyAmount, Source1Amount, Source2Amount, Offset };
				//                    return statics->CreateSingleArray(4, coefficients, (IInspectable**)value);
				//                }
				//                case D2D1_ARITHMETICCOMPOSITE_PROP_CLAMP_OUTPUT:
				//                    return statics->CreateBoolean(ClampOutput, (IInspectable**)value);
				//                default:
				//                    return E_INVALIDARG;
				//            }
				//        });
				//    }
				//};

				//-----------------------------------------------------------------------------------------------------------------

				class BlendEffect WrlFinal : public EffectBase<IBlendEffect>
				{
					InspectableClass(L"RuntimeClass_Microsoft_UI_Composition_Effects_BlendEffect", BaseTrust);

				public:
					DECLARE_D2D_GUID(CLSID_D2D1Blend);
					DECLARE_DUAL_SOURCES(Background, Foreground);
					DECLARE_POD_PROPERTY(Mode, BlendEffectMode, BlendEffectMode_Multiply, true);
					DECLARE_NAMED_PROPERTY_MAPPING(
						{ L"Mode", D2D1_BLEND_PROP_MODE, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT });

				public:
					IFACEMETHODIMP GetPropertyCount(_Out_ UINT * count) override { *count = 1; return S_OK; }

					IFACEMETHODIMP GetProperty(UINT index, _Outptr_ IPropertyValue ** value) override
					{
						return UsePropertyFactory([=](IPropertyValueStatics* statics)
						{
							switch (index)
							{
							case D2D1_BLEND_PROP_MODE: return statics->CreateUInt32(Mode, (IInspectable**)value);
							default: return E_INVALIDARG;
							}
						});
					}
				};

				//-----------------------------------------------------------------------------------------------------------------

				//class BorderEffect WrlFinal : public EffectBase<IBorderEffect>
				//{
				//    InspectableClass(RuntimeClass_Microsoft_UI_Composition_Effects_BorderEffect, BaseTrust);

				//public:
				//    DECLARE_D2D_GUID(CLSID_D2D1Border);
				//    DECLARE_SINGLE_SOURCE(Source);
				//    DECLARE_POD_PROPERTY(ExtendX, CanvasEdgeBehavior, CanvasEdgeBehavior_Clamp, true);
				//    DECLARE_POD_PROPERTY(ExtendY, CanvasEdgeBehavior, CanvasEdgeBehavior_Clamp, true);
				//    DECLARE_NAMED_PROPERTY_MAPPING(
				//        { L"ExtendX", D2D1_BORDER_PROP_EDGE_MODE_X, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
				//        { L"ExtendY", D2D1_BORDER_PROP_EDGE_MODE_Y, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT });

				//public:
				//    IFACEMETHODIMP GetPropertyCount(_Out_ UINT * count) override { *count = 2; return S_OK; }

				//    IFACEMETHODIMP GetProperty(UINT index, _Outptr_ IPropertyValue ** value) override
				//    {
				//        return UsePropertyFactory([=](IPropertyValueStatics* statics)
				//        {
				//            switch (index)
				//            {
				//                case D2D1_BORDER_PROP_EDGE_MODE_X: return statics->CreateUInt32(ExtendX, (IInspectable**)value);
				//                case D2D1_BORDER_PROP_EDGE_MODE_Y: return statics->CreateUInt32(ExtendY, (IInspectable**)value);
				//                default: return E_INVALIDARG;
				//            }
				//        });
				//    }
				//};

				////-----------------------------------------------------------------------------------------------------------------

				//class ColorMatrixEffect WrlFinal : public EffectBase<IColorMatrixEffect>
				//{
				//    InspectableClass(RuntimeClass_Microsoft_UI_Composition_Effects_ColorMatrixEffect, BaseTrust);

				//public:
				//    DECLARE_D2D_GUID(CLSID_D2D1ColorMatrix);
				//    DECLARE_SINGLE_SOURCE(Source);
				//    DECLARE_POD_PROPERTY(ColorMatrix, Matrix5x4, (Matrix5x4{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0 }), true);
				//    DECLARE_POD_PROPERTY(AlphaMode, CanvasAlphaMode, CanvasAlphaMode_Premultiplied, value != CanvasAlphaMode_Ignore);
				//    DECLARE_POD_PROPERTY(ClampOutput, boolean, static_cast<boolean>(false), true);
				//    DECLARE_NAMED_PROPERTY_MAPPING(
				//        { L"ColorMatrix", D2D1_COLORMATRIX_PROP_COLOR_MATRIX, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
				//        { L"AlphaMode", D2D1_COLORMATRIX_PROP_ALPHA_MODE, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_COLORMATRIX_ALPHA_MODE },
				//        { L"ClampOutput", D2D1_COLORMATRIX_PROP_CLAMP_OUTPUT, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT });

				//public:
				//    IFACEMETHODIMP GetPropertyCount(_Out_ UINT * count) override { *count = 3; return S_OK; }

				//    IFACEMETHODIMP GetProperty(UINT index, _Outptr_ IPropertyValue ** value) override
				//    {
				//        return UsePropertyFactory([=](IPropertyValueStatics* statics)
				//        {
				//            switch (index)
				//            {
				//                case D2D1_COLORMATRIX_PROP_COLOR_MATRIX: return statics->CreateSingleArray(20, reinterpret_cast<const float*>(&ColorMatrix), (IInspectable**)value);
				//                case D2D1_COLORMATRIX_PROP_ALPHA_MODE:
				//                {
				//                    switch (AlphaMode)
				//                    {
				//                        case CanvasAlphaMode_Premultiplied: 
				//                            return statics->CreateUInt32(D2D1_COLORMANAGEMENT_ALPHA_MODE_PREMULTIPLIED, (IInspectable**)value);
				//                        case CanvasAlphaMode_Straight:
				//                            return statics->CreateUInt32(D2D1_COLORMANAGEMENT_ALPHA_MODE_STRAIGHT, (IInspectable**)value);
				//                    }
				//                    break;
				//                }
				//                case D2D1_COLORMATRIX_PROP_CLAMP_OUTPUT: return statics->CreateBoolean(ClampOutput, (IInspectable**)value);
				//            }
				//            return E_INVALIDARG;
				//        });
				//    }
				//};

				//-----------------------------------------------------------------------------------------------------------------

				class ColorSourceEffect WrlFinal : public EffectBase<IColorSourceEffect>
				{
					InspectableClass(L"RuntimeClass_Microsoft_UI_Composition_Effects_ColorSourceEffect", BaseTrust);

				public:
					DECLARE_D2D_GUID(CLSID_D2D1Flood);
					DECLARE_POD_PROPERTY(Color, UIColor, (UIColor{ 255, 0, 0, 0 }), true);
					DECLARE_NAMED_PROPERTY_MAPPING(
						{ L"Color", D2D1_FLOOD_PROP_COLOR, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_COLOR_TO_VECTOR4 });

				public:
					IFACEMETHODIMP GetPropertyCount(_Out_ UINT * count) override { *count = 1; return S_OK; }

					IFACEMETHODIMP GetProperty(UINT index, _Outptr_ IPropertyValue ** value) override
					{
						return UsePropertyFactory([=](IPropertyValueStatics* statics)
						{
							switch (index)
							{
							case D2D1_FLOOD_PROP_COLOR: return CreateColor<4>(statics, Color, value);
							default: return E_INVALIDARG;
							}
						});
					}
				};

				//-----------------------------------------------------------------------------------------------------------------
				// Win2D has CompositeEffect with an arbitrary number of sources,
				// but this involves having an IVector of sources and is more trouble than it's worth.
				// We declare a simplified single-step composite effect between two sources.

				class CompositeStepEffect WrlFinal : public EffectBase<ICompositeStepEffect>
				{
					InspectableClass(L"RuntimeClass_Microsoft_UI_Composition_Effects_CompositeStepEffect", BaseTrust);

				public:
					DECLARE_D2D_GUID(CLSID_D2D1Composite);
					DECLARE_DUAL_SOURCES(Destination, Source);
					DECLARE_POD_PROPERTY(Mode, CanvasComposite, CanvasComposite_SourceOver, true);
					DECLARE_NAMED_PROPERTY_MAPPING(
						{ L"Mode", D2D1_COMPOSITE_PROP_MODE, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT });

				public:
					IFACEMETHODIMP GetPropertyCount(_Out_ UINT * count) override { *count = 1; return S_OK; }

					IFACEMETHODIMP GetProperty(UINT index, _Outptr_ IPropertyValue ** value) override
					{
						return UsePropertyFactory([=](IPropertyValueStatics* statics)
						{
							switch (index)
							{
							case D2D1_COMPOSITE_PROP_MODE: return statics->CreateUInt32(Mode, (IInspectable**)value);
							default: return E_INVALIDARG;
							}
						});
					}
				};

				////-----------------------------------------------------------------------------------------------------------------

				//class ContrastEffect WrlFinal : public EffectBase<IContrastEffect>
				//{
				//    InspectableClass(RuntimeClass_Microsoft_UI_Composition_Effects_ContrastEffect, BaseTrust);

				//public:
				//    DECLARE_D2D_GUID(CLSID_D2D1Contrast);
				//    DECLARE_SINGLE_SOURCE(Source);
				//    DECLARE_POD_PROPERTY(Contrast, float, 0.0f, value >= -1.0f && value <= 1.0f);
				//    DECLARE_POD_PROPERTY(ClampSource, boolean, static_cast<boolean>(false), true);
				//    DECLARE_NAMED_PROPERTY_MAPPING(
				//        { L"Contrast", D2D1_CONTRAST_PROP_CONTRAST, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
				//        { L"ClampSource", D2D1_CONTRAST_PROP_CLAMP_INPUT, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT });

				//public:
				//    IFACEMETHODIMP GetPropertyCount(_Out_ UINT * count) override { *count = 2; return S_OK; }

				//    IFACEMETHODIMP GetProperty(UINT index, _Outptr_ IPropertyValue ** value) override
				//    {
				//        return UsePropertyFactory([=](IPropertyValueStatics* statics)
				//        {
				//            switch (index)
				//            {
				//                case D2D1_CONTRAST_PROP_CONTRAST: return statics->CreateSingle(Contrast, (IInspectable**)value);
				//                case D2D1_CONTRAST_PROP_CLAMP_INPUT: return statics->CreateBoolean(ClampSource, (IInspectable**)value);
				//                default: return E_INVALIDARG;
				//            }
				//        });
				//    }
				//};

				////-----------------------------------------------------------------------------------------------------------------

				//class CrossFadeEffect WrlFinal : public EffectBase<ICrossFadeEffect>
				//{
				//    InspectableClass(RuntimeClass_Microsoft_UI_Composition_Effects_CrossFadeEffect, BaseTrust);

				//public:
				//    DECLARE_D2D_GUID(CLSID_D2D1CrossFade);
				//    DECLARE_DUAL_SOURCES(Source1, Source2);
				//    DECLARE_POD_PROPERTY(Weight, float, 0.5f, value >= 0.0f && value <= 1.0f);
				//    DECLARE_NAMED_PROPERTY_MAPPING(
				//        { L"Weight", D2D1_CROSSFADE_PROP_WEIGHT, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT });

				//public:
				//    IFACEMETHODIMP GetPropertyCount(_Out_ UINT * count) override { *count = 1; return S_OK; }

				//    IFACEMETHODIMP GetProperty(UINT index, _Outptr_ IPropertyValue ** value) override
				//    {
				//        return UsePropertyFactory([=](IPropertyValueStatics* statics)
				//        {
				//            switch (index)
				//            {
				//                case D2D1_CROSSFADE_PROP_WEIGHT: return statics->CreateSingle(Weight, (IInspectable**)value);
				//                default: return E_INVALIDARG;
				//            }
				//        });
				//    }
				//};

				////-----------------------------------------------------------------------------------------------------------------

				//class DistantDiffuseEffect WrlFinal : public EffectBase<IDistantDiffuseEffect>
				//{
				//    InspectableClass(RuntimeClass_Microsoft_UI_Composition_Effects_DistantDiffuseEffect, BaseTrust);

				//public:
				//    DECLARE_D2D_GUID(CLSID_D2D1DistantDiffuse);
				//    DECLARE_SINGLE_SOURCE(Source);
				//    DECLARE_POD_PROPERTY(Azimuth, float, 0.0f, true); // D2D clamps within [0, 360] degrees
				//    DECLARE_POD_PROPERTY(Elevation, float, 0.0f, true); // D2D clamps within [0, 360] degrees
				//    DECLARE_POD_PROPERTY(DiffuseAmount, float, 1.0f, value >= 0.0f && value <= 10000.0f);
				//    DECLARE_POD_PROPERTY(HeightMapScale, float, 1.0f, value >= -10000.0f && value <= 10000.0f);
				//    DECLARE_POD_PROPERTY(LightColor, UIColor, (UIColor{ 255, 255, 255, 255 }), true);
				//    DECLARE_POD_PROPERTY(HeightMapKernelSize, Vector2, (Vector2{ 1.0f, 1.0f }),
				//        value.X >= 0.01f && value.Y >= 0.01f && value.X <= 100.0f && value.Y <= 100.0f);
				//    DECLARE_POD_PROPERTY(HeightMapInterpolationMode, CanvasImageInterpolation, CanvasImageInterpolation_Linear, true);
				//    DECLARE_NAMED_PROPERTY_MAPPING(
				//        { L"Azimuth", D2D1_DISTANTDIFFUSE_PROP_AZIMUTH, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_RADIANS_TO_DEGREES },
				//        { L"Elevation", D2D1_DISTANTDIFFUSE_PROP_ELEVATION, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_RADIANS_TO_DEGREES },
				//        { L"DiffuseAmount", D2D1_DISTANTDIFFUSE_PROP_DIFFUSE_CONSTANT, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
				//        { L"HeightMapScale", D2D1_DISTANTDIFFUSE_PROP_SURFACE_SCALE, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
				//        { L"LightColor", D2D1_DISTANTDIFFUSE_PROP_COLOR, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_COLOR_TO_VECTOR3 },
				//        { L"HeightMapKernelSize", D2D1_DISTANTDIFFUSE_PROP_KERNEL_UNIT_LENGTH, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
				//        { L"HeightMapInterpolationMode", D2D1_DISTANTDIFFUSE_PROP_SCALE_MODE, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT }, );

				//public:
				//    IFACEMETHODIMP GetPropertyCount(_Out_ UINT * count) override { *count = 7; return S_OK; }

				//    IFACEMETHODIMP GetProperty(UINT index, _Outptr_ IPropertyValue ** value) override
				//    {
				//        return UsePropertyFactory([=](IPropertyValueStatics* statics)
				//        {
				//            switch (index)
				//            {
				//                case D2D1_DISTANTDIFFUSE_PROP_AZIMUTH: return statics->CreateSingle(Azimuth * k_DegreesPerRadian, (IInspectable**)value);
				//                case D2D1_DISTANTDIFFUSE_PROP_ELEVATION: return statics->CreateSingle(Elevation * k_DegreesPerRadian, (IInspectable**)value);
				//                case D2D1_DISTANTDIFFUSE_PROP_DIFFUSE_CONSTANT: return statics->CreateSingle(DiffuseAmount, (IInspectable**)value);
				//                case D2D1_DISTANTDIFFUSE_PROP_SURFACE_SCALE: return statics->CreateSingle(HeightMapScale, (IInspectable**)value);
				//                case D2D1_DISTANTDIFFUSE_PROP_COLOR: return CreateColor<3>(statics, LightColor, value);
				//                case D2D1_DISTANTDIFFUSE_PROP_KERNEL_UNIT_LENGTH:
				//                    return statics->CreateSingleArray(2, reinterpret_cast<const float*>(&HeightMapKernelSize), (IInspectable**)value);
				//                case D2D1_DISTANTDIFFUSE_PROP_SCALE_MODE: return statics->CreateUInt32(HeightMapInterpolationMode, (IInspectable**)value);
				//                default: return E_INVALIDARG;
				//            }
				//        });
				//    }
				//};

				////-----------------------------------------------------------------------------------------------------------------

				//class DistantSpecularEffect WrlFinal : public EffectBase<IDistantSpecularEffect>
				//{
				//    InspectableClass(RuntimeClass_Microsoft_UI_Composition_Effects_DistantSpecularEffect, BaseTrust);

				//public:
				//    DECLARE_D2D_GUID(CLSID_D2D1DistantSpecular);
				//    DECLARE_SINGLE_SOURCE(Source);
				//    DECLARE_POD_PROPERTY(Azimuth, float, 0.0f, true); // D2D clamps within [0, 360] degrees
				//    DECLARE_POD_PROPERTY(Elevation, float, 0.0f, true); // D2D clamps within [0, 360] degrees
				//    DECLARE_POD_PROPERTY(SpecularExponent, float, 1.0f, value >= -10000.0f && value <= 10000.0f);
				//    DECLARE_POD_PROPERTY(SpecularAmount, float, 1.0f, value >= 0.0f && value <= 10000.0f);
				//    DECLARE_POD_PROPERTY(HeightMapScale, float, 1.0f, value >= -10000.0f && value <= 10000.0f);
				//    DECLARE_POD_PROPERTY(LightColor, UIColor, (UIColor{ 255, 255, 255, 255 }), true);
				//    DECLARE_POD_PROPERTY(HeightMapKernelSize, Vector2, (Vector2{ 1.0f, 1.0f }),
				//        value.X >= 0.01f && value.Y >= 0.01f && value.X <= 100.0f && value.Y <= 100.0f);
				//    DECLARE_POD_PROPERTY(HeightMapInterpolationMode, CanvasImageInterpolation, CanvasImageInterpolation_Linear, true);
				//    DECLARE_NAMED_PROPERTY_MAPPING(
				//        { L"Azimuth", D2D1_DISTANTSPECULAR_PROP_AZIMUTH, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_RADIANS_TO_DEGREES },
				//        { L"Elevation", D2D1_DISTANTSPECULAR_PROP_ELEVATION, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_RADIANS_TO_DEGREES },
				//        { L"SpecularExponent", D2D1_DISTANTSPECULAR_PROP_SPECULAR_EXPONENT, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
				//        { L"SpecularAmount", D2D1_DISTANTSPECULAR_PROP_SPECULAR_CONSTANT, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
				//        { L"HeightMapScale", D2D1_DISTANTSPECULAR_PROP_SURFACE_SCALE, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
				//        { L"LightColor", D2D1_DISTANTSPECULAR_PROP_COLOR, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_COLOR_TO_VECTOR3 },
				//        { L"HeightMapKernelSize", D2D1_DISTANTSPECULAR_PROP_KERNEL_UNIT_LENGTH, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
				//        { L"HeightMapInterpolationMode", D2D1_DISTANTSPECULAR_PROP_SCALE_MODE, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },);

				//public:
				//    IFACEMETHODIMP GetPropertyCount(_Out_ UINT * count) override { *count = 8; return S_OK; }

				//    IFACEMETHODIMP GetProperty(UINT index, _Outptr_ IPropertyValue ** value) override
				//    {
				//        return UsePropertyFactory([=](IPropertyValueStatics* statics)
				//        {
				//            switch (index)
				//            {
				//                case D2D1_DISTANTSPECULAR_PROP_AZIMUTH: return statics->CreateSingle(Azimuth * k_DegreesPerRadian, (IInspectable**)value);
				//                case D2D1_DISTANTSPECULAR_PROP_ELEVATION: return statics->CreateSingle(Elevation * k_DegreesPerRadian, (IInspectable**)value);
				//                case D2D1_DISTANTSPECULAR_PROP_SPECULAR_EXPONENT: return statics->CreateSingle(SpecularExponent, (IInspectable**)value);
				//                case D2D1_DISTANTSPECULAR_PROP_SPECULAR_CONSTANT: return statics->CreateSingle(SpecularAmount, (IInspectable**)value);
				//                case D2D1_DISTANTSPECULAR_PROP_SURFACE_SCALE: return statics->CreateSingle(HeightMapScale, (IInspectable**)value);
				//                case D2D1_DISTANTSPECULAR_PROP_COLOR: return CreateColor<3>(statics, LightColor, value);
				//                case D2D1_DISTANTSPECULAR_PROP_KERNEL_UNIT_LENGTH:
				//                    return statics->CreateSingleArray(2, reinterpret_cast<const float*>(&HeightMapKernelSize), (IInspectable**)value);
				//                case D2D1_DISTANTSPECULAR_PROP_SCALE_MODE: return statics->CreateUInt32(HeightMapInterpolationMode, (IInspectable**)value);
				//                default: return E_INVALIDARG;
				//            }
				//        });
				//    }
				//};

				////-----------------------------------------------------------------------------------------------------------------

				//class ExposureEffect WrlFinal : public EffectBase<IExposureEffect>
				//{
				//    InspectableClass(RuntimeClass_Microsoft_UI_Composition_Effects_ExposureEffect, BaseTrust);

				//public:
				//    DECLARE_D2D_GUID(CLSID_D2D1Exposure);
				//    DECLARE_SINGLE_SOURCE(Source);
				//    DECLARE_POD_PROPERTY(Exposure, float, 0.0f, value >= -2.0f && value <= 2.0f);
				//    DECLARE_NAMED_PROPERTY_MAPPING(
				//        { L"Exposure", D2D1_EXPOSURE_PROP_EXPOSURE_VALUE, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT });

				//public:
				//    IFACEMETHODIMP GetPropertyCount(_Out_ UINT * count) override { *count = 1; return S_OK; }

				//    IFACEMETHODIMP GetProperty(UINT index, _Outptr_ IPropertyValue ** value) override
				//    {
				//        return UsePropertyFactory([=](IPropertyValueStatics* statics)
				//        {
				//            switch (index)
				//            {
				//                case D2D1_EXPOSURE_PROP_EXPOSURE_VALUE: return statics->CreateSingle(Exposure, (IInspectable**)value);
				//                default: return E_INVALIDARG;
				//            }
				//        });
				//    }
				//};

				////-----------------------------------------------------------------------------------------------------------------

				//class GammaTransferEffect WrlFinal : public EffectBase<IGammaTransferEffect>
				//{
				//    InspectableClass(RuntimeClass_Microsoft_UI_Composition_Effects_GammaTransferEffect, BaseTrust);

				//public:
				//    DECLARE_D2D_GUID(CLSID_D2D1GammaTransfer);
				//    DECLARE_SINGLE_SOURCE(Source);
				//    DECLARE_POD_PROPERTY(RedAmplitude, float, 1.0f, true);
				//    DECLARE_POD_PROPERTY(RedExponent, float, 1.0f, true);
				//    DECLARE_POD_PROPERTY(RedOffset, float, 0.0f, true);
				//    DECLARE_POD_PROPERTY(RedDisable, boolean, static_cast<boolean>(false), true);
				//    DECLARE_POD_PROPERTY(GreenAmplitude, float, 1.0f, true);
				//    DECLARE_POD_PROPERTY(GreenExponent, float, 1.0f, true);
				//    DECLARE_POD_PROPERTY(GreenOffset, float, 0.0f, true);
				//    DECLARE_POD_PROPERTY(GreenDisable, boolean, static_cast<boolean>(false), true);
				//    DECLARE_POD_PROPERTY(BlueAmplitude, float, 1.0f, true);
				//    DECLARE_POD_PROPERTY(BlueExponent, float, 1.0f, true);
				//    DECLARE_POD_PROPERTY(BlueOffset, float, 0.0f, true);
				//    DECLARE_POD_PROPERTY(BlueDisable, boolean, static_cast<boolean>(false), true);
				//    DECLARE_POD_PROPERTY(AlphaAmplitude, float, 1.0f, true);
				//    DECLARE_POD_PROPERTY(AlphaExponent, float, 1.0f, true);
				//    DECLARE_POD_PROPERTY(AlphaOffset, float, 0.0f, true);
				//    DECLARE_POD_PROPERTY(AlphaDisable, boolean, static_cast<boolean>(false), true);
				//    DECLARE_POD_PROPERTY(ClampOutput, boolean, static_cast<boolean>(false), true);
				//    DECLARE_NAMED_PROPERTY_MAPPING(
				//        { L"RedAmplitude", D2D1_GAMMATRANSFER_PROP_RED_AMPLITUDE, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
				//        { L"RedExponent", D2D1_GAMMATRANSFER_PROP_RED_EXPONENT, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
				//        { L"RedOffset", D2D1_GAMMATRANSFER_PROP_RED_OFFSET, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
				//        { L"RedDisable", D2D1_GAMMATRANSFER_PROP_RED_DISABLE, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
				//        { L"GreenAmplitude", D2D1_GAMMATRANSFER_PROP_GREEN_AMPLITUDE, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
				//        { L"GreenExponent", D2D1_GAMMATRANSFER_PROP_GREEN_EXPONENT, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
				//        { L"GreenOffset", D2D1_GAMMATRANSFER_PROP_GREEN_OFFSET, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
				//        { L"GreenDisable", D2D1_GAMMATRANSFER_PROP_GREEN_DISABLE, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
				//        { L"BlueAmplitude", D2D1_GAMMATRANSFER_PROP_BLUE_AMPLITUDE, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
				//        { L"BlueExponent", D2D1_GAMMATRANSFER_PROP_BLUE_EXPONENT, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
				//        { L"BlueOffset", D2D1_GAMMATRANSFER_PROP_BLUE_OFFSET, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
				//        { L"BlueDisable", D2D1_GAMMATRANSFER_PROP_BLUE_DISABLE, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
				//        { L"AlphaAmplitude", D2D1_GAMMATRANSFER_PROP_ALPHA_AMPLITUDE, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
				//        { L"AlphaExponent", D2D1_GAMMATRANSFER_PROP_ALPHA_EXPONENT, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
				//        { L"AlphaOffset", D2D1_GAMMATRANSFER_PROP_ALPHA_OFFSET, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
				//        { L"AlphaDisable", D2D1_GAMMATRANSFER_PROP_ALPHA_DISABLE, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
				//        { L"ClampOutput", D2D1_GAMMATRANSFER_PROP_CLAMP_OUTPUT, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT });

				//public:
				//    IFACEMETHODIMP GetPropertyCount(_Out_ UINT * count) override { *count = 17; return S_OK; }

				//    IFACEMETHODIMP GetProperty(UINT index, _Outptr_ IPropertyValue ** value) override
				//    {
				//        return UsePropertyFactory([=](IPropertyValueStatics* statics)
				//        {
				//            switch (index)
				//            {
				//                case D2D1_GAMMATRANSFER_PROP_RED_AMPLITUDE: return statics->CreateSingle(RedAmplitude, (IInspectable**)value);
				//                case D2D1_GAMMATRANSFER_PROP_RED_EXPONENT: return statics->CreateSingle(RedExponent, (IInspectable**)value);
				//                case D2D1_GAMMATRANSFER_PROP_RED_OFFSET: return statics->CreateSingle(RedOffset, (IInspectable**)value);
				//                case D2D1_GAMMATRANSFER_PROP_RED_DISABLE: return statics->CreateBoolean(RedDisable, (IInspectable**)value);
				//                case D2D1_GAMMATRANSFER_PROP_GREEN_AMPLITUDE: return statics->CreateSingle(GreenAmplitude, (IInspectable**)value);
				//                case D2D1_GAMMATRANSFER_PROP_GREEN_EXPONENT: return statics->CreateSingle(GreenExponent, (IInspectable**)value);
				//                case D2D1_GAMMATRANSFER_PROP_GREEN_OFFSET: return statics->CreateSingle(GreenOffset, (IInspectable**)value);
				//                case D2D1_GAMMATRANSFER_PROP_GREEN_DISABLE: return statics->CreateBoolean(GreenDisable, (IInspectable**)value);
				//                case D2D1_GAMMATRANSFER_PROP_BLUE_AMPLITUDE: return statics->CreateSingle(BlueAmplitude, (IInspectable**)value);
				//                case D2D1_GAMMATRANSFER_PROP_BLUE_EXPONENT: return statics->CreateSingle(BlueExponent, (IInspectable**)value);
				//                case D2D1_GAMMATRANSFER_PROP_BLUE_OFFSET: return statics->CreateSingle(BlueOffset, (IInspectable**)value);
				//                case D2D1_GAMMATRANSFER_PROP_BLUE_DISABLE: return statics->CreateBoolean(BlueDisable, (IInspectable**)value);
				//                case D2D1_GAMMATRANSFER_PROP_ALPHA_AMPLITUDE: return statics->CreateSingle(AlphaAmplitude, (IInspectable**)value);
				//                case D2D1_GAMMATRANSFER_PROP_ALPHA_EXPONENT: return statics->CreateSingle(AlphaExponent, (IInspectable**)value);
				//                case D2D1_GAMMATRANSFER_PROP_ALPHA_OFFSET: return statics->CreateSingle(AlphaOffset, (IInspectable**)value);
				//                case D2D1_GAMMATRANSFER_PROP_ALPHA_DISABLE: return statics->CreateBoolean(AlphaDisable, (IInspectable**)value);
				//                case D2D1_GAMMATRANSFER_PROP_CLAMP_OUTPUT: return statics->CreateBoolean(ClampOutput, (IInspectable**)value);
				//                default: return E_INVALIDARG;
				//            }
				//        });
				//    }
				//};

				//-----------------------------------------------------------------------------------------------------------------

				class GaussianBlurEffect WrlFinal : public EffectBase<IGaussianBlurEffect>
				{
					InspectableClass(L"RuntimeClass_Microsoft_UI_Composition_Effects_GaussianBlurEffect", BaseTrust);

				public:
					DECLARE_D2D_GUID(CLSID_D2D1GaussianBlur);
					DECLARE_SINGLE_SOURCE(Source);
					DECLARE_POD_PROPERTY(BlurAmount, float, 3.0f, value >= 0.0f && value <= 250.0f);
					DECLARE_POD_PROPERTY(Optimization, EffectOptimization, EffectOptimization_Balanced, true);
					DECLARE_POD_PROPERTY(BorderMode, EffectBorderMode, EffectBorderMode_Soft, true);
					DECLARE_NAMED_PROPERTY_MAPPING(
			{ L"BlurAmount", D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
			{ L"Optimization", D2D1_GAUSSIANBLUR_PROP_OPTIMIZATION, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
						{ L"BorderMode", D2D1_GAUSSIANBLUR_PROP_BORDER_MODE, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT });

				public:
					IFACEMETHODIMP GetPropertyCount(_Out_ UINT * count) override { *count = 3; return S_OK; }

					IFACEMETHODIMP GetProperty(UINT index, _Outptr_ IPropertyValue ** value) override
					{
						return UsePropertyFactory([=](IPropertyValueStatics* statics)
						{
							switch (index)
							{
							case D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION: return statics->CreateSingle(BlurAmount, (IInspectable**)value);
							case D2D1_GAUSSIANBLUR_PROP_OPTIMIZATION: return statics->CreateUInt32(Optimization, (IInspectable**)value);
							case D2D1_GAUSSIANBLUR_PROP_BORDER_MODE: return statics->CreateUInt32(BorderMode, (IInspectable**)value);
							default: return E_INVALIDARG;
							}
						});
					}
				};

				//-----------------------------------------------------------------------------------------------------------------

				//class GrayscaleEffect WrlFinal : public EffectBase<IGrayscaleEffect>
				//{
				//    InspectableClass(RuntimeClass_Microsoft_UI_Composition_Effects_GrayscaleEffect, BaseTrust);

				//public:
				//    DECLARE_D2D_GUID(CLSID_D2D1Grayscale);
				//    DECLARE_SINGLE_SOURCE(Source);
				//};

				////-----------------------------------------------------------------------------------------------------------------

				//class HueRotationEffect WrlFinal : public EffectBase<IHueRotationEffect>
				//{
				//    InspectableClass(RuntimeClass_Microsoft_UI_Composition_Effects_HueRotationEffect, BaseTrust);

				//public:
				//    DECLARE_D2D_GUID(CLSID_D2D1HueRotation);
				//    DECLARE_SINGLE_SOURCE(Source);
				//    DECLARE_POD_PROPERTY(Angle, float, 0.0f, true);  // D2D clamps within [0, 360] degrees
				//    DECLARE_NAMED_PROPERTY_MAPPING(
				//        { L"Angle", D2D1_HUEROTATION_PROP_ANGLE, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_RADIANS_TO_DEGREES });

				//public:
				//    IFACEMETHODIMP GetPropertyCount(_Out_ UINT * count) override { *count = 1; return S_OK; }

				//    IFACEMETHODIMP GetProperty(UINT index, _Outptr_ IPropertyValue ** value) override
				//    {
				//        return UsePropertyFactory([=](IPropertyValueStatics* statics)
				//        {
				//            switch (index)
				//            {
				//                case D2D1_HUEROTATION_PROP_ANGLE:
				//                    return statics->CreateSingle(Angle * k_DegreesPerRadian, (IInspectable**)value);
				//                default: return E_INVALIDARG;
				//            }
				//        });
				//    }
				//};

				////-----------------------------------------------------------------------------------------------------------------

				//class InvertEffect WrlFinal : public EffectBase<IInvertEffect>
				//{
				//    InspectableClass(RuntimeClass_Microsoft_UI_Composition_Effects_InvertEffect, BaseTrust);

				//public:
				//    DECLARE_D2D_GUID(CLSID_D2D1Invert);
				//    DECLARE_SINGLE_SOURCE(Source);
				//};

				////-----------------------------------------------------------------------------------------------------------------

				//class LinearTransferEffect WrlFinal : public EffectBase<ILinearTransferEffect>
				//{
				//    InspectableClass(RuntimeClass_Microsoft_UI_Composition_Effects_LinearTransferEffect, BaseTrust);

				//public:
				//    DECLARE_D2D_GUID(CLSID_D2D1LinearTransfer);
				//    DECLARE_SINGLE_SOURCE(Source);
				//    DECLARE_POD_PROPERTY(RedOffset, float, 0.0f, true);
				//    DECLARE_POD_PROPERTY(RedSlope, float, 1.0f, true);
				//    DECLARE_POD_PROPERTY(RedDisable, boolean, static_cast<boolean>(false), true);
				//    DECLARE_POD_PROPERTY(GreenOffset, float, 0.0f, true);
				//    DECLARE_POD_PROPERTY(GreenSlope, float, 1.0f, true);
				//    DECLARE_POD_PROPERTY(GreenDisable, boolean, static_cast<boolean>(false), true);
				//    DECLARE_POD_PROPERTY(BlueOffset, float, 0.0f, true);
				//    DECLARE_POD_PROPERTY(BlueSlope, float, 1.0f, true);
				//    DECLARE_POD_PROPERTY(BlueDisable, boolean, static_cast<boolean>(false), true);
				//    DECLARE_POD_PROPERTY(AlphaOffset, float, 0.0f, true);
				//    DECLARE_POD_PROPERTY(AlphaSlope, float, 1.0f, true);
				//    DECLARE_POD_PROPERTY(AlphaDisable, boolean, static_cast<boolean>(false), true);
				//    DECLARE_POD_PROPERTY(ClampOutput, boolean, static_cast<boolean>(false), true);
				//    DECLARE_NAMED_PROPERTY_MAPPING(
				//        { L"RedOffset", D2D1_LINEARTRANSFER_PROP_RED_Y_INTERCEPT, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
				//        { L"RedSlope", D2D1_LINEARTRANSFER_PROP_RED_SLOPE, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
				//        { L"RedDisable", D2D1_LINEARTRANSFER_PROP_RED_DISABLE, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
				//        { L"GreenOffset", D2D1_LINEARTRANSFER_PROP_GREEN_Y_INTERCEPT, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
				//        { L"GreenSlope", D2D1_LINEARTRANSFER_PROP_GREEN_SLOPE, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
				//        { L"GreenDisable", D2D1_LINEARTRANSFER_PROP_GREEN_DISABLE, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
				//        { L"BlueOffset", D2D1_LINEARTRANSFER_PROP_BLUE_Y_INTERCEPT, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
				//        { L"BlueSlope", D2D1_LINEARTRANSFER_PROP_BLUE_SLOPE, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
				//        { L"BlueDisable", D2D1_LINEARTRANSFER_PROP_BLUE_DISABLE, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
				//        { L"AlphaOffset", D2D1_LINEARTRANSFER_PROP_ALPHA_Y_INTERCEPT, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
				//        { L"AlphaSlope", D2D1_LINEARTRANSFER_PROP_ALPHA_SLOPE, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
				//        { L"AlphaDisable", D2D1_LINEARTRANSFER_PROP_ALPHA_DISABLE, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
				//        { L"ClampOutput", D2D1_LINEARTRANSFER_PROP_CLAMP_OUTPUT, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT } );

				//public:
				//    IFACEMETHODIMP GetPropertyCount(_Out_ UINT * count) override { *count = 13; return S_OK; }

				//    IFACEMETHODIMP GetProperty(UINT index, _Outptr_ IPropertyValue ** value) override
				//    {
				//        return UsePropertyFactory([=](IPropertyValueStatics* statics)
				//        {
				//            switch (index)
				//            {
				//                case D2D1_LINEARTRANSFER_PROP_RED_Y_INTERCEPT: return statics->CreateSingle(RedOffset, (IInspectable**)value);
				//                case D2D1_LINEARTRANSFER_PROP_RED_SLOPE: return statics->CreateSingle(RedSlope, (IInspectable**)value);
				//                case D2D1_LINEARTRANSFER_PROP_RED_DISABLE: return statics->CreateBoolean(RedDisable, (IInspectable**)value);
				//                case D2D1_LINEARTRANSFER_PROP_GREEN_Y_INTERCEPT: return statics->CreateSingle(GreenOffset, (IInspectable**)value);
				//                case D2D1_LINEARTRANSFER_PROP_GREEN_SLOPE: return statics->CreateSingle(GreenSlope, (IInspectable**)value);
				//                case D2D1_LINEARTRANSFER_PROP_GREEN_DISABLE: return statics->CreateBoolean(GreenDisable, (IInspectable**)value);
				//                case D2D1_LINEARTRANSFER_PROP_BLUE_Y_INTERCEPT: return statics->CreateSingle(BlueOffset, (IInspectable**)value);
				//                case D2D1_LINEARTRANSFER_PROP_BLUE_SLOPE: return statics->CreateSingle(BlueSlope, (IInspectable**)value);
				//                case D2D1_LINEARTRANSFER_PROP_BLUE_DISABLE: return statics->CreateBoolean(BlueDisable, (IInspectable**)value);
				//                case D2D1_LINEARTRANSFER_PROP_ALPHA_Y_INTERCEPT: return statics->CreateSingle(AlphaOffset, (IInspectable**)value);
				//                case D2D1_LINEARTRANSFER_PROP_ALPHA_SLOPE: return statics->CreateSingle(AlphaSlope, (IInspectable**)value);
				//                case D2D1_LINEARTRANSFER_PROP_ALPHA_DISABLE: return statics->CreateBoolean(AlphaDisable, (IInspectable**)value);
				//                case D2D1_LINEARTRANSFER_PROP_CLAMP_OUTPUT: return statics->CreateBoolean(ClampOutput, (IInspectable**)value);
				//                default: return E_INVALIDARG;
				//            }
				//        });
				//    }
				//};

				////-----------------------------------------------------------------------------------------------------------------

				//class LuminanceToAlphaEffect WrlFinal : public EffectBase<ILuminanceToAlphaEffect>
				//{
				//    InspectableClass(RuntimeClass_Microsoft_UI_Composition_Effects_LuminanceToAlphaEffect, BaseTrust);

				//public:
				//    DECLARE_D2D_GUID(CLSID_D2D1LuminanceToAlpha);
				//    DECLARE_SINGLE_SOURCE(Source);
				//};

				//-----------------------------------------------------------------------------------------------------------------

				class OpacityEffect WrlFinal : public EffectBase<IOpacityEffect>
				{
					InspectableClass(L"RuntimeClass_Microsoft_UI_Composition_Effects_OpacityEffect", BaseTrust);

				public:
					DECLARE_D2D_GUID(CLSID_D2D1Opacity);
					DECLARE_SINGLE_SOURCE(Source);
					DECLARE_POD_PROPERTY(Opacity, float, 1.0f, value >= 0.0f && value <= 1.0f);
					DECLARE_NAMED_PROPERTY_MAPPING(
						{ L"Opacity", D2D1_OPACITY_PROP_OPACITY, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT });

				public:
					IFACEMETHODIMP GetPropertyCount(_Out_ UINT * count) override { *count = 1; return S_OK; }

					IFACEMETHODIMP GetProperty(UINT index, _Outptr_ IPropertyValue ** value) override
					{
						return UsePropertyFactory([=](IPropertyValueStatics* statics)
						{
							switch (index)
							{
							case D2D1_OPACITY_PROP_OPACITY: return statics->CreateSingle(Opacity, (IInspectable**)value);
							default: return E_INVALIDARG;
							}
						});
					}
				};

				//-----------------------------------------------------------------------------------------------------------------

				//class PointDiffuseEffect WrlFinal : public EffectBase<IPointDiffuseEffect>
				//{
				//    InspectableClass(RuntimeClass_Microsoft_UI_Composition_Effects_PointDiffuseEffect, BaseTrust);

				//public:
				//    DECLARE_D2D_GUID(CLSID_D2D1PointDiffuse);
				//    DECLARE_SINGLE_SOURCE(Source);
				//    DECLARE_POD_PROPERTY(LightPosition, Vector3, (Vector3{ 0.0f, 0.0f, 0.0f }), true);
				//    DECLARE_POD_PROPERTY(DiffuseAmount, float, 1.0f, value >= 0.0f && value <= 10000.0f);
				//    DECLARE_POD_PROPERTY(HeightMapScale, float, 1.0f, value >= -10000.0f && value <= 10000.0f);
				//    DECLARE_POD_PROPERTY(LightColor, UIColor, (UIColor{ 255, 255, 255, 255 }), true);
				//    DECLARE_POD_PROPERTY(HeightMapKernelSize, Vector2, (Vector2{ 1.0f, 1.0f }),
				//        value.X >= 0.01f && value.Y >= 0.01f && value.X <= 100.0f && value.Y <= 100.0f);
				//    DECLARE_POD_PROPERTY(HeightMapInterpolationMode, CanvasImageInterpolation, CanvasImageInterpolation_Linear, true);
				//    DECLARE_NAMED_PROPERTY_MAPPING(
				//        { L"LightPosition", D2D1_POINTDIFFUSE_PROP_LIGHT_POSITION, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
				//        { L"DiffuseAmount", D2D1_POINTDIFFUSE_PROP_DIFFUSE_CONSTANT, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
				//        { L"HeightMapScale", D2D1_POINTDIFFUSE_PROP_SURFACE_SCALE, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
				//        { L"LightColor", D2D1_POINTDIFFUSE_PROP_COLOR, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_COLOR_TO_VECTOR3 },
				//        { L"HeightMapKernelSize", D2D1_POINTDIFFUSE_PROP_KERNEL_UNIT_LENGTH, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
				//        { L"HeightMapInterpolationMode", D2D1_POINTDIFFUSE_PROP_SCALE_MODE, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },);

				//public:
				//    IFACEMETHODIMP GetPropertyCount(_Out_ UINT * count) override { *count = 6; return S_OK; }

				//    IFACEMETHODIMP GetProperty(UINT index, _Outptr_ IPropertyValue ** value) override
				//    {
				//        return UsePropertyFactory([=](IPropertyValueStatics* statics)
				//        {
				//            switch (index)
				//            {
				//                case D2D1_POINTDIFFUSE_PROP_LIGHT_POSITION:
				//                    return statics->CreateSingleArray(3, reinterpret_cast<const float*>(&LightPosition), (IInspectable**)value);
				//                case D2D1_POINTDIFFUSE_PROP_DIFFUSE_CONSTANT: return statics->CreateSingle(DiffuseAmount, (IInspectable**)value);
				//                case D2D1_POINTDIFFUSE_PROP_SURFACE_SCALE: return statics->CreateSingle(HeightMapScale, (IInspectable**)value);
				//                case D2D1_POINTDIFFUSE_PROP_COLOR: return CreateColor<3>(statics, LightColor, value);
				//                case D2D1_POINTDIFFUSE_PROP_KERNEL_UNIT_LENGTH:
				//                    return statics->CreateSingleArray(2, reinterpret_cast<const float*>(&HeightMapKernelSize), (IInspectable**)value);
				//                case D2D1_POINTDIFFUSE_PROP_SCALE_MODE: return statics->CreateUInt32(HeightMapInterpolationMode, (IInspectable**)value);
				//                default: return E_INVALIDARG;
				//            }
				//        });
				//    }
				//};

				////-----------------------------------------------------------------------------------------------------------------

				//class PointSpecularEffect WrlFinal : public EffectBase<IPointSpecularEffect>
				//{
				//    InspectableClass(RuntimeClass_Microsoft_UI_Composition_Effects_PointSpecularEffect, BaseTrust);

				//public:
				//    DECLARE_D2D_GUID(CLSID_D2D1PointSpecular);
				//    DECLARE_SINGLE_SOURCE(Source);
				//    DECLARE_POD_PROPERTY(LightPosition, Vector3, (Vector3{ 0.0f, 0.0f, 0.0f }), true);
				//    DECLARE_POD_PROPERTY(SpecularExponent, float, 1.0f, value >= -10000.0f && value <= 10000.0f);
				//    DECLARE_POD_PROPERTY(SpecularAmount, float, 1.0f, value >= 0.0f && value <= 10000.0f);
				//    DECLARE_POD_PROPERTY(HeightMapScale, float, 1.0f, value >= -10000.0f && value <= 10000.0f);
				//    DECLARE_POD_PROPERTY(LightColor, UIColor, (UIColor{ 255, 255, 255, 255 }), true);
				//    DECLARE_POD_PROPERTY(HeightMapKernelSize, Vector2, (Vector2{ 1.0f, 1.0f }),
				//        value.X >= 0.01f && value.Y >= 0.01f && value.X <= 100.0f && value.Y <= 100.0f);
				//    DECLARE_POD_PROPERTY(HeightMapInterpolationMode, CanvasImageInterpolation, CanvasImageInterpolation_Linear, true);
				//    DECLARE_NAMED_PROPERTY_MAPPING(
				//        { L"LightPosition", D2D1_POINTDIFFUSE_PROP_LIGHT_POSITION, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
				//        { L"SpecularExponent", D2D1_POINTSPECULAR_PROP_SPECULAR_EXPONENT, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
				//        { L"SpecularAmount", D2D1_POINTSPECULAR_PROP_SPECULAR_CONSTANT, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
				//        { L"HeightMapScale", D2D1_POINTSPECULAR_PROP_SURFACE_SCALE, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
				//        { L"LightColor", D2D1_POINTSPECULAR_PROP_COLOR, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_COLOR_TO_VECTOR3 },
				//        { L"HeightMapKernelSize", D2D1_POINTSPECULAR_PROP_KERNEL_UNIT_LENGTH, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
				//        { L"HeightMapInterpolationMode", D2D1_POINTSPECULAR_PROP_SCALE_MODE, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },);

				//public:
				//    IFACEMETHODIMP GetPropertyCount(_Out_ UINT * count) override { *count = 7; return S_OK; }

				//    IFACEMETHODIMP GetProperty(UINT index, _Outptr_ IPropertyValue ** value) override
				//    {
				//        return UsePropertyFactory([=](IPropertyValueStatics* statics)
				//        {
				//            switch (index)
				//            {
				//                case D2D1_POINTDIFFUSE_PROP_LIGHT_POSITION:
				//                    return statics->CreateSingleArray(3, reinterpret_cast<const float*>(&LightPosition), (IInspectable**)value);
				//                case D2D1_POINTSPECULAR_PROP_SPECULAR_EXPONENT: return statics->CreateSingle(SpecularExponent, (IInspectable**)value);
				//                case D2D1_POINTSPECULAR_PROP_SPECULAR_CONSTANT: return statics->CreateSingle(SpecularAmount, (IInspectable**)value);
				//                case D2D1_POINTSPECULAR_PROP_SURFACE_SCALE: return statics->CreateSingle(HeightMapScale, (IInspectable**)value);
				//                case D2D1_POINTSPECULAR_PROP_COLOR: return CreateColor<3>(statics, LightColor, value);
				//                case D2D1_POINTSPECULAR_PROP_KERNEL_UNIT_LENGTH:
				//                    return statics->CreateSingleArray(2, reinterpret_cast<const float*>(&HeightMapKernelSize), (IInspectable**)value);
				//                case D2D1_POINTSPECULAR_PROP_SCALE_MODE: return statics->CreateUInt32(HeightMapInterpolationMode, (IInspectable**)value);
				//                default: return E_INVALIDARG;
				//            }
				//        });
				//    }
				//};

				////-----------------------------------------------------------------------------------------------------------------

				//class PosterizeEffect WrlFinal : public EffectBase<IPosterizeEffect>
				//{
				//    InspectableClass(RuntimeClass_Microsoft_UI_Composition_Effects_PosterizeEffect, BaseTrust);

				//public:
				//    DECLARE_D2D_GUID(CLSID_D2D1Posterize);
				//    DECLARE_SINGLE_SOURCE(Source);
				//    DECLARE_POD_PROPERTY(RedValueCount, int, 4, value >= 2 && value <= 16);
				//    DECLARE_POD_PROPERTY(GreenValueCount, int, 4, value >= 2 && value <= 16);
				//    DECLARE_POD_PROPERTY(BlueValueCount, int, 4, value >= 2 && value <= 16);
				//    DECLARE_NAMED_PROPERTY_MAPPING(
				//        { L"RedValueCount", D2D1_POSTERIZE_PROP_RED_VALUE_COUNT, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
				//        { L"GreenValueCount", D2D1_POSTERIZE_PROP_GREEN_VALUE_COUNT, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
				//        { L"BlueValueCount", D2D1_POSTERIZE_PROP_BLUE_VALUE_COUNT, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT });

				//public:
				//    IFACEMETHODIMP GetPropertyCount(_Out_ UINT * count) override { *count = 1; return S_OK; }

				//    IFACEMETHODIMP GetProperty(UINT index, _Outptr_ IPropertyValue ** value) override
				//    {
				//        return UsePropertyFactory([=](IPropertyValueStatics* statics)
				//        {
				//            switch (index)
				//            {
				//                case D2D1_POSTERIZE_PROP_RED_VALUE_COUNT: return statics->CreateInt32(RedValueCount, (IInspectable**)value);
				//                case D2D1_POSTERIZE_PROP_GREEN_VALUE_COUNT: return statics->CreateInt32(GreenValueCount, (IInspectable**)value);
				//                case D2D1_POSTERIZE_PROP_BLUE_VALUE_COUNT: return statics->CreateInt32(BlueValueCount, (IInspectable**)value);
				//                default: return E_INVALIDARG;
				//            }
				//        });
				//    }
				//};

				////-----------------------------------------------------------------------------------------------------------------

				//class PremultiplyEffect WrlFinal : public EffectBase<IPremultiplyEffect>
				//{
				//    InspectableClass(RuntimeClass_Microsoft_UI_Composition_Effects_PremultiplyEffect, BaseTrust);

				//public:
				//    DECLARE_D2D_GUID(CLSID_D2D1Premultiply);
				//    DECLARE_SINGLE_SOURCE(Source);
				//};

				////-----------------------------------------------------------------------------------------------------------------

				//class SaturationEffect WrlFinal : public EffectBase<ISaturationEffect>
				//{
				//    InspectableClass(RuntimeClass_Microsoft_UI_Composition_Effects_SaturationEffect, BaseTrust);

				//public:
				//    DECLARE_D2D_GUID(CLSID_D2D1Saturation);
				//    DECLARE_SINGLE_SOURCE(Source);
				//    DECLARE_POD_PROPERTY(Saturation, float, 0.5f, value >= 0.0f && value <= 2.0f);
				//    DECLARE_NAMED_PROPERTY_MAPPING(
				//        { L"Saturation", D2D1_SATURATION_PROP_SATURATION, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT });

				//public:
				//    IFACEMETHODIMP GetPropertyCount(_Out_ UINT * count) override { *count = 1; return S_OK; }

				//    IFACEMETHODIMP GetProperty(UINT index, _Outptr_ IPropertyValue ** value) override
				//    {
				//        return UsePropertyFactory([=](IPropertyValueStatics* statics)
				//        {
				//            switch (index)
				//            {
				//                case D2D1_SATURATION_PROP_SATURATION: return statics->CreateSingle(Saturation, (IInspectable**)value);
				//                default: return E_INVALIDARG;
				//            }
				//        });
				//    }
				//};

				////-----------------------------------------------------------------------------------------------------------------

				//class SepiaEffect WrlFinal : public EffectBase<ISepiaEffect>
				//{
				//    InspectableClass(RuntimeClass_Microsoft_UI_Composition_Effects_SepiaEffect, BaseTrust);

				//public:
				//    DECLARE_D2D_GUID(CLSID_D2D1Sepia);
				//    DECLARE_SINGLE_SOURCE(Source);
				//    DECLARE_POD_PROPERTY(Intensity, float, 0.5f, value >= 0.0f && value <= 1.0f);
				//    DECLARE_POD_PROPERTY(AlphaMode, CanvasAlphaMode, CanvasAlphaMode_Premultiplied, value != CanvasAlphaMode_Ignore);
				//    DECLARE_NAMED_PROPERTY_MAPPING(
				//        { L"Intensity", D2D1_SEPIA_PROP_INTENSITY, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
				//        { L"AlphaMode", D2D1_SEPIA_PROP_ALPHA_MODE, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_COLORMATRIX_ALPHA_MODE });

				//public:
				//    IFACEMETHODIMP GetPropertyCount(_Out_ UINT * count) override { *count = 2; return S_OK; }

				//    IFACEMETHODIMP GetProperty(UINT index, _Outptr_ IPropertyValue ** value) override
				//    {
				//        return UsePropertyFactory([=](IPropertyValueStatics* statics)
				//        {
				//            switch (index)
				//            {
				//                case D2D1_SEPIA_PROP_INTENSITY: return statics->CreateSingle(Intensity, (IInspectable**)value);
				//                case D2D1_SEPIA_PROP_ALPHA_MODE:
				//                {
				//                    switch (AlphaMode)
				//                    {
				//                        case CanvasAlphaMode_Premultiplied: 
				//                            return statics->CreateUInt32(D2D1_COLORMANAGEMENT_ALPHA_MODE_PREMULTIPLIED, (IInspectable**)value);
				//                        case CanvasAlphaMode_Straight:
				//                            return statics->CreateUInt32(D2D1_COLORMANAGEMENT_ALPHA_MODE_STRAIGHT, (IInspectable**)value);
				//                    }
				//                    break;
				//                }
				//            }
				//            return E_INVALIDARG;
				//        });
				//    }
				//};

				////-----------------------------------------------------------------------------------------------------------------

				//class SpotDiffuseEffect WrlFinal : public EffectBase<ISpotDiffuseEffect>
				//{
				//    InspectableClass(RuntimeClass_Microsoft_UI_Composition_Effects_SpotDiffuseEffect, BaseTrust);

				//public:
				//    DECLARE_D2D_GUID(CLSID_D2D1SpotDiffuse);
				//    DECLARE_SINGLE_SOURCE(Source);
				//    DECLARE_POD_PROPERTY(LightPosition, Vector3, (Vector3{ 0.0f, 0.0f, 0.0f }), true);
				//    DECLARE_POD_PROPERTY(LightTarget, Vector3, (Vector3{ 0.0f, 0.0f, 0.0f }), true);
				//    DECLARE_POD_PROPERTY(Focus, float, 1.0f, value >= -10000.0f && value <= 10000.0f);
				//    DECLARE_POD_PROPERTY(LimitingConeAngle, float, k_PI / 2.0f, true); // D2D clamps within [-90, 90] degrees
				//    DECLARE_POD_PROPERTY(DiffuseAmount, float, 1.0f, value >= 0.0f && value <= 10000.0f);
				//    DECLARE_POD_PROPERTY(HeightMapScale, float, 1.0f, value >= -10000.0f && value <= 10000.0f);
				//    DECLARE_POD_PROPERTY(LightColor, UIColor, (UIColor{ 255, 255, 255, 255 }), true);
				//    DECLARE_POD_PROPERTY(HeightMapKernelSize, Vector2, (Vector2{ 1.0f, 1.0f }),
				//        value.X >= 0.01f && value.Y >= 0.01f && value.X <= 100.0f && value.Y <= 100.0f);
				//    DECLARE_POD_PROPERTY(HeightMapInterpolationMode, CanvasImageInterpolation, CanvasImageInterpolation_Linear, true);
				//    DECLARE_NAMED_PROPERTY_MAPPING(
				//        { L"LightPosition", D2D1_SPOTDIFFUSE_PROP_LIGHT_POSITION, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
				//        { L"LightTarget", D2D1_SPOTDIFFUSE_PROP_POINTS_AT, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
				//        { L"Focus", D2D1_SPOTDIFFUSE_PROP_FOCUS, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
				//        { L"LimitingConeAngle", D2D1_SPOTDIFFUSE_PROP_LIMITING_CONE_ANGLE, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_RADIANS_TO_DEGREES },
				//        { L"DiffuseAmount", D2D1_SPOTDIFFUSE_PROP_DIFFUSE_CONSTANT, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
				//        { L"HeightMapScale", D2D1_SPOTDIFFUSE_PROP_SURFACE_SCALE, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
				//        { L"LightColor", D2D1_SPOTDIFFUSE_PROP_COLOR, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_COLOR_TO_VECTOR3 },
				//        { L"HeightMapKernelSize", D2D1_SPOTDIFFUSE_PROP_KERNEL_UNIT_LENGTH, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
				//        { L"HeightMapInterpolationMode", D2D1_SPOTDIFFUSE_PROP_SCALE_MODE, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },);

				//public:
				//    IFACEMETHODIMP GetPropertyCount(_Out_ UINT * count) override { *count = 9; return S_OK; }

				//    IFACEMETHODIMP GetProperty(UINT index, _Outptr_ IPropertyValue ** value) override
				//    {
				//        return UsePropertyFactory([=](IPropertyValueStatics* statics)
				//        {
				//            switch (index)
				//            {
				//                case D2D1_SPOTDIFFUSE_PROP_LIGHT_POSITION:
				//                    return statics->CreateSingleArray(3, reinterpret_cast<const float*>(&LightPosition), (IInspectable**)value);
				//                case D2D1_SPOTDIFFUSE_PROP_POINTS_AT:
				//                    return statics->CreateSingleArray(3, reinterpret_cast<const float*>(&LightTarget), (IInspectable**)value);
				//                case D2D1_SPOTDIFFUSE_PROP_FOCUS: return statics->CreateSingle(Focus, (IInspectable**)value);
				//                case D2D1_SPOTDIFFUSE_PROP_LIMITING_CONE_ANGLE:
				//                    return statics->CreateSingle(LimitingConeAngle * k_DegreesPerRadian, (IInspectable**)value);
				//                case D2D1_SPOTDIFFUSE_PROP_DIFFUSE_CONSTANT: return statics->CreateSingle(DiffuseAmount, (IInspectable**)value);
				//                case D2D1_SPOTDIFFUSE_PROP_SURFACE_SCALE: return statics->CreateSingle(HeightMapScale, (IInspectable**)value);
				//                case D2D1_SPOTDIFFUSE_PROP_COLOR: return CreateColor<3>(statics, LightColor, value);
				//                case D2D1_SPOTDIFFUSE_PROP_KERNEL_UNIT_LENGTH:
				//                    return statics->CreateSingleArray(2, reinterpret_cast<const float*>(&HeightMapKernelSize), (IInspectable**)value);
				//                case D2D1_SPOTDIFFUSE_PROP_SCALE_MODE: return statics->CreateUInt32(HeightMapInterpolationMode, (IInspectable**)value);
				//                default: return E_INVALIDARG;
				//            }
				//        });
				//    }
				//};

				////-----------------------------------------------------------------------------------------------------------------

				//class SpotSpecularEffect WrlFinal : public EffectBase<ISpotSpecularEffect>
				//{
				//    InspectableClass(RuntimeClass_Microsoft_UI_Composition_Effects_SpotSpecularEffect, BaseTrust);

				//public:
				//    DECLARE_D2D_GUID(CLSID_D2D1SpotSpecular);
				//    DECLARE_SINGLE_SOURCE(Source);
				//    DECLARE_POD_PROPERTY(LightPosition, Vector3, (Vector3{ 0.0f, 0.0f, 0.0f }), true);
				//    DECLARE_POD_PROPERTY(LightTarget, Vector3, (Vector3{ 0.0f, 0.0f, 0.0f }), true);
				//    DECLARE_POD_PROPERTY(Focus, float, 1.0f, value >= -10000.0f && value <= 10000.0f);
				//    DECLARE_POD_PROPERTY(LimitingConeAngle, float, k_PI / 2.0f, true); // D2D clamps within [-90, 90] degrees
				//    DECLARE_POD_PROPERTY(SpecularExponent, float, 1.0f, value >= -10000.0f && value <= 10000.0f);
				//    DECLARE_POD_PROPERTY(SpecularAmount, float, 1.0f, value >= 0.0f && value <= 10000.0f);
				//    DECLARE_POD_PROPERTY(HeightMapScale, float, 1.0f, value >= -10000.0f && value <= 10000.0f);
				//    DECLARE_POD_PROPERTY(LightColor, UIColor, (UIColor{ 255, 255, 255, 255 }), true);
				//    DECLARE_POD_PROPERTY(HeightMapKernelSize, Vector2, (Vector2{ 1.0f, 1.0f }),
				//        value.X >= 0.01f && value.Y >= 0.01f && value.X <= 100.0f && value.Y <= 100.0f);
				//    DECLARE_POD_PROPERTY(HeightMapInterpolationMode, CanvasImageInterpolation, CanvasImageInterpolation_Linear, true);
				//    DECLARE_NAMED_PROPERTY_MAPPING(
				//        { L"LightPosition", D2D1_SPOTDIFFUSE_PROP_LIGHT_POSITION, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
				//        { L"LightTarget", D2D1_SPOTDIFFUSE_PROP_POINTS_AT, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
				//        { L"Focus", D2D1_SPOTDIFFUSE_PROP_FOCUS, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
				//        { L"LimitingConeAngle", D2D1_SPOTDIFFUSE_PROP_LIMITING_CONE_ANGLE, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_RADIANS_TO_DEGREES },
				//        { L"SpecularExponent", D2D1_SPOTSPECULAR_PROP_SPECULAR_EXPONENT, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
				//        { L"SpecularAmount", D2D1_SPOTSPECULAR_PROP_SPECULAR_CONSTANT, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
				//        { L"HeightMapScale", D2D1_SPOTSPECULAR_PROP_SURFACE_SCALE, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
				//        { L"LightColor", D2D1_SPOTSPECULAR_PROP_COLOR, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_COLOR_TO_VECTOR3 },
				//        { L"HeightMapKernelSize", D2D1_SPOTSPECULAR_PROP_KERNEL_UNIT_LENGTH, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
				//        { L"HeightMapInterpolationMode", D2D1_SPOTSPECULAR_PROP_SCALE_MODE, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },);

				//public:
				//    IFACEMETHODIMP GetPropertyCount(_Out_ UINT * count) override { *count = 10; return S_OK; }

				//    IFACEMETHODIMP GetProperty(UINT index, _Outptr_ IPropertyValue ** value) override
				//    {
				//        return UsePropertyFactory([=](IPropertyValueStatics* statics)
				//        {
				//            switch (index)
				//            {
				//                case D2D1_SPOTDIFFUSE_PROP_LIGHT_POSITION:
				//                    return statics->CreateSingleArray(3, reinterpret_cast<const float*>(&LightPosition), (IInspectable**)value);
				//                case D2D1_SPOTDIFFUSE_PROP_POINTS_AT:
				//                    return statics->CreateSingleArray(3, reinterpret_cast<const float*>(&LightTarget), (IInspectable**)value);
				//                case D2D1_SPOTDIFFUSE_PROP_FOCUS: return statics->CreateSingle(Focus, (IInspectable**)value);
				//                case D2D1_SPOTDIFFUSE_PROP_LIMITING_CONE_ANGLE:
				//                    return statics->CreateSingle(LimitingConeAngle * k_DegreesPerRadian, (IInspectable**)value);
				//                case D2D1_SPOTSPECULAR_PROP_SPECULAR_EXPONENT: return statics->CreateSingle(SpecularExponent, (IInspectable**)value);
				//                case D2D1_SPOTSPECULAR_PROP_SPECULAR_CONSTANT: return statics->CreateSingle(SpecularAmount, (IInspectable**)value);
				//                case D2D1_SPOTSPECULAR_PROP_SURFACE_SCALE: return statics->CreateSingle(HeightMapScale, (IInspectable**)value);
				//                case D2D1_SPOTSPECULAR_PROP_COLOR: return CreateColor<3>(statics, LightColor, value);
				//                case D2D1_SPOTSPECULAR_PROP_KERNEL_UNIT_LENGTH:
				//                    return statics->CreateSingleArray(2, reinterpret_cast<const float*>(&HeightMapKernelSize), (IInspectable**)value);
				//                case D2D1_SPOTSPECULAR_PROP_SCALE_MODE: return statics->CreateUInt32(HeightMapInterpolationMode, (IInspectable**)value);
				//                default: return E_INVALIDARG;
				//            }
				//        });
				//    }
				//};

				////-----------------------------------------------------------------------------------------------------------------

				//class TemperatureAndTintEffect WrlFinal : public EffectBase<ITemperatureAndTintEffect>
				//{
				//    InspectableClass(RuntimeClass_Microsoft_UI_Composition_Effects_TemperatureAndTintEffect, BaseTrust);

				//public:
				//    DECLARE_D2D_GUID(CLSID_D2D1TemperatureTint);
				//    DECLARE_SINGLE_SOURCE(Source);
				//    DECLARE_POD_PROPERTY(Temperature, float, 0.0f, value >= -1.0f && value <= 1.0f);
				//    DECLARE_POD_PROPERTY(Tint, float, 0.0f, value >= -1.0f && value <= 1.0f);
				//    DECLARE_NAMED_PROPERTY_MAPPING(
				//        { L"Temperature", D2D1_TEMPERATUREANDTINT_PROP_TEMPERATURE, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
				//        { L"Tint", D2D1_TEMPERATUREANDTINT_PROP_TINT, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT });

				//public:
				//    IFACEMETHODIMP GetPropertyCount(_Out_ UINT * count) override { *count = 2; return S_OK; }

				//    IFACEMETHODIMP GetProperty(UINT index, _Outptr_ IPropertyValue ** value) override
				//    {
				//        return UsePropertyFactory([=](IPropertyValueStatics* statics)
				//        {
				//            switch (index)
				//            {
				//                case D2D1_TEMPERATUREANDTINT_PROP_TEMPERATURE: return statics->CreateSingle(Temperature, (IInspectable**)value);
				//                case D2D1_TEMPERATUREANDTINT_PROP_TINT: return statics->CreateSingle(Tint, (IInspectable**)value);
				//                default: return E_INVALIDARG;
				//            }
				//        });
				//    }
				//};

				////-----------------------------------------------------------------------------------------------------------------

				//class TintEffect WrlFinal : public EffectBase<ITintEffect>
				//{
				//    InspectableClass(RuntimeClass_Microsoft_UI_Composition_Effects_TintEffect, BaseTrust);

				//public:
				//    DECLARE_D2D_GUID(CLSID_D2D1Tint);
				//    DECLARE_SINGLE_SOURCE(Source);
				//    DECLARE_POD_PROPERTY(Color, UIColor, (UIColor{ 255, 255, 255, 255 }), true);
				//    DECLARE_POD_PROPERTY(ClampOutput, boolean, static_cast<boolean>(false), true);
				//    DECLARE_NAMED_PROPERTY_MAPPING(
				//        { L"Color", D2D1_TINT_PROP_COLOR, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_COLOR_TO_VECTOR4 },
				//        { L"ClampOutput", D2D1_TINT_PROP_CLAMP_OUTPUT, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT });

				//public:
				//    IFACEMETHODIMP GetPropertyCount(_Out_ UINT * count) override { *count = 2; return S_OK; }

				//    IFACEMETHODIMP GetProperty(UINT index, _Outptr_ IPropertyValue ** value) override
				//    {
				//        return UsePropertyFactory([=](IPropertyValueStatics* statics)
				//        {
				//            switch (index)
				//            {
				//                case D2D1_TINT_PROP_COLOR: return CreateColor<4>(statics, Color, value);
				//                case D2D1_TINT_PROP_CLAMP_OUTPUT: return statics->CreateBoolean(ClampOutput, (IInspectable**)value);
				//                default: return E_INVALIDARG;
				//            }
				//        });
				//    }
				//};

				////-----------------------------------------------------------------------------------------------------------------

				//class Transform2DEffect WrlFinal : public EffectBase<ITransform2DEffect>
				//{
				//    InspectableClass(RuntimeClass_Microsoft_UI_Composition_Effects_Transform2DEffect, BaseTrust);

				//public:
				//    DECLARE_D2D_GUID(CLSID_D2D12DAffineTransform);
				//    DECLARE_SINGLE_SOURCE(Source);
				//    DECLARE_POD_PROPERTY(InterpolationMode, CanvasImageInterpolation, CanvasImageInterpolation_Linear, true);
				//    DECLARE_POD_PROPERTY(BorderMode, EffectBorderMode, EffectBorderMode_Soft, true);
				//    DECLARE_POD_PROPERTY(TransformMatrix, Matrix3x2, (Matrix3x2{ 1, 0, 0, 1, 0, 0}), true);
				//    DECLARE_POD_PROPERTY(Sharpness, float, 0.0f, value >= 0.0f && value <= 1.0f);
				//    DECLARE_NAMED_PROPERTY_MAPPING(
				//        { L"InterpolationMode", D2D1_2DAFFINETRANSFORM_PROP_INTERPOLATION_MODE, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
				//        { L"BorderMode", D2D1_2DAFFINETRANSFORM_PROP_BORDER_MODE, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
				//        { L"TransformMatrix", D2D1_2DAFFINETRANSFORM_PROP_TRANSFORM_MATRIX, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
				//        { L"Sharpness", D2D1_2DAFFINETRANSFORM_PROP_SHARPNESS, PropertyMapping::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT });

				//public:
				//    IFACEMETHODIMP GetPropertyCount(_Out_ UINT * count) override { *count = 4; return S_OK; }

				//    IFACEMETHODIMP GetProperty(UINT index, _Outptr_ IPropertyValue ** value) override
				//    {
				//        return UsePropertyFactory([=](IPropertyValueStatics* statics)
				//        {
				//            switch (index)
				//            {
				//                case D2D1_2DAFFINETRANSFORM_PROP_INTERPOLATION_MODE: return statics->CreateUInt32(InterpolationMode, (IInspectable**)value);
				//                case D2D1_2DAFFINETRANSFORM_PROP_BORDER_MODE: return statics->CreateUInt32(BorderMode, (IInspectable**)value);
				//                case D2D1_2DAFFINETRANSFORM_PROP_TRANSFORM_MATRIX:
				//                    return statics->CreateSingleArray(6, reinterpret_cast<const float*>(&TransformMatrix), (IInspectable**)value);
				//                case D2D1_2DAFFINETRANSFORM_PROP_SHARPNESS: return statics->CreateSingle(Sharpness, (IInspectable**)value);
				//            default: return E_INVALIDARG;
				//            }
				//        });
				//    }
				//};

				//-----------------------------------------------------------------------------------------------------------------

				/*class UnPremultiplyEffect WrlFinal : public EffectBase<IUnPremultiplyEffect>
				{
				InspectableClass(RuntimeClass_Microsoft_UI_Composition_Effects_UnPremultiplyEffect, BaseTrust);

				public:
				DECLARE_D2D_GUID(CLSID_D2D1UnPremultiply);
				DECLARE_SINGLE_SOURCE(Source);
				};*/

				//-----------------------------------------------------------------------------------------------------------------
				// Clean up preprocessor state

#ifndef MICROSOFT_UI_COMPOSITION_EFFECT_IMPL_KEEP_MACROS
#    pragma pop_macro("DECLARE_D2D_GUID")
#    pragma pop_macro("DECLARE_POD_PROPERTY")
#    pragma pop_macro("DECLARE_SOURCE")
#    pragma pop_macro("DECLARE_SINGLE_SOURCE")
#    pragma pop_macro("DECLARE_DUAL_SOURCES")
#    pragma pop_macro("DECLARE_NAMED_PROPERTY_MAPPING")
#endif

			}
		}
	}
}
#pragma once
