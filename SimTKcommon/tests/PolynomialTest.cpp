/* -------------------------------------------------------------------------- *
 *                       Simbody(tm): SimTKcommon                             *
 * -------------------------------------------------------------------------- *
 * This is part of the SimTK biosimulation toolkit originating from           *
 * Simbios, the NIH National Center for Physics-Based Simulation of           *
 * Biological Structures at Stanford, funded under the NIH Roadmap for        *
 * Medical Research, grant U54 GM072970. See https://simtk.org/home/simbody.  *
 *                                                                            *
 * Portions copyright (c) 2007-12 Stanford University and the Authors.        *
 * Authors: Peter Eastman                                                     *
 * Contributors:                                                              *
 *                                                                            *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may    *
 * not use this file except in compliance with the License. You may obtain a  *
 * copy of the License at http://www.apache.org/licenses/LICENSE-2.0.         *
 *                                                                            *
 * Unless required by applicable law or agreed to in writing, software        *
 * distributed under the License is distributed on an "AS IS" BASIS,          *
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.   *
 * See the License for the specific language governing permissions and        *
 * limitations under the License.                                             *
 * -------------------------------------------------------------------------- */

#include "SimTKcommon.h"

#include <iostream>

#define ASSERT(cond) {SimTK_ASSERT_ALWAYS(cond, "Assertion failed");}

using std::cout;
using std::endl;
using namespace SimTK;

/**
 * Determine if two complex numbers are equal to within the specified tolerance.
 */

bool equal(Complex expected, Complex found, Real tol) {
    Real diffr = std::abs(expected.real()-found.real());
    Real diffi = std::abs(expected.imag()-found.imag());
    Real scaler = std::max(std::abs(expected.real()), std::abs(found.real()));
    Real scalei = std::max(std::abs(expected.imag()), std::abs(found.imag()));
    if (expected.real() == 0.0)
        scaler = 1.0;
    if (expected.imag() == 0.0)
        scalei = 1.0;
    return (diffr < tol*scaler && diffi < tol*scalei);
}

/**
 * Determine if two roots of a quadratic polynomial are equal.  This is a more stringent test then the above routine,
 * since the quadratic solver has very high accuracy.
 */

bool equal2(Complex expected, Complex found) {
    if (expected.imag() == 0.0 && found.imag() != 0.0)
        return false; // If we expect a real number, require the number found to be precisely real as well.
    return equal(expected, found, std::sqrt(NTraits<Real>::getEps()));
}

/**
 * Find the roots of the specified polynomial with real coefficients, and compare them to the expected ones.
 */

void testQuadratic(Vec3 coeff, Vec<2,Complex> expected) {
    Vec<2,Complex> found;
    PolynomialRootFinder::findRoots(coeff, found);
    ASSERT((equal2(expected[0], found[0]) && equal2(expected[1], found[1])) || (equal2(expected[0], found[1]) && equal2(expected[1], found[0])))
}

/**
 * Find the roots of the specified polynomial with complex coefficients, and compare them to the expected ones.
 */

void testQuadratic(Vec<3,Complex> coeff, Vec<2,Complex> expected) {
    Vec<2,Complex> found;
    PolynomialRootFinder::findRoots(coeff, found);
    ASSERT((equal2(expected[0], found[0]) && equal2(expected[1], found[1])) || (equal2(expected[0], found[1]) && equal2(expected[1], found[0])))
}

/**
 * Generate a polynomial that has the specified roots, then solve it and compare.
 */

void testQuadraticRealRoots(Vec2 roots) {
    static Random::Gaussian random(0.0, 100.0);
    Vec3 coeff(1.0, -roots[0]-roots[1], roots[0]*roots[1]);
    coeff *= random.getValue();
    testQuadratic(coeff, Vec<2,Complex>(roots[0], roots[1]));
}

/**
 * Generate a polynomial whose roots are a complex conjugate pair, then solve it and compare.
 */

void testQuadraticConjugateRoots(Complex root) {
    static Random::Gaussian random(0.0, 1000.0);
    Vec3 coeff(1.0, -2.0*root.real(), norm(root));
    coeff *= random.getValue();
    testQuadratic(coeff, Vec<2,Complex>(root, conj(root)));
}

/**
 * Generate a polynomial that has the specified roots, then solve it and compare.
 */

void testQuadraticComplexRoots(Vec<2,Complex>roots) {
    static Random::Gaussian random(0.0, 100.0);
    Vec<3,Complex> coeff(1.0, -roots[0]-roots[1], roots[0]*roots[1]);
    coeff *= Complex(random.getValue(), random.getValue());
    testQuadratic(coeff, roots);
}

/**
 * Find the roots of the specified polynomial with real coefficients, and compare them to the expected ones.
 */

void testCubic(Vec4 coeff, Vec<3,Complex> expected) {
    Vec<3,Complex> found;
    PolynomialRootFinder::findRoots(coeff, found);
    Real tol = 1e-4;
    if (equal(expected[0], found[0], tol))
        ASSERT((equal(expected[1], found[1], tol) && equal(expected[2], found[2], tol)) || (equal(expected[1], found[2], tol) && equal(expected[2], found[1], tol)))
    else if (equal(expected[0], found[1], tol))
        ASSERT((equal(expected[1], found[2], tol) && equal(expected[2], found[0], tol)) || (equal(expected[1], found[0], tol) && equal(expected[2], found[2], tol)))
    else if (equal(expected[0], found[2], tol))
        ASSERT((equal(expected[1], found[0], tol) && equal(expected[2], found[1], tol)) || (equal(expected[1], found[1], tol) && equal(expected[2], found[0], tol)))
    else
        ASSERT(false)
}

/**
 * Find the roots of the specified polynomial with complex coefficients, and compare them to the expected ones.
 */

void testCubic(Vec<4,Complex> coeff, Vec<3,Complex> expected) {
    Vec<3,Complex> found;
    PolynomialRootFinder::findRoots(coeff, found);
    Real tol = 1e-4;
    if (equal(expected[0], found[0], tol))
        ASSERT((equal(expected[1], found[1], tol) && equal(expected[2], found[2], tol)) || (equal(expected[1], found[2], tol) && equal(expected[2], found[1], tol)))
    else if (equal(expected[0], found[1], tol))
        ASSERT((equal(expected[1], found[2], tol) && equal(expected[2], found[0], tol)) || (equal(expected[1], found[0], tol) && equal(expected[2], found[2], tol)))
    else if (equal(expected[0], found[2], tol))
        ASSERT((equal(expected[1], found[0], tol) && equal(expected[2], found[1], tol)) || (equal(expected[1], found[1], tol) && equal(expected[2], found[0], tol)))
    else
        ASSERT(false)
}

/**
 * Generate a polynomial that has the specified roots, then solve it and compare.
 */

void testCubicRealRoots(Vec3 roots) {
    static Random::Gaussian random(0.0, 100.0);
    Vec4 coeff(1.0, -roots[0]-roots[1]-roots[2], roots[0]*roots[1]+roots[1]*roots[2]+roots[2]*roots[0], -roots[0]*roots[1]*roots[2]);
    coeff *= random.getValue();
    testCubic(coeff, Vec<3,Complex>(roots[0], roots[1], roots[2]));
}

/**
 * Generate a polynomial whose roots are a complex conjugate pair plus a single real root, then solve it and compare.
 */

void testCubicConjugateRoots(Complex root1, Real root2) {
    static Random::Gaussian random(0.0, 1000.0);
    Vec4 coeff(1.0, -2.0*root1.real()-root2, norm(root1)+2.0*root1.real()*root2,-norm(root1)*root2);
    coeff *= random.getValue();
    testCubic(coeff, Vec<3,Complex>(root1, conj(root1), root2));
}

/**
 * Generate a polynomial that has the specified roots, then solve it and compare.
 */

