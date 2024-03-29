#include <vector>
#include <string>
#include <cmath>
#include "shared.h"
#include "SOLE.h"

using namespace std;

vector<TT> f(const vector<TT> &x) {
    // robertson
    return {
            -0.04 * x[0] + pow(10, 4) * x[1] * x[2],
            0.04 * x[0] - pow(10, 4) * x[1] * x[2] - 3.0 * pow(10, 7) * pow(x[1], 2),
            3.0 * pow(10, 7) * pow(x[1], 2)
    };
    // pendulum
//    return {x[1], -x[0]};
    // 3d
//    return {
//            x[0] - x[1] + x[2],
//            x[0] + x[1] - x[2],
//            2 * x[2] - x[1]
//    };
}

vector<TT> calcMethod(const size_t dim, const string &method, const vector<TT> &x0, const vector<vector<TT>> &y) {
    vector<TT> ans(dim);

    if (method == "Euler") {
        for (size_t i = 0; i < dim; ++i) {
            ans[i] = x0[i] - y[0][i] - step * f(x0)[i];
        }
    }

    if (method == "Symmetric")
        for (size_t i = 0; i < dim; ++i) {
            ans[i] = x0[i] - y[0][i] - step / 2 * (f(x0)[i] + f(y[0])[i]);
        }

    if (method == "BDF2") {
        for (size_t i = 0; i < dim; ++i) {
            ans[i] = x0[i] + (-4 * y[1][i] + y[0][i] - 2 * step * f(x0)[i]) / 3.0;
        }
    }

    if (method == "BDF4") {
        for (size_t i = 0; i < dim; ++i) {
            ans[i] = x0[i] + (-48 * y[3][i] + 36 * y[2][i] - 16 * y[1][i] + 3 * y[0][i]
                              - 12 * step * f(x0)[i]) / 25.0;
        }
    }


    if (method == "implicit2Adams") {
        for (size_t i = 0; i < dim; ++i) {
            ans[i] = x0[i] - y[1][i] - step * (
                    5 * f(x0)[i] + 8 * f(y[1])[i] - f(y[0])[i]) / 12.0;
        }
    }

    if (method == "implicitAdams") {
        for (size_t i = 0; i < dim; ++i) {
            ans[i] = x0[i] - y[3][i] - step * (
                    251 * f(x0)[i] + 646 * f(y[3])[i] - 264 * f(y[2])[i] + 106 * f(y[1])[i] -
                    19 * f(y[0])[i]) / 720.0;
        }
    }
    return ans;
}

// Матрица Якоби
vector<vector<TT>>
Jacobi_matr(size_t const dim, const string &method, const vector<TT> &x0, const vector<vector<TT>> &y) {
    vector<vector<TT>> Jacobi(dim, vector<TT>(dim));
    vector<TT> fx = calcMethod(dim, method, x0, y);
    vector<TT> x0Vect(dim);
    vector<TT> xDiffVect(dim);

    for (size_t i = 0; i < dim; ++i) {
        x0Vect = x0;
        x0Vect[i] += COMPARE_RATE;
        xDiffVect = calcMethod(dim, method, x0Vect, y);
        for (size_t j = 0; j < dim; ++j) {
            Jacobi[j][i] = (xDiffVect[j] - fx[j]) / COMPARE_RATE;
        }
    }
    return Jacobi;
}


// Метод Ньютона
vector<TT> Newton(const string &method, const size_t dim, const vector<vector<TT>> &y) {
    vector<vector<TT>> Jacobi(dim, vector<TT>(dim));
    vector<TT> x(dim);
    vector<TT> xk(dim);

    do {
        x = xk;
        Jacobi = Jacobi_matr(dim, method, x, y);
        Jacobi = inverseMatrix(Jacobi);
        xk = vectorMatrixMultiplication(Jacobi, calcMethod(dim, method, xk, y));
        xk = vectorOperation(x, xk, '-');
    } while (norm1Vector(vectorOperation(x, xk, '-')) > COMPARE_RATE);
    return xk;
}
