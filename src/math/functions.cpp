
#include <iostream>
#include <cassert>
#include <list>

#include "math/functions.h"
#include "math/vector.h"
//#include "math/solvequartic.h"

/**
 * The binomial coefficient is defined as 
 * \f[ {n \choose k} = \frac{n!}{k!(n-k)!} \quad \mbox{if } n\geq k\geq 0 \qquad \f]
 * \f[ {n \choose k} = 0 \quad \mbox{if } k<0 \mbox{ or } k>n. \f]
 * 
 * Implemented as a slow recursive function.
 *
 * @see http://www.brpreiss.com/books/opus4/html/page467.html for a O(n^2) version.
 */
unsigned long Math::binomialCoefficient(long n, long k) {
    if (k == 0 || k == n) {
	return 1;
    } else if (k < 0 || k > n) {
	return 0;
    } else {
        return binomialCoefficient(n-1,k-1) + binomialCoefficient(n-1,k);
    }
}

/**
 * The Bernstein polynomial is defined as
 *
 * \f[ B_{i,n}(t) = {n \choose i} t^i (1-t)^{n-i} \quad \mbox{for } t \in {[0,1]} \qquad \f]
 * 
 */
double Math::bernsteinPolynomial(unsigned int i, unsigned int n, double t) {
    return binomialCoefficient(n,i) * pow(t,i) * pow((1-t),n-i);
}

/**
 * Solves the quartic equation
 *
 * \f[ x^4 + Ax^3 + Bx^2 + Cx + D = 0 \f]
 * 
 * The returned roots are sorted and duplicates removed.
 * 
 * @param A, B, C, D real coefficients of the equation above.
 * @param roots an array of four doubles where the roots are stored
 * @return the number of real roots
 *
 * @see http://www.magic-software.com/ for method used.
 */
int Math::solveQuartic(double A, double B, double C, double D, double* roots) {
    double a = -B;
    double b = A*C - 4*D;
    double c = 4*B*D - C*C - A*A*D;
    double cubic_roots[3];
    solveCubic(a,b,c,cubic_roots);
    double y = cubic_roots[0];
    double R = (A*A/4) - B + y;
    if (R < 0) 
	return 0;
    R = sqrt(R);
    double D2,E2;
    int num = 0;
    if (IS_ZERO(R)) {
	D2 = (3*A*A/4) - (2*B) + (2*sqrt(y*y - 4*D));
	E2 = (3*A*A/4) - (2*B) - (2*sqrt(y*y - 4*D));
    } else {
	D2 = 3*A*A/4 - R*R - 2*B + (4*A*B - 8*C - A*A*A)/(4*R);
	E2 = 3*A*A/4 - R*R - 2*B - (4*A*B - 8*C - A*A*A)/(4*R);
    }
    if (D2 >= 0) {
	D2 = sqrt(D2);
	roots[num++] = -(A/4) + (R/2) + (D2/2);
	roots[num++] = -(A/4) + (R/2) - (D2/2);
    }
    if (E2 >= 0) {
	E2 = sqrt(E2);
	roots[num++] = -(A/4) - (R/2) + (E2/2);
	roots[num++] = -(A/4) - (R/2) - (E2/2);
    }

    if (num < 2) 
	return num;

    // Prune and sort
    std::list<double> L;
    for(int i = 0; i < num; i++) {
	L.push_back(roots[i]);
    }
    L.sort();
    L.unique();
    int i = 0;
    for (std::list<double>::iterator ite = L.begin(); ite != L.end(); ite++) {
	roots[i++] = *ite;
    }
    return L.size();
}

/**
 * Solves the quartic equation
 *
 * \f[ x^4 + Ax^3 + Bx^2 + Cx + D = 0 \f]
 * 
 * This method is invalid!
 *
 * @param A, B, C, D real coefficients of the equation above.
 * @param roots an array of four doubles where the roots are stored
 * @return the number of real roots
 * @see http://mathworld.wolfram.com/QuarticEquation.html equations (32) and (33) for method used. Which is the same as Schaum, p.33.
 */
int Math::solveQuartic_Schaum(double A, double B, double C, double D, double* roots) {
    double a = -B;
    double b = A*C - 4*D;
    double c = 4*B*D - C*C - A*A*D;
    double cubic_roots[3];
    std::cout << "a b c = " << a << ", " << b << ", " << c << std::endl;

    int n = solveCubic(a,b,c,cubic_roots);
    std::cout << "n = " << n << std::endl;
    double y = cubic_roots[0];
    std::cout << "y = " << y << std::endl;

    std::cout << " sqrt1^2 = " << A*A - 4*B + 4*y << std::endl;
    double sqrt1 = sqrt(A*A - 4*B + 4*y);
    std::cout << " sqrt1 = " << sqrt1 << std::endl;
    std::cout << " sqrt2^2 = " << y*y - 4*D << std::endl;
    double sqrt2 = sqrt(y*y - 4*D);
    std::cout << " sqrt2 = " << sqrt2 << std::endl;

    int num1 = solveQuadratic(1,(A + sqrt1)/2.0,(y - sqrt2)/2.0,roots);
    std::cout << "num1 = " << num1 << std::endl;
    std::cout << "root1 = " << roots[0] << std::endl;
    std::cout << "root2 = " << roots[1] << std::endl;
    int num2 = solveQuadratic(1,(A - sqrt1)/2.0,(y + sqrt2)/2.0,&(roots[num1]));
    std::cout << "num2 = " << num2 << std::endl;
    std::cout << "root1 = " << roots[2] << std::endl;
    std::cout << "root2 = " << roots[3] << std::endl;

    // prune duplicate roots
    if (num1 == 1 && num2 == 1) {
	if (IS_EQUAL(roots[0],roots[1])) {
	    num2 = 0;
	}
    } else if (num1 == 1 && num2 == 2) {
	if (IS_EQUAL(roots[0],roots[1] || IS_EQUAL(roots[0],roots[2]))) {
	    roots[0] = roots[2];
	    num1 = 0;
	}
    } else if (num1 == 2 && num2 == 1) {
	if (IS_EQUAL(roots[0],roots[2]) || IS_EQUAL(roots[1],roots[2])) {
	    num2 = 0;
	}
    } else if (num1 == 2 && num2 == 2) {
	if (IS_EQUAL(roots[0],roots[3]) || IS_EQUAL(roots[1],roots[3])) {
	    num2--;
	}
	if (IS_EQUAL(roots[0],roots[2]) || IS_EQUAL(roots[1],roots[2])) {
	    if (num2 == 1) {
		num2--;
	    } else {
		roots[2] = roots[3];
		num2--;
	    }
	}
    }

    return num1+num2;
}

/**
 * Solves the cubic equation
 *
 * \f[ x^3 + Ax^2 + Bx + C = 0 \f]
 *
 * Every cubic equation has at least one real root.
 *
 * 
 * @param A, B, C real coefficients of the equation above.
 * @param roots an array of three doubles where the roots are stored
 * @return the number of real roots
 * @see http://mathworld.wolfram.com/CubicEquation.html
 */
int Math::solveCubic(double A, double B, double C, double* roots) {
    double Q = (3.0 * B - A * A) / 9.0;
    double R = (9.0 * A * B - 27.0 * C - 2.0 * A * A * A) / 54.0;
    double D = Q * Q * Q + R * R;
    if (D < 0) {
	// Three real roots
	double phi = acos(R / sqrt(-(Q*Q*Q)));
	double G = 2.0 * sqrt(-Q);
	double H = A / 3.0;
	roots[0] = G * cos(phi/3.0) - H;
	roots[1] = G * cos(phi/3.0 + M_2PI/3.0) - H;
	roots[2] = G * cos(phi/3.0 + 2*M_2PI/3.0) - H;
	return 3;
    } else {
	double sqrtD = sqrt(D);
	double S = cbrt(R + sqrtD);
	double T = cbrt(R - sqrtD);
	roots[0] = S + T - A/3;
	if (IS_ZERO(D)) {
	    roots[1] = -(S+T)/2 - A/3;
	    return (roots[1] == roots[0]) ? 1 : 2;
	}
	// D > 0 gives only one real root
	return 1;
    }
}

/**
 * Solves the quadratic equation
 *
 * \f[ Ax^2 + Bx + C = 0 \f]
 * 
 * Where A != 0 or B != 0.
 *
 * @param A, B, C real coefficients of the equation above.
 * @param roots an array of two doubles where the roots are stored
 * @return the number of real roots
 */
int Math::solveQuadratic(double A, double B, double C, double* roots) {
    if (IS_ZERO(A) && !IS_ZERO(B)) {
	roots[0] = -C / B;
	return 1;
    }
    assert(!IS_ZERO(A));
    double D = B*B - 4*A*C;
    if (D < 0) {
	return 0;
    } else if (IS_EQUAL(D,0)) {
	roots[0] = -B / (2.0 * A);
	return 1;
    } else {
	double sqrtD = sqrt(D);
	roots[0] = (-B + sqrtD) / (2.0 * A);
	roots[1] = (-B - sqrtD) / (2.0 * A);
	return 2;
    }
}


