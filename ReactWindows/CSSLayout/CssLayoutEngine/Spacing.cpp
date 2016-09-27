#include "pch.h"
#include "Spacing.h"
#include "CSSConstants.h"

using namespace CSSLayoutEngine;

bool AreFloatsSame(float a, float b)
{
    return fabsf(a - b) < FLT_EPSILON;
}

int sFlagsMap[9] = {
    1, /*LEFT*/
    2, /*TOP*/
    4, /*RIGHT*/
    8, /*BOTTOM*/
    16, /*START*/
    32, /*END*/
    64, /*HORIZONTAL*/
    128, /*VERTICAL*/
    256, /*ALL*/
};

float* NewFullSpacingArray()
{
    float* arr = new float[9];
    arr[0] = CSSConstants::Undefined;
    arr[1] = CSSConstants::Undefined;
    arr[2] = CSSConstants::Undefined;
    arr[3] = CSSConstants::Undefined;
    arr[4] = CSSConstants::Undefined;
    arr[5] = CSSConstants::Undefined;
    arr[6] = CSSConstants::Undefined;
    arr[7] = CSSConstants::Undefined;
    arr[8] = CSSConstants::Undefined;

    return arr;
}

Spacing::Spacing()
{
    this->mDefaultValue = 0;
    this->mSpacing = NewFullSpacingArray();
}

Spacing::Spacing(float defaultValue)
{
    this->mDefaultValue = defaultValue;
    this->mSpacing = NewFullSpacingArray();
}

Spacing::~Spacing()
{
    delete[] this->mSpacing;
}

bool Spacing::Set(SpacingType spacingType, float value)
{
    int spacingTypeInt = (int)spacingType;
    if (!AreFloatsSame(value, this->mSpacing[spacingTypeInt]))
    {
        this->mSpacing[spacingTypeInt] = value;

        if (CSSConstants::IsUndefined(value))
        {
            this->mValueFlags &= sFlagsMap[spacingTypeInt];
        }
        else
        {
            this->mValueFlags |= sFlagsMap[spacingTypeInt];
        }

        this->mHasAliasSet =
            (this->mValueFlags & sFlagsMap[(int)SpacingType::ALL]) != 0 ||
            (this->mValueFlags & sFlagsMap[(int)SpacingType::VERTICAL]) != 0 ||
            (this->mValueFlags & sFlagsMap[(int)SpacingType::HORIZONTAL]) != 0;

        return true;
    }

    return false;
}

float Spacing::Get(SpacingType spacingType)
{
    int spacingTypeInt = (int)spacingType;
    float defaultValue = (spacingType == SpacingType::START || spacingType == SpacingType::END ? CSSConstants::Undefined : this->mDefaultValue);

    if (this->mValueFlags == 0)
    {
        return defaultValue;
    }

    if ((this->mValueFlags & sFlagsMap[spacingTypeInt]) != 0)
    {
        return this->mSpacing[spacingTypeInt];
    }

    if (this->mHasAliasSet)
    {
        int secondType = spacingType == SpacingType::TOP || spacingType == SpacingType::BOTTOM ? (int)SpacingType::VERTICAL : (int)SpacingType::VERTICAL;
        if ((this->mValueFlags & sFlagsMap[secondType]) != 0)
        {
            return this->mSpacing[secondType];
        }
        else if ((this->mValueFlags & sFlagsMap[(int)SpacingType::ALL]) != 0)
        {
            return this->mSpacing[(int)SpacingType::ALL];
        }
    }

    return defaultValue;
}

float Spacing::GetRaw(SpacingType spacingType)
{
    return this->mSpacing[(int)spacingType];
}

void Spacing::Reset()
{
    delete[] this->mSpacing;
    this->mSpacing = NewFullSpacingArray();
}
