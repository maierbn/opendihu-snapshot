#pragma once

#include "operator_splitting/operator_splitting.h"
#include "control/runnable.h"
#include "data_management/time_stepping.h"

namespace OperatorSplitting
{

template<typename TimeStepping1, typename TimeStepping2>
class Strang : public OperatorSplitting, public Runnable
{
public:
  //! constructor
  Strang(const DihuContext &context);
  
  //! run the simulation
  void run();
  
  //! advance time stepping by span
  void advanceTimeSpan();
  
  //! return a solution vector
  Vec &solution();
  
  //! return whether the object has a specified mesh type or if it is independent of any mesh type
  bool knowsMeshType();
  
private:
 
  void initialize();
 
  //DihuContext &context_;    ///< the context object containing everything to be stored
  Data::TimeStepping<typename TimeStepping1::BasisOnMesh> data_;     ///< data object that holds all PETSc vectors and matrices
  
  TimeStepping1 timeStepping1_;    ///< the object to be discretized
  TimeStepping2 timeStepping2_;    ///< the object to be discretized
  
  bool outputData1_;               ///< if data output via writer is enabled for timeStepping1
  bool outputData2_;               ///< if data output via writer is enabled for timeStepping2
};

}  // namespace

#include "operator_splitting/strang.tpp"