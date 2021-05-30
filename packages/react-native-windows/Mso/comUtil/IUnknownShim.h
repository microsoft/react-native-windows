// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#ifndef MSO_COMUTIL_IUNKNOWNSHIM_H
#define MSO_COMUTIL_IUNKNOWNSHIM_H

#include <guid/msoGuid.h>
#include <cstdint>

// This is needed for compiling with Clang on Windows as well
MSO_STRUCT_GUID(IUnknown, "00000000-0000-0000-C000-000000000046")

#if !defined(MS_TARGET_POSIX)

#include <combaseapi.h>
#include <unknwn.h>
#include "compilerAdapters/declspecDefinitions.h"
#include "platformAdapters/windowsFirst.h"

#else

#include <guiddef.h>

#ifdef COM_STDMETHOD_CAN_THROW
#define COM_DECLSPEC_NOTHROW
#else
#define COM_DECLSPEC_NOTHROW DECLSPEC_NOTHROW
#endif

#define FACILITY_WIN32 7

#define __HRESULT_FROM_WIN32(x) \
  ((HRESULT)(x) <= 0 ? ((HRESULT)(x)) : ((HRESULT)(((x)&0x0000FFFF) | (FACILITY_WIN32 << 16) | 0x80000000)))
#define HRESULT_FROM_WIN32(x) __HRESULT_FROM_WIN32(x)

#ifndef __LP64__
typedef long HRESULT;
#else
typedef int HRESULT;
#endif
using ULONG = unsigned long;
using LONG = long;
using BYTE = uint8_t;
#ifndef __GNUC__
#define STDMETHODCALLTYPE __stdcall
#else
#define STDMETHODCALLTYPE
#endif
#define STDMETHOD(method) virtual HRESULT STDMETHODCALLTYPE method
#define STDMETHOD_(type, method) virtual type STDMETHODCALLTYPE method
#define STDMETHODIMP HRESULT STDMETHODCALLTYPE
#define STDMETHODIMP_(type) type STDMETHODCALLTYPE
#define STDMETHODIMPNOTHROW COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE
#define STDMETHODIMPNOTHROW_(type) COM_DECLSPEC_NOTHROW type STDMETHODCALLTYPE

#define TRUE 1
#define FALSE 0

#define S_OK ((HRESULT)0L)
#define S_FALSE ((HRESULT)1L)

#define ERROR_INSUFFICIENT_BUFFER 122L

#define STG_E_WRITEFAULT ((HRESULT)0x8003001DL)
#define STG_E_READFAULT ((HRESULT)0x8003001EL)
#define E_NOINTERFACE ((HRESULT)0x80004002L)
#define E_ABORT ((HRESULT)0x80004004L)
#define E_FAIL ((HRESULT)0x80004005L)
#define E_POINTER ((HRESULT)0x80004003L)
#define E_NOTIMPL ((HRESULT)0x80004001L)
#define ERROR_NOT_SUPPORTED ((HRESULT)0x80070032L)
#define E_INVALIDARG ((HRESULT)0x80070057L)
#define E_ACCESSDENIED ((HRESULT)0x80070005L)
#define NOERROR 0
#define E_UNEXPECTED ((HRESULT)0x8000FFFFL)
#define E_CHANGED_STATE ((HRESULT)0x8000000CL)
#define DISP_E_UNKNOWNNAME ((HRESULT)0x80020006L)
#define DISP_E_PARAMNOTFOUND ((HRESULT)0x80020004L)
#define DISP_E_MEMBERNOTFOUND ((HRESULT)0x80020003L)
#define DISP_E_BADINDEX ((HRESULT)0x8002000BL)
#define DISP_E_TYPEMISMATCH ((HRESULT)0x80020005L)
#define DISP_E_OVERFLOW ((HRESULT)0x8002000AL)
#define DISP_E_BADVARTYPE ((HRESULT)0x80020008L)
#define DISP_E_ARRAYISLOCKED ((HRESULT)0x8002000DL)
#define CLASS_E_CLASSNOTAVAILABLE ((HRESULT)0x80040111L)
#define E_BOUNDS ((HRESULT)0x8000000BL)
#define CO_E_CLASSSTRING ((HRESULT)0x800401F3L)

#ifndef __GNUC__ // Redefinition (see line ~5)
MSO_STRUCT_GUID(IUnknown, "00000000-0000-0000-C000-000000000046")
#endif
struct IUnknown {
  virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject) = 0;
  virtual ULONG STDMETHODCALLTYPE AddRef(void) = 0;
  virtual ULONG STDMETHODCALLTYPE Release(void) = 0;
};

#define SUCCEEDED(hr) (((HRESULT)(hr)) >= 0)
#define FAILED(hr) (((HRESULT)(hr)) < 0)

#define __RPC_FAR

#endif

namespace Mso {
using IUnknown = ::IUnknown;
}

#endif // MSO_COMUTIL_IUNKNOWNSHIM_H
