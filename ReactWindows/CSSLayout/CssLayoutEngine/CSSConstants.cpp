#include "pch.h"
#include "CSSConstants.h"

using namespace CSSLayoutEngine;

bool CSSConstants::IsUndefined(float value)
{
    return isnan(value);
}
