#pragma once

// System Includes
#include <cassert>

/* Assert function allowing assert and use of the variable at the same time */
static bool AssertValue(bool Exp)
{
    assert(Exp);
    return Exp;
}

/* attempts to static cast to cast type and assert value is none null if the given pointer is none null */
template<typename CastType, typename FromType>
CastType* AssertCast(FromType* InPtr)
{
    CastType* CastPtr = static_cast<CastType*>(InPtr);
    assert(InPtr == nullptr || CastPtr != nullptr);

    return CastPtr;
}
