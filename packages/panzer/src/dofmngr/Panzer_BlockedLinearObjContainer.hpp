// @HEADER
// ***********************************************************************
//
//           Panzer: A partial differential equation assembly
//       engine for strongly coupled complex multiphysics systems
//                 Copyright (2011) Sandia Corporation
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
// Questions? Contact Roger P. Pawlowski (rppawlo@sandia.gov) and
// Eric C. Cyr (eccyr@sandia.gov)
// ***********************************************************************
// @HEADER

#ifndef __Panzer_BlockedLinearObjContainer_hpp__
#define __Panzer_BlockedLinearObjContainer_hpp__

#include "Panzer_config.hpp"

#include "Teuchos_RCP.hpp"
#include "Panzer_LinearObjContainer.hpp"

#include "Thyra_PhysicallyBlockedLinearOpBase.hpp"
#include "Thyra_ProductVectorBase.hpp"

#include <boost/unordered_map.hpp>

namespace panzer {

/** Linear object container for Block operators, this
  * always assumes the matrix is square.
  */
template <typename BaseContainerType>
class BlockedLinearObjContainer : public LinearObjContainer {
public:
   typedef Thyra::VectorBase<double> VectorType;
   typedef Thyra::LinearOpBase<double> CrsMatrixType;

   //! Make sure row and column spaces match up 
   bool checkCompatibility() const;

   virtual void initialize();
   virtual void clear();

   inline void set_x(const Teuchos::RCP<VectorType> & in) { x = in; }
   inline Teuchos::RCP<VectorType> get_x() const { return x; }

   inline void set_dxdt(const Teuchos::RCP<VectorType> & in) { dxdt = in; }
   inline Teuchos::RCP<VectorType> get_dxdt() const { return dxdt; }

   inline void set_f(const Teuchos::RCP<VectorType> & in) { f = in; }
   inline Teuchos::RCP<VectorType> get_f() const { return f; }

   inline void set_A(const Teuchos::RCP<CrsMatrixType> & in) { A = in; }
   inline Teuchos::RCP<CrsMatrixType> get_A() const { return A; }

private:
   Teuchos::RCP<VectorType> x, dxdt, f;
   Teuchos::RCP<CrsMatrixType> A;
};

}

#include "Panzer_BlockedLinearObjContainer_impl.hpp"

#endif
