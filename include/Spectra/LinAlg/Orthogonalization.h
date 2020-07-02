// Copyright (C) 2020 Netherlands eScience Center <f.zapata@esciencecenter.nl>
//
// This Source Code Form is subject to the terms of the Mozilla
// Public License v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef SPECTRA_ORTHOGONALIZATION_H
#define SPECTRA_ORTHOGONALIZATION_H

#include <Eigen/Dense>

namespace Spectra {

/// Check if the number of column to skip is
/// larger than 0 but smaller than the total number
/// of column of the matrix
/// \param in_output matrix to be orthogonalized
/// \param leftColsToSkip Number of left columns to be left untouched
template <typename Matrix>
void assert_leftColsToSkip(Matrix& in_output, Eigen::Index leftColsToSkip)
{
    assert(in_output.cols() > leftColsToSkip && "leftColsToSkip is larger than columns of matrix");
    assert(leftColsToSkip >= 0 && "leftColsToSkip is negative");
}

/// if the the number of col to skip is null
/// normalize the first column and set lefColsToSkip=1
/// \param in_output matrix to be orthogonalized
/// \param leftColsToSkip Number of left columns to be left untouched
/// \return Eigen::Index index equals to 1
template <typename Matrix>
Eigen::Index treatFirstCol(Matrix& in_output, Eigen::Index leftColsToSkip)
{
    if (leftColsToSkip == 0)
    {
        in_output.col(0).normalize();
        leftColsToSkip = 1;
    }
    return leftColsToSkip;
}

/// Orthogonalize the in_output matrix using a QR decomposition
/// \param in_output matrix to be orthogonalized
/// \param leftColsToSkip Number of left columns to be left untouched
template <typename Matrix>
void QR_orthogonalisation(Matrix& in_output)
{
    using InternalMatrix = Eigen::Matrix<typename Matrix::Scalar, Eigen::Dynamic, Eigen::Dynamic>;
    Eigen::Index nrows = in_output.rows();
    Eigen::Index ncols = in_output.cols();
    ncols = std::min(nrows, ncols);
    InternalMatrix I = InternalMatrix::Identity(nrows, ncols);
    Eigen::HouseholderQR<Matrix> qr(in_output);
    in_output = qr.householderQ() * I;
}

/// Orthogonalize the in_output matrix using a
/// modified Gram Schmidt process
/// \param in_output matrix to be orthogonalized
/// \param leftColsToSkip Number of left columns to be left untouched
template <typename Matrix>
void MGS_orthogonalisation(Matrix& in_output, Eigen::Index leftColsToSkip = 0)
{
    assert_leftColsToSkip(in_output, leftColsToSkip);
    leftColsToSkip = treatFirstCol(in_output, leftColsToSkip);

    for (Eigen::Index k = leftColsToSkip; k < in_output.cols(); ++k)
    {
        for (Eigen::Index j = 0; j < k; j++)
        {
            in_output.col(k) -= in_output.col(j).dot(in_output.col(k)) * in_output.col(j);
        }
        in_output.col(k).normalize();
    }
}

/// Orthogonalize the in_output matrix using a
/// Gram Schmidt process
/// \param in_output matrix to be orthogonalized
/// \param leftColsToSkip Number of left columns to be left untouched
template <typename Matrix>
void GS_orthogonalisation(Matrix& in_output, Eigen::Index leftColsToSkip = 0)
{
    assert_leftColsToSkip(in_output, leftColsToSkip);
    leftColsToSkip = treatFirstCol(in_output, leftColsToSkip);

    for (Eigen::Index j = leftColsToSkip; j < in_output.cols(); ++j)
    {
        in_output.col(j) -= in_output.leftCols(j) * (in_output.leftCols(j).transpose() * in_output.col(j));
        in_output.col(j).normalize();
    }
}

/// Orthogonalize the in_output matrix using a
/// twice-is-enough Gram Schmidt process
/// \param in_output matrix to be orthogonalized
/// \param leftColsToSkip Number of left columns to be left untouched
template <typename Matrix>
void twice_is_enough_orthogonalisation(Matrix& in_output, Eigen::Index leftColsToSkip = 0)
{
    GS_orthogonalisation(in_output, leftColsToSkip);
    GS_orthogonalisation(in_output, leftColsToSkip);
}

/// Orthogonalize the right cols of in_output against the left cols
/// It assumes that the right cols are already orthogonal
/// and it does not orthogonalize the left cols against each other
/// \param in_output matrix to be orthogonalized
/// \param leftColsToSkip Number of left columns to be left untouched
template <typename Matrix>
void partial_orthogonalisation(Matrix& in_output, Eigen::Index leftColsToSkip)
{
    assert_leftColsToSkip(in_output, leftColsToSkip);
    if (leftColsToSkip == 0)
    {
        return;
    }

    Eigen::Index rightColToOrtho = in_output.cols() - leftColsToSkip;
    in_output.rightCols(rightColToOrtho) -= in_output.leftCols(leftColsToSkip) * (in_output.leftCols(leftColsToSkip).transpose() * in_output.rightCols(rightColToOrtho));
    in_output.rightCols(rightColToOrtho).colwise().normalize();
}

/// Orthogonalize the in_output matrix using a
/// Jens process. The right cols are first orthogonalized
/// agains the leftc cols and then a QR is applied on the right cols
/// to make them orthogonalized agains each other
/// \param in_output matrix to be orthogonalized
/// \param leftColsToSkip Number of left columns to be left untouched
template <typename Matrix>
void JensWehner_orthogonalisation(Matrix& in_output, Eigen::Index leftColsToSkip = 0)
{
    assert_leftColsToSkip(in_output, leftColsToSkip);

    Eigen::Index rightColToOrtho = in_output.cols() - leftColsToSkip;
    partial_orthogonalisation(in_output, leftColsToSkip);
    Eigen::Ref<Matrix> right_cols = in_output.rightCols(rightColToOrtho);
    QR_orthogonalisation(right_cols);
    in_output.rightCols(rightColToOrtho) = right_cols;
}

}  // namespace Spectra

#endif  //SPECTRA_ORTHOGONALIZATION_H