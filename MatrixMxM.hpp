#ifndef _ARDUINO3D_MATRIXM_HPP_
#define _ARDUINO3D_MATRIXM_HPP_

namespace Array
{
/// M x M matrix class
template<typename MyScalar, DimensionType kDimension> class MatrixMxM : public ArrayM<MyScalar, kDimension*kDimension>
{
    private:
        typedef ArrayM<MyScalar, kDimension*kDimension> MyArray;
        typedef MatrixMxM<MyScalar, kDimension> MyMatrix;
        typedef MatrixMxM<MyScalar, kDimension> MyVector;
        //~ typedef MatrixMxN<T, N, M> MyTranspose;
        //~ typedef MatrixMxN<T, N, M> MySquare;

    public:
        MatrixMxM() : MyArray() {}
        MatrixMxM(const MyScalar& val) : MyArray(val) {}
        MatrixMxM(const MyScalar* init) : MyArray(init) {}
        MatrixMxM(const MyMatrix& other) : MyArray(other) {}

        /// Matrix assignment multiplication
        MyMatrix& operator*=(const MyMatrix& other)
        {
            *this = *this * other;
            return *this;
        }

        /// Matrix multiplication
        MyMatrix operator*(const MyMatrix& other)
        {
            MyMatrix res;
            for (DimensionType row = 0; row < kDimension; ++row)
            {
                for (DimensionType col = 0; col < kDimension; ++col)
                {
                    for (DimensionType i = 0; i < kDimension; ++i)
                    {
                        // Accumulate
                        res[row*kDimension + col] += (*this)[row*kDimension + i] * other[i*kDimension + col];
                    }
                }
            }
            return res;
        }

        MyScalar& operator()(const DimensionType row, const DimensionType col)
        {
            return (*this)[row*kDimension + col];
        }

        MyScalar operator()(const DimensionType row, const DimensionType col) const
        {
            return (*this)[row*kDimension + col];
        }

        //~ MyScalar& operator[](const DimensionType pos)
        //~ {
            //~ return this->values[pos];
        //~ }
//~
        //~ MyScalar operator[](const DimensionType pos) const
        //~ {
            //~ return this->values[pos];
        //~ }
    private:

};
} // namespace Array
#endif