void testCubicComplexRoots(Vec<3,Complex> roots) {
    static Random::Gaussian random(0.0, 100.0);
    Vec<4,Complex> coeff(1.0, -roots[0]-roots[1]-roots[2], roots[0]*roots[1]+roots[1]*roots[2]+roots[2]*roots[0], -roots[0]*roots[1]*roots[2]);
    coeff *= random.getValue();
    testCubic(coeff, Vec<3,Complex>(roots[0], roots[1], roots[2]));
}

/**
 * Given a polynomial and a list of roots, verify that they really are roots.
 */

void verifyRoots(Vector_<Real>& coefficients, Vector_<Complex>& roots) {
    for (int i = 0; i < roots.size(); ++i) {
        Complex sum = 0.0;
        for (int j = 0; j < coefficients.size(); ++j) {
            sum = sum*roots[i]+coefficients[j];
        }
        ASSERT(equal(0.0, sum, 1e-2))
    }
}

/**
 * Given a polynomial and a list of roots, verify that they really are roots.
 */

void verifyRoots(Vector_<Complex>& coefficients, Vector_<Complex>& roots) {
    for (int i = 0; i < roots.size(); ++i) {
        Complex sum = 0.0;
        for (int j = 0; j < coefficients.size(); ++j) {
            sum = sum*roots[i]+coefficients[j];
        }
        ASSERT(equal(0.0, sum, 1e-2))
    }
}

/**
 * Perform a variety of tests solving quadratic polynomials.
 */

void testSolveQuadratic() {
    
    // Try a few fixed tests.
    
    testQuadraticRealRoots(Vec2(0.0, 0.0));
    testQuadraticRealRoots(Vec2(1.0, 1.0));
    testQuadraticRealRoots(Vec2(0.0, 5.0));
    testQuadraticRealRoots(Vec2(10.0, -5.0));
    testQuadratic(Vec3(1.0, 0.0, 0.0), Vec<2,Complex>(0.0, 0.0));

    // Try cases with a single, repeated real root.
    
    static Random::Gaussian random(0.0, 100.0);
    for (int i = 0; i < 1000; ++i) {
        Real root = random.getValue();
        testQuadraticRealRoots(Vec2(root, root));
    }
    
    // Try cases with two distinct real roots.
    
    for (int i = 0; i < 1000; ++i)
        testQuadraticRealRoots(Vec2(random.getValue(), random.getValue()));
    
    // Try cases whose roots are complex conjugates.
    
    for (int i = 0; i < 1000; ++i)
        testQuadraticConjugateRoots(Complex(random.getValue(), random.getValue()));
    
    // Try cases with two distinct complex roots.
    
    for (int i = 0; i < 1000; ++i)
        testQuadraticComplexRoots(Vec<2,Complex>(Complex(random.getValue(), random.getValue()), Complex(random.getValue(), random.getValue())));
    
    // Verify that if the leading coefficient is zero, it throws an exception.
    
    try {
        testQuadratic(Vec3(0.0, 1.0, 1.0), Vec<2,Complex>(0.0, 0.0));
        ASSERT(false)
    }
    catch (PolynomialRootFinder::ZeroLeadingCoefficient ex) {
    }
    try {
        testQuadratic(Vec<3,Complex>(0.0, 1.0, 1.0), Vec<2,Complex>(0.0, 0.0));
        ASSERT(false)
    }
    catch (PolynomialRootFinder::ZeroLeadingCoefficient ex) {
    }
}

/**
 * Perform a variety of tests solving cubic polynomials.
 */

