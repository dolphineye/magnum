/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018
              Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2016 Jonathan Hale <squareys@googlemail.com>

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/

#include <sstream>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Container.h>

#include "Magnum/Math/Frustum.h"

struct Frstm {
    float data[24];
};

namespace Magnum { namespace Math {

namespace Implementation {

template<> struct FrustumConverter<Float, Frstm> {
    constexpr static Frustum<Float> from(const Frstm& other) {
        return {{other.data[ 0 + 0], other.data[ 0 + 1], other.data[ 0 + 2], other.data[ 0 + 3]},
                {other.data[ 4 + 0], other.data[ 4 + 1], other.data[ 4 + 2], other.data[ 4 + 3]},
                {other.data[ 8 + 0], other.data[ 8 + 1], other.data[ 8 + 2], other.data[ 8 + 3]},
                {other.data[12 + 0], other.data[12 + 1], other.data[12 + 2], other.data[12 + 3]},
                {other.data[16 + 0], other.data[16 + 1], other.data[16 + 2], other.data[16 + 3]},
                {other.data[20 + 0], other.data[20 + 1], other.data[20 + 2], other.data[20 + 3]}};
    }

    constexpr static Frstm to(const Frustum<Float>& other) {
        return {{other[0][0], other[0][1], other[0][2], other[0][3],
                 other[1][0], other[1][1], other[1][2], other[1][3],
                 other[2][0], other[2][1], other[2][2], other[2][3],
                 other[3][0], other[3][1], other[3][2], other[3][3],
                 other[4][0], other[4][1], other[4][2], other[4][3],
                 other[5][0], other[5][1], other[5][2], other[5][3]}};
    }
};

}

namespace Test {

struct FrustumTest: Corrade::TestSuite::Tester {
    explicit FrustumTest();

    void construct();
    void constructIdentity();
    void constructNoInit();
    void constructConversion();
    void constructCopy();
    void constructFromMatrix();
    void convert();

    void data();

    void compare();

    void debug();
};

typedef Math::Vector4<Float> Vector4;
typedef Math::Matrix4<Float> Matrix4;
typedef Math::Frustum<Float> Frustum;
typedef Math::Frustum<Double> Frustumd;

FrustumTest::FrustumTest() {
    addTests({&FrustumTest::construct,
              &FrustumTest::constructIdentity,
              &FrustumTest::constructNoInit,
              &FrustumTest::constructConversion,
              &FrustumTest::constructCopy,
              &FrustumTest::constructFromMatrix,
              &FrustumTest::convert,

              &FrustumTest::data,

              &FrustumTest::compare,

              &FrustumTest::debug});
}

void FrustumTest::construct() {
    constexpr Vector4 planes[6]{
        {-1.0f,  2.0f, -3.0f, 0.1f},
        { 1.0f, -2.0f,  3.0f, 0.2f},
        {-4.0f,  5.0f, -6.0f, 0.3f},
        { 4.0f, -5.0f,  6.0f, 0.4f},
        {-7.0f,  8.0f, -9.0f, 0.5f},
        { 7.0f,  8.0f,  9.0f, 0.6f}};

    constexpr Frustum frustum = {
        planes[0], planes[1],
        planes[2], planes[3],
        planes[4], planes[5]};

    CORRADE_COMPARE_AS(frustum.planes(), Corrade::Containers::arrayView(planes),
                       Corrade::TestSuite::Compare::Container);

    CORRADE_VERIFY((std::is_nothrow_constructible<Frustum, Vector4, Vector4, Vector4, Vector4, Vector4, Vector4>::value));
}

void FrustumTest::constructIdentity() {
    Frustum expected{
        { 1.0f,  0.0f,  0.0f, 1.0f},
        {-1.0f,  0.0f,  0.0f, 1.0f},
        { 0.0f,  1.0f,  0.0f, 1.0f},
        { 0.0f, -1.0f,  0.0f, 1.0f},
        { 0.0f,  0.0f,  1.0f, 1.0f},
        { 0.0f,  0.0f, -1.0f, 1.0f}};

    constexpr Frustum a;
    constexpr Frustum b{IdentityInit};
    CORRADE_COMPARE(a, expected);
    CORRADE_COMPARE(b, expected);

    CORRADE_VERIFY(std::is_nothrow_default_constructible<Frustum>::value);
    CORRADE_VERIFY((std::is_nothrow_constructible<Frustum, IdentityInitT>::value));
}

void FrustumTest::constructNoInit() {
    Frustum a{
        {-1.0f,  2.0f, -3.0f, 0.1f},
        { 1.0f, -2.0f,  3.0f, 0.2f},
        {-4.0f,  5.0f, -6.0f, 0.3f},
        { 4.0f, -5.0f,  6.0f, 0.4f},
        {-7.0f,  8.0f, -9.0f, 0.5f},
        { 7.0f,  8.0f,  9.0f, 0.6f}};

    new(&a) Frustum{NoInit};
    {
        #if defined(__GNUC__) && __GNUC__*100 + __GNUC_MINOR__ >= 601 && __OPTIMIZE__
        CORRADE_EXPECT_FAIL("GCC 6.1+ misoptimizes and overwrites the value.");
        #endif

        CORRADE_COMPARE(a, (Frustum{
            {-1.0f,  2.0f, -3.0f, 0.1f},
            { 1.0f, -2.0f,  3.0f, 0.2f},
            {-4.0f,  5.0f, -6.0f, 0.3f},
            { 4.0f, -5.0f,  6.0f, 0.4f},
            {-7.0f,  8.0f, -9.0f, 0.5f},
            { 7.0f,  8.0f,  9.0f, 0.6f}}));
    }

    /* Implicit construction is not allowed */
    CORRADE_VERIFY((std::is_nothrow_constructible<Frustum, NoInitT>::value));
}

void FrustumTest::constructConversion() {
    constexpr Frustumd a{
        {-1.0,  2.0, -3.0, 0.1},
        { 1.0, -2.0,  3.0, 0.2},
        {-4.0,  5.0, -6.0, 0.3},
        { 4.0, -5.0,  6.0, 0.4},
        {-7.0,  8.0, -9.0, 0.5},
        { 7.0,  8.0,  9.0, 0.6}};
    Frustum expected{
        {-1.0f,  2.0f, -3.0f, 0.1f},
        { 1.0f, -2.0f,  3.0f, 0.2f},
        {-4.0f,  5.0f, -6.0f, 0.3f},
        { 4.0f, -5.0f,  6.0f, 0.4f},
        {-7.0f,  8.0f, -9.0f, 0.5f},
        { 7.0f,  8.0f,  9.0f, 0.6f}};

    constexpr Frustum b{a};
    CORRADE_COMPARE(b, expected);

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<Frustum, Frustumd>::value));

