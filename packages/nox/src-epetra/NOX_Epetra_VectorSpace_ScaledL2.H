// $Id$
// $Source$

//@HEADER
// ************************************************************************
//
//            NOX: An Object-Oriented Nonlinear Solver Package
//                 Copyright (2002) Sandia Corporation
//
// Under terms of Contract DE-AC04-94AL85000, there is a non-exclusive
// license for use of this work by or on behalf of the U.S. Government.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the Corporation nor the names of the
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY SANDIA CORPORATION "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SANDIA CORPORATION OR THE
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Questions? Contact Roger Pawlowski (rppawlo@sandia.gov) or
// Eric Phipps (etphipp@sandia.gov), Sandia National Laboratories.
// ************************************************************************
//  CVS Information
//  $Source$
//  $Author$
//  $Date$
//  $Revision$
// ************************************************************************
//@HEADER

#ifndef NOX_EPETRA_VECTORSPACE_SCALEDL2_H
#define NOX_EPETRA_VECTORSPACE_SCALEDL2_H

#include "NOX_Epetra_VectorSpace.H" // base class
#include "Teuchos_RCP.hpp"
#include "NOX_Epetra_Scaling.H"

// Forward declarations
class Epetra_Vector;

namespace NOX {

namespace Epetra {

  /*! \brief Concrete class for a weighted L2 vector space.

  This class allows users to override the inner product and norm
  used by the NOX::Epetra::Vector class.  The most frequent use of
  this class is for introducing a weighted norm throughout NOX.

  */
  class VectorSpaceScaledL2 : public NOX::Epetra::VectorSpace {

  public:

    //! Constructor
    VectorSpaceScaledL2(const Teuchos::RCP<NOX::Epetra::Scaling>& s,
            NOX::Epetra::Scaling::ScaleType st =
            NOX::Epetra::Scaling::Left);

    //! Destructor
    virtual ~VectorSpaceScaledL2();

    //! Computes a scaled inner product.
    /*! Computes a scaled inner product: \f$ <Da, Db> \f$ where
      \f$D\f$ is the set of scaling vectors associated with either
      left of right scaling.
    */
    virtual double innerProduct(const Epetra_Vector& a,
                const Epetra_Vector& b) const;

    //! Computes the scaled norm.
    /*! Computes the scaled norm using \f$ Da \f$ where \f$D\f$ is the
      set of scaling vectors associated with either left of right
      scaling.
    */
    virtual double norm(const Epetra_Vector& a,
            NOX::Abstract::Vector::NormType =
            NOX::Abstract::Vector::TwoNorm) const;

  protected:

    //! Scaling vector used in the inner product
    Teuchos::RCP<NOX::Epetra::Scaling> scalingPtr;

    //! Scaling type to apply to vector space
    NOX::Epetra::Scaling::ScaleType scaleType;

    //! Temporary vector used in scaling computations
    mutable Teuchos::RCP<Epetra_Vector> tmpVectorPtr;

  };
} // namespace Epetra
} // namespace NOX

#endif
