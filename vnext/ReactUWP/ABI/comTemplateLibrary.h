// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include <wrl.h>
#include <wrl/ftm.h>
//
// Adapted from Windows DXaml comTemplateLibrary.h
//

namespace ctl
{

#define ValidateOutputArgPtr(o) if ((LPVOID)o == NULL) return E_POINTER;
#define CheckAllocationPtr(o) if (o == NULL) return E_OUTOFMEMORY;

	interface INonDelegatingUnknown
	{
	public:
		virtual HRESULT STDMETHODCALLTYPE NonDelegatingQueryInterface(
			/* [in] */ REFIID riid,
			/* [iid_is][out] */ __RPC__deref_out void __RPC_FAR *__RPC_FAR *ppvObject) = 0;
		virtual ULONG STDMETHODCALLTYPE NonDelegatingAddRef() = 0;
		virtual ULONG STDMETHODCALLTYPE NonDelegatingRelease() = 0;
	};


	interface INonDelegatingInspectable : public INonDelegatingUnknown
	{
		virtual HRESULT STDMETHODCALLTYPE NonDelegatingGetIids(
			/* [out] */ __RPC__out ULONG* iidCount,
			/* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID** iids) = 0;

		virtual HRESULT STDMETHODCALLTYPE NonDelegatingGetRuntimeClassName(
			/* [out] */ __RPC__deref_out_opt HSTRING* className) = 0;

		virtual HRESULT STDMETHODCALLTYPE NonDelegatingGetTrustLevel(
			/* [out] */ __RPC__out TrustLevel* trustLevel) = 0;
	};


	template <class TBASE>
	class AggregableComObject :
		public INonDelegatingInspectable,
		public TBASE
	{
	public:
		AggregableComObject(__in IInspectable* pOuter)
		{
			static_assert(__is_base_of(::Microsoft::WRL::Details::RuntimeClassBase, TBASE), "AggregableComObject can only be used with ::Microsoft::WRL::RuntimeClass types");

			m_pControllingUnknown = pOuter;
		}

		template <class... TArgs>
		AggregableComObject(__in IInspectable* pOuter, TArgs&&... args) : TBASE(std::forward<TArgs>(args)...)
		{
			static_assert(__is_base_of(::Microsoft::WRL::Details::RuntimeClassBase, TBASE), "AggregableComObject can only be used with ::Microsoft::WRL::RuntimeClass types");

			m_pControllingUnknown = pOuter;
		}

		// IInspectable (non-delegating) implementation
		IFACEMETHODIMP NonDelegatingQueryInterface(REFIID iid, void** ppInterface) override
		{
			ValidateOutputArgPtr(ppInterface);
			IUnknown* pInterface = nullptr;

			if (iid == IID_IUnknown)
			{
				pInterface = reinterpret_cast<IUnknown*>(static_cast<ctl::INonDelegatingUnknown*>(this));
			}
			else if (iid == IID_IInspectable)
			{
				pInterface = reinterpret_cast<IInspectable*>(static_cast<ctl::INonDelegatingInspectable*>(this));
			}
			else
			{
				return TBASE::QueryInterface(iid, ppInterface);
			}

			*ppInterface = pInterface;
			pInterface->AddRef();
			return S_OK;
		}

		IFACEMETHODIMP_(ULONG) NonDelegatingAddRef() override
		{
			return TBASE::AddRef();
		}

		IFACEMETHODIMP_(ULONG) NonDelegatingRelease() override
		{
			return TBASE::Release();
		}

		IFACEMETHODIMP NonDelegatingGetRuntimeClassName(__out HSTRING* pClassName) override
		{
			return TBASE::GetRuntimeClassName(pClassName);
		}

		IFACEMETHODIMP NonDelegatingGetTrustLevel(__out TrustLevel* trustLevel) override
		{
			return TBASE::GetTrustLevel(trustLevel);
		}

		IFACEMETHODIMP NonDelegatingGetIids(__out ULONG* iidCount, __deref_out IID** iids) override
		{
			return TBASE::GetIids(iidCount, iids);
		}

		// IInspectable (delegating) implementation
		IFACEMETHODIMP QueryInterface(REFIID iid, void** ppValue) override
		{
			return m_pControllingUnknown->QueryInterface(iid, ppValue);
		}

		IFACEMETHODIMP_(ULONG) AddRef() override
		{
			return m_pControllingUnknown->AddRef();
		}

		IFACEMETHODIMP_(ULONG) Release() override
		{
			return m_pControllingUnknown->Release();
		}

		IFACEMETHODIMP GetRuntimeClassName(__out HSTRING* pClassName) override
		{
			return m_pControllingUnknown->GetRuntimeClassName(pClassName);
		}

		IFACEMETHODIMP GetTrustLevel(__out TrustLevel* trustLvl) override
		{
			return m_pControllingUnknown->GetTrustLevel(trustLvl);
		}

		IFACEMETHODIMP GetIids(__out ULONG* iidCount, __deref_out IID** iids) override
		{
			return m_pControllingUnknown->GetIids(iidCount, iids);
		}

	public:

		static __checkReturn HRESULT CreateInstance(__in IInspectable* pOuter, __deref_out TBASE** ppNewInstance)
		{
			ValidateOutputArgPtr(ppNewInstance);
			::Microsoft::WRL::ComPtr<ctl::AggregableComObject<TBASE>> pNewInstance(::Microsoft::WRL::Make<ctl::AggregableComObject<TBASE>>(pOuter));
			CheckAllocationPtr(pNewInstance);
			*ppNewInstance = static_cast<TBASE*>(pNewInstance.Detach());
			return S_OK;
		}

		template <class... TArgs>
		static __checkReturn HRESULT CreateInstance(__in IInspectable* pOuter, __deref_out TBASE** ppNewInstance, TArgs&&... args)
		{
			ValidateOutputArgPtr(ppNewInstance);
			::Microsoft::WRL::ComPtr<ctl::AggregableComObject<TBASE>> pNewInstance(::Microsoft::WRL::Make<ctl::AggregableComObject<TBASE>>(pOuter, std::forward<TArgs>(args)...));
			CheckAllocationPtr(pNewInstance);
			*ppNewInstance = static_cast<TBASE*>(pNewInstance.Detach());
			return S_OK;
		}

		template <class T>
		static __checkReturn HRESULT CreateInstance(__in IInspectable* pOuter, __deref_out T** ppNewInstance)
		{
			ValidateOutputArgPtr(ppNewInstance);
			::Microsoft::WRL::ComPtr<ctl::AggregableComObject<TBASE>> pNewInstance(::Microsoft::WRL::Make<ctl::AggregableComObject<TBASE>>(pOuter));
			CheckAllocationPtr(pNewInstance);
			return pNewInstance.CopyTo(ppNewInstance);
		}

		template <class T, class... TArgs>
		static __checkReturn HRESULT CreateInstance(__in IInspectable* pOuter, __deref_out T** ppNewInstance, TArgs&&... args)
		{
			ValidateOutputArgPtr(ppNewInstance);
			::Microsoft::WRL::ComPtr<ctl::AggregableComObject<TBASE>> pNewInstance(::Microsoft::WRL::Make<ctl::AggregableComObject<TBASE>>(pOuter, std::forward<TArgs>(args)...));
			CheckAllocationPtr(pNewInstance);
			return pNewInstance.CopyTo(ppNewInstance);
		}

	private:
		IInspectable* m_pControllingUnknown;
	};