    CORRADE_VERIFY((std::is_nothrow_constructible<Frustum, Frustumd>::value));
}

void FrustumTest::constructCopy() {
    constexpr Frustum a{
        {-1.0f,  2.0f, -3.0f, 0.1f},
        { 1.0f, -2.0f,  3.0f, 0.2f},
        {-4.0f,  5.0f, -6.0f, 0.3f},
        { 4.0f, -5.0f,  6.0f, 0.4f},
        {-7.0f,  8.0f, -9.0f, 0.5f},
        { 7.0f,  8.0f,  9.0f, 0.6f}};
    constexpr Frustum b{a};
    CORRADE_COMPARE(b, a);

    CORRADE_VERIFY(std::is_nothrow_copy_constructible<Frustum>::value);
    CORRADE_VERIFY(std::is_nothrow_copy_assignable<Frustum>::value);
}

void FrustumTest::constructFromMatrix() {
    using namespace Magnum::Math::Literals;

    Frustum expected{
        { 1.0f,  0.0f, -1.0f, 0.0f},
        {-1.0f,  0.0f, -1.0f, 0.0f},
        { 0.0f,  1.0f, -1.0f, 0.0f},
        { 0.0f, -1.0f, -1.0f, 0.0f},
        { 0.0f,  0.0f, -2.22222f, -2.22222f},
        { 0.0f,  0.0f,  0.22222f,  2.22222f}};

    const Frustum frustum = Frustum::fromMatrix(
            Matrix4::perspectiveProjection(90.0_degf, 1.0f, 1.0f, 10.0f));

    CORRADE_COMPARE(frustum, expected);

    /* Constructing from a default-constructed matrix should be equivalent to
       default constructor */
    CORRADE_COMPARE(Frustum::fromMatrix({}), Frustum{});
}

void FrustumTest::convert() {
    constexpr Frstm a{{
        -1.0f,  2.0f, -3.0f, 0.1f,
         1.0f, -2.0f,  3.0f, 0.2f,
        -4.0f,  5.0f, -6.0f, 0.3f,
         4.0f, -5.0f,  6.0f, 0.4f,
        -7.0f,  8.0f, -9.0f, 0.5f,
         7.0f,  8.0f,  9.0f, 0.6f}};
    constexpr Frustum b{
        {-1.0f,  2.0f, -3.0f, 0.1f},
        { 1.0f, -2.0f,  3.0f, 0.2f},
        {-4.0f,  5.0f, -6.0f, 0.3f},
        { 4.0f, -5.0f,  6.0f, 0.4f},
        {-7.0f,  8.0f, -9.0f, 0.5f},
        { 7.0f,  8.0f,  9.0f, 0.6f}};

    constexpr Frustum c{a};
    CORRADE_COMPARE(c, b);

    constexpr Frstm d(b);
    CORRADE_COMPARE_AS(Corrade::Containers::arrayView(d.data),
        Corrade::Containers::arrayView(a.data),
        Corrade::TestSuite::Compare::Container);

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<Frstm, Frustum>::value));
    CORRADE_VERIFY(!(std::is_convertible<Frustum, Frstm>::value));
}