void testSolveCubic() {

    // Try a few fixed tests.
    
    testCubicRealRoots(Vec3(0.0, 0.0, 0.0));
    testCubicRealRoots(Vec3(1.0, 1.0, 1.0));
    testCubicRealRoots(Vec3(0.0, 5.0, 5.0));
    testCubicRealRoots(Vec3(10.0, -5.0, 100.0));
    testCubic(Vec4(1.0, 0.0, 0.0, 0.0), Vec<3,Complex>(0.0, 0.0, 0.0));

    // Try cases with a single, repeated real root.
    
    static Random::Gaussian random(0.0, 100.0);
    for (int i = 0; i < 1000; ++i) {
        Real root = random.getValue();
        testCubicRealRoots(Vec3(root, root, root));
    }
    
    // Try cases with two distinct real roots.
    
    for (int i = 0; i < 1000; ++i) {
        Real root = random.getValue();
        testCubicRealRoots(Vec3(root, root, random.getValue()));
    }
    
    // Try cases with three distinct real roots.
    
    for (int i = 0; i < 1000; ++i)
        testCubicRealRoots(Vec3(random.getValue(), random.getValue(), random.getValue()));
    
    // Try cases whose roots include a complex conjugates pair.
    
    for (int i = 0; i < 1000; ++i)
        testCubicConjugateRoots(Complex(random.getValue(), random.getValue()), random.getValue());
    
    // Try cases with three distinct complex roots.
    
    for (int i = 0; i < 1000; ++i)
        testCubicComplexRoots(Vec<3,Complex>(Complex(random.getValue(), random.getValue()), Complex(random.getValue(), random.getValue()), Complex(random.getValue(), random.getValue())));
    
    // Verify that if the leading coefficient is zero, it throws an exception.
    
    try {
        testCubic(Vec4(0.0, 0.0, 0.0, 0.0), Vec<3,Complex>(0.0, 0.0, 0.0));
        ASSERT(false)
    }
    catch (PolynomialRootFinder::ZeroLeadingCoefficient ex) {
    }
    try {
        testCubic(Vec<4,Complex>(0.0, 0.0, 0.0, 0.0), Vec<3,Complex>(0.0, 0.0, 0.0));
        ASSERT(false)
    }
    catch (PolynomialRootFinder::ZeroLeadingCoefficient ex) {
    }
}

/**
 * Test solving polynomials of arbitrary degree.
 */

void testSolveArbitrary() {
    static Random::Uniform uniform(2.0, 7.0);
    static Random::Gaussian random(0.0, 100.0);
    
    // Test random polynomials of various degrees with real coefficients.
    
    Vector realCoeff;
    Vector_<Complex> roots;
    for (int i = 0; i < 1000; ++i) {
        int length = uniform.getIntValue();
        realCoeff.resize(length);
        roots.resize(length-1);
        for (int j = 0; j < length; ++j)
            realCoeff[j] = random.getValue();
        PolynomialRootFinder::findRoots(realCoeff, roots);
        verifyRoots(realCoeff, roots);
    }
    
    // Test random polynomials of various degrees with complex coefficients.
    
    Vector_<Complex> complexCoeff;
    for (int i = 0; i < 1000; ++i) {
        int length = uniform.getIntValue();
        complexCoeff.resize(length);
        roots.resize(length-1);
        for (int j = 0; j < length; ++j)
            complexCoeff[j] = Complex(random.getValue(), random.getValue());
        PolynomialRootFinder::findRoots(complexCoeff, roots);
        verifyRoots(complexCoeff, roots);
    }
    
    // Verify that if the leading coefficient is zero, it throws an exception.
    
    try {
        realCoeff[0] = 0.0;
        PolynomialRootFinder::findRoots(realCoeff, roots);
        ASSERT(false)
    }
    catch (PolynomialRootFinder::ZeroLeadingCoefficient ex) {
    }
    try {
        complexCoeff[0] = 0.0;
        PolynomialRootFinder::findRoots(complexCoeff, roots);
        ASSERT(false)
    }
    catch (PolynomialRootFinder::ZeroLeadingCoefficient ex) {
    }
}

int main() {
    try {
        testSolveQuadratic();
        testSolveCubic();
        testSolveArbitrary();
    } catch(const std::exception& e) {
        cout << "exception: " << e.what() << endl;
        return 1;
    }
    return 0;
}


