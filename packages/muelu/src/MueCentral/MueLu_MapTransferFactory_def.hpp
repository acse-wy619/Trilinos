// @HEADER
//
// ***********************************************************************
//
//        MueLu: A package for multigrid based preconditioning
//                  Copyright 2012 Sandia Corporation
//
// Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation,
// the U.S. Government retains certain rights in this software.
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
// Questions? Contact
//                    Jeremie Gaidamour (jngaida@sandia.gov)
//                    Jonathan Hu       (jhu@sandia.gov)
//                    Ray Tuminaro      (rstumin@sandia.gov)
//
// ***********************************************************************
//
// @HEADER
/*
 * MueLu_ContactMapTransferFactory_def.hpp
 *
 *  Created on: Aug 2, 2012
 *      Author: wiesner
 */

#ifndef MUELU_MAPTRANSFERFACTORY_DEF_HPP_
#define MUELU_MAPTRANSFERFACTORY_DEF_HPP_

#include "MueLu_MapTransferFactory_decl.hpp"

#include <Xpetra_Matrix.hpp>
#include <Xpetra_CrsMatrixWrap.hpp>
#include <Xpetra_MapFactory.hpp>
#include <Xpetra_VectorFactory.hpp>

#include "MueLu_Level.hpp"
#include "MueLu_FactoryManagerBase.hpp"
#include "MueLu_Monitor.hpp"

namespace MueLu {

  template <class Scalar, class LocalOrdinal, class GlobalOrdinal, class Node, class LocalMatOps>
  MapTransferFactory<Scalar, LocalOrdinal, GlobalOrdinal, Node, LocalMatOps>::MapTransferFactory(std::string mapName, Teuchos::RCP<const FactoryBase> mapFact)
  : mapName_(mapName), mapFact_(mapFact)
    {

    }

  template <class Scalar, class LocalOrdinal, class GlobalOrdinal, class Node, class LocalMatOps>
  void MapTransferFactory<Scalar, LocalOrdinal, GlobalOrdinal, Node, LocalMatOps>::DeclareInput(Level &fineLevel, Level &coarseLevel) const {
    fineLevel.DeclareInput(mapName_,mapFact_.get(),this);
    Teuchos::RCP<const FactoryBase> tentPFact = GetFactory("P");
    if (tentPFact == Teuchos::null) { tentPFact = coarseLevel.GetFactoryManager()->GetFactory("Ptent"); }
    coarseLevel.DeclareInput("P", tentPFact.get(), this);
  }

  template <class Scalar, class LocalOrdinal, class GlobalOrdinal, class Node, class LocalMatOps>
  void MapTransferFactory<Scalar, LocalOrdinal, GlobalOrdinal, Node, LocalMatOps>::Build(Level & fineLevel, Level & coarseLevel) const {
    typedef Xpetra::Matrix<Scalar, LocalOrdinal, GlobalOrdinal, Node, LocalMatOps> OperatorClass; //TODO
    typedef Xpetra::CrsMatrixWrap<Scalar, LocalOrdinal, GlobalOrdinal, Node, LocalMatOps> CrsOOperator; //TODO
    typedef Xpetra::Map<LocalOrdinal, GlobalOrdinal, Node> MapClass;
    typedef Xpetra::MapFactory<LocalOrdinal, GlobalOrdinal, Node> MapFactoryClass;
    typedef Xpetra::Vector<Scalar, LocalOrdinal, GlobalOrdinal, Node> VectorClass;
    typedef Xpetra::VectorFactory<Scalar, LocalOrdinal, GlobalOrdinal, Node> VectorFactoryClass;

    Monitor m(*this, "Contact Map transfer factory");

    if (fineLevel.IsAvailable(mapName_, mapFact_.get())==false) {
        GetOStream(Runtime0, 0) << "MapTransferFactory::Build: User provided map " << mapName_ << " not found in Level class." << std::endl;
    }

    // fetch map extractor from level
    RCP<const MapClass> transferMap = fineLevel.Get<RCP<const MapClass> >(mapName_,mapFact_.get());

    // Get default tentative prolongator factory
    // Getting it that way ensure that the same factory instance will be used for both SaPFactory and NullspaceFactory.
    // -- Warning: Do not use directly initialPFact_. Use initialPFact instead everywhere!
    RCP<const FactoryBase> tentPFact = GetFactory("P");
    if (tentPFact == Teuchos::null) { tentPFact = coarseLevel.GetFactoryManager()->GetFactory("Ptent"); }
    TEUCHOS_TEST_FOR_EXCEPTION(!coarseLevel.IsAvailable("P",tentPFact.get()),Exceptions::RuntimeError, "MueLu::MapTransferFactory::Build(): P (generated by TentativePFactory) not available.");
    RCP<OperatorClass> Ptent = coarseLevel.Get<RCP<OperatorClass> >("P", tentPFact.get());

    std::vector<GlobalOrdinal > coarseMapGids;

    // loop over local rows of Ptent
    for(size_t row=0; row<Ptent->getNodeNumRows(); row++) {

      GlobalOrdinal grid = Ptent->getRowMap()->getGlobalElement(row);
      if(transferMap->isNodeGlobalElement(grid)) {

        Teuchos::ArrayView<const LocalOrdinal> indices;
        Teuchos::ArrayView<const Scalar> vals;
        Ptent->getLocalRowView(row, indices, vals);

        for(size_t i=0; i<(size_t)indices.size(); i++) {
          // mark all columns in Ptent(grid,*) to be coarse Dofs of next level transferMap
          GlobalOrdinal gcid = Ptent->getColMap()->getGlobalElement(indices[i]);
          coarseMapGids.push_back(gcid);
        }
      } // end if isNodeGlobalElement(grid)
    }

    // build column maps
    std::sort(coarseMapGids.begin(), coarseMapGids.end());
    coarseMapGids.erase(std::unique(coarseMapGids.begin(), coarseMapGids.end()), coarseMapGids.end());
    Teuchos::ArrayView<GlobalOrdinal> coarseMapGidsView (&coarseMapGids[0],coarseMapGids.size());
    Teuchos::RCP<const MapClass> coarseTransferMap = MapFactoryClass::Build(Ptent->getColMap()->lib(), -1, coarseMapGidsView, Ptent->getColMap()->getIndexBase(), Ptent->getColMap()->getComm());

    // store map extractor in coarse level
    coarseLevel.Set(mapName_, coarseTransferMap, mapFact_.get());
  }


} // namespace MueLu

#endif /* MUELU_MAPTRANSFERFACTORY_DEF_HPP_ */