	template <class TBASE>
	class AggregableComFactory
	{
	public:
		static HRESULT STDMETHODCALLTYPE ActivateInstance(IInspectable** ppInstance)
		{
			ValidateOutputArgPtr(ppInstance);
			::Microsoft::WRL::ComPtr<TBASE> instance(::Microsoft::WRL::Make<TBASE>());
			CheckAllocationPtr(instance);
			return instance.CopyTo(ppInstance);
		}

		template <class... TArgs>
		static HRESULT STDMETHODCALLTYPE ActivateInstance(IInspectable** ppInstance, TArgs&&... args)
		{
			ValidateOutputArgPtr(ppInstance);
			::Microsoft::WRL::ComPtr<TBASE> instance(::Microsoft::WRL::Make<TBASE>(std::forward<TArgs>(args)...));
			CheckAllocationPtr(instance);
			return instance.CopyTo(ppInstance);
		}

		template <class TInterface>
		static HRESULT STDMETHODCALLTYPE CreateInstance(IInspectable* pOuter, IInspectable** ppInner, TInterface** ppInstance)
		{
			ValidateOutputArgPtr(ppInstance);
      if (pOuter != nullptr && ppInner == nullptr)
        return E_UNEXPECTED;

			if (pOuter != nullptr)
			{
				return CreateAggregatedInstance(pOuter, ppInner, ppInstance);
			}
			else
			{
				::Microsoft::WRL::ComPtr<IInspectable> pInstance;
				ActivateInstance(&pInstance);
				return pInstance.CopyTo(ppInstance);
			}
		}

		template <class TInterface, class... TArgs>
		static HRESULT STDMETHODCALLTYPE CreateInstance(IInspectable* pOuter, IInspectable** ppInner, TInterface** ppInstance, TArgs&&... args)
		{
			ValidateOutputArgPtr(ppInstance);
      if (pOuter != nullptr && ppInner == nullptr)
        return E_UNEXPECTED;

			if (pOuter != nullptr)
			{
				return CreateAggregatedInstance(pOuter, ppInner, ppInstance, std::forward<TArgs>(args)...);
			}
			else
			{
				::Microsoft::WRL::ComPtr<IInspectable> pInstance;
				ActivateInstance(&pInstance, std::forward<TArgs>(args)...);
				return pInstance.CopyTo(ppInstance);
			}
		}

	private:
		template <class TInterface>
		static HRESULT CreateAggregatedInstance(IInspectable* pOuter, IInspectable** ppInner, TInterface** ppInstance)
		{
			ValidateOutputArgPtr(ppInstance);
			ValidateOutputArgPtr(ppInner);
      if (pOuter != nullptr)
        return E_INVALIDARG;

			ctl::AggregableComObject<TBASE> *pInstance = nullptr;
			ctl::AggregableComObject<TBASE>::CreateInstance(pOuter, &pInstance);

			*ppInner = reinterpret_cast<IInspectable*>(static_cast<ctl::INonDelegatingInspectable*>(pInstance));

			*ppInstance = static_cast<TInterface*>(pInstance);
			(*ppInstance)->AddRef();
			return S_OK;
		}

		template <class TInterface, class... TArgs>
		static HRESULT CreateAggregatedInstance(IInspectable* pOuter, IInspectable** ppInner, TInterface** ppInstance, TArgs&&... args)
		{
			ValidateOutputArgPtr(ppInstance);
			ValidateOutputArgPtr(ppInner);
      if (pOuter != nullptr)
        return E_INVALIDARG;

			ctl::AggregableComObject<TBASE> *pInstance = nullptr;
			ctl::AggregableComObject<TBASE>::CreateInstance(pOuter, &pInstance, std::forward<TArgs>(args)...);

			*ppInner = reinterpret_cast<IInspectable*>(static_cast<ctl::INonDelegatingInspectable*>(pInstance));

			*ppInstance = static_cast<TInterface*>(pInstance);
			(*ppInstance)->AddRef();
			return S_OK;
		}
	};


} // namespace ctl