void FrustumTest::data() {
    /* Using default-constructed to verify that the planes are in correct order */
    constexpr Frustum a;

    #if !defined(__GNUC__) || __GNUC__*100 + __GNUC_MINOR__ >= 500
    constexpr
    #endif
    Vector4 right = a.planes()[1];
    CORRADE_COMPARE(right, (Vector4{-1.0f, 0.0f, 0.0f, 1.0f}));

    constexpr Vector4 bottom = a[2];
    CORRADE_COMPARE(bottom, (Vector4{0.0f, 1.0f, 0.0f, 1.0f}));

    constexpr Vector4 near = a.near();
    CORRADE_COMPARE(near, (Vector4{0.0f, 0.0f, 1.0f, 1.0f}));

    #ifndef CORRADE_MSVC2015_COMPATIBILITY /* Apparently dereferencing pointer is verboten */
    constexpr
    #endif
    Float b = *a.data();
    CORRADE_COMPARE(b, 1.0f);
}

void FrustumTest::compare() {
    Frustum a{
        {-1.0f,  2.0f, -3.0f, 0.1f},
        { 1.0f, -2.0f,  3.0f, 0.2f},
        {-4.0f,  5.0f, -6.0f, 0.3f},
        { 4.0f, -5.0f,  6.0f, 0.4f},
        {-7.0f,  8.0f, -9.0f, 0.5f},
        { 7.0f,  8.0f,  9.0f, 0.6f}};

    Frustum b{
        {-1.0f,  2.0f, -3.0f, 0.1f},
        { 1.0f, -2.0f,  3.0f, 0.2f},
        {-4.0f,  5.0f, -6.0f, 0.3f + Math::TypeTraits<Float>::epsilon()/2.0f},
        { 4.0f, -5.0f,  6.0f, 0.4f},
        {-7.0f,  8.0f, -9.0f, 0.5f},
        { 7.0f,  8.0f,  9.0f, 0.6f}};

    Frustum c{
        {-1.0f,  2.0f, -3.0f, 0.1f},
        { 1.0f, -2.0f,  3.0f, 0.2f},
        {-4.0f,  5.0f, -6.0f, 0.3f},
        { 4.0f, -5.0f,  6.0f, 0.4f},
        {-7.0f,  8.0f, -9.0f, 0.5f + Math::TypeTraits<Float>::epsilon()*2.0f},
        { 7.0f,  8.0f,  9.0f, 0.6f}};

    CORRADE_VERIFY(a == a);
    CORRADE_VERIFY(a == b);
    CORRADE_VERIFY(a != c);
}

void FrustumTest::debug() {
    Frustum frustum{
        {-1.0f,  2.0f, -3.0f, 0.1f},
        { 1.0f, -2.0f,  3.0f, 0.2f},
        {-4.0f,  5.0f, -6.0f, 0.3f},
        { 4.0f, -5.0f,  6.0f, 0.4f},
        {-7.0f,  8.0f, -9.0f, 0.5f},
        { 7.0f, -8.0f,  9.0f, 0.6f}};

    std::ostringstream out;
    Debug{&out} << frustum;
    CORRADE_COMPARE(out.str(), "Frustum({-1, 2, -3, 0.1},\n"
                               "        {1, -2, 3, 0.2},\n"
                               "        {-4, 5, -6, 0.3},\n"
                               "        {4, -5, 6, 0.4},\n"
                               "        {-7, 8, -9, 0.5},\n"
                               "        {7, -8, 9, 0.6})\n");
}

}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::FrustumTest)
