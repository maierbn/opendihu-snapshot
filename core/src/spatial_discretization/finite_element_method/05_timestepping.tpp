#include "spatial_discretization/finite_element_method/05_timestepping.h"

#include <Python.h>
#include <iostream>
#include <petscksp.h>
#include <vector>
#include <numeric>

#include "easylogging++.h"

#include "control/types.h"
#include "utility/python_utility.h"
#include "utility/petsc_utility.h"
#include "solver/solver_manager.h"
#include "solver/linear.h"


namespace SpatialDiscretization
{
  
template<typename BasisOnMeshType, typename QuadratureType, typename Term>
FiniteElementMethodTimeStepping<BasisOnMeshType, QuadratureType, Term>::
FiniteElementMethodTimeStepping(DihuContext context)
  : FiniteElementMethodBaseRhs<BasisOnMeshType, QuadratureType, Term>(context),
  DiscretizableInTime(SolutionVectorMapping(true))
{
  // the solutionVectorMapping_ object stores the information which range of values of the solution will be further used 
  // in methods that use the result of this method, e.g. in operator splittings. Since there are no internal values
  // in this FEM, set the range to all values.
  solutionVectorMapping_.setOutputRange(0, this->data_.mesh()->nNodes());
}

template<typename BasisOnMeshType, typename QuadratureType, typename Term>
void FiniteElementMethodTimeStepping<BasisOnMeshType, QuadratureType, Term>::
initialize()
{
  this->data_.debug("FiniteElementMethodTimeStepping::initialize");
  
  this->data_.initialize();
  this->setStiffnessMatrix();
  this->setMassMatrix();
  this->data_.finalAssembly();
}

template<typename BasisOnMeshType, typename QuadratureType, typename Term>
void FiniteElementMethodTimeStepping<BasisOnMeshType, QuadratureType, Term>::
recoverRightHandSideStrongForm(Vec &result)
{
  // massMatrix * f_strong = rhs_weak
  Vec &rhs = this->data_.rightHandSide().values();   // rhs in weak formulation
  Mat &massMatrix = this->data_.massMatrix();
  
  PetscErrorCode ierr;
  
  // create linear solver context
  std::shared_ptr<Solver::Linear> linearSolver = this->context_.solverManager()->template solver<Solver::Linear>(this->specificSettings_);
  std::shared_ptr<KSP> ksp = linearSolver->ksp();
  
  // set matrix used for linear system and preconditioner to ksp context
  ierr = KSPSetOperators (*ksp, massMatrix, massMatrix); CHKERRV(ierr);
  
  // non zero initial values
  PetscScalar scalar = .5;
  ierr = VecSet(result, scalar); CHKERRV(ierr);
  ierr = KSPSetInitialGuessNonzero(*ksp, PETSC_TRUE); CHKERRV(ierr);
  
  // solve the system
  ierr = KSPSolve(*ksp, rhs, result); CHKERRV(ierr);
  
  int numberOfIterations = 0;
  PetscReal residualNorm = 0.0;
  ierr = KSPGetIterationNumber(*ksp, &numberOfIterations); CHKERRV(ierr);
  ierr = KSPGetResidualNorm(*ksp, &residualNorm); CHKERRV(ierr);
  
  //LOG(INFO) << "Rhs recovered in " << numberOfIterations << " iterations, residual norm " << residualNorm;
}

template<typename BasisOnMeshType, typename QuadratureType, typename Term>
void FiniteElementMethodTimeStepping<BasisOnMeshType, QuadratureType, Term>::
evaluateTimesteppingRightHandSide(Vec &input, Vec &output, int timeStepNo, double currentTime)
{
  Mat &stiffnessMatrix = this->data_.stiffnessMatrix();
  Vec &rhs = this->data_.rightHandSide().values();
  
  // compute rhs = stiffnessMatrix*input
  MatMult(stiffnessMatrix, input, rhs);
  
  // compute output = massMatrix^{-1}*rhs
  recoverRightHandSideStrongForm(output);
  
  this->data_.print();
  this->outputWriterManager_.writeOutput(this->data_, timeStepNo, currentTime);
}

template<typename BasisOnMeshType, typename QuadratureType, typename Term>
bool FiniteElementMethodTimeStepping<BasisOnMeshType, QuadratureType, Term>::
knowsMeshType()
{
  return true;
}

template<typename BasisOnMeshType, typename QuadratureType, typename Term>
std::shared_ptr<Mesh::Mesh> FiniteElementMethodTimeStepping<BasisOnMeshType, QuadratureType, Term>::
mesh()
{
  return FiniteElementMethodBase<BasisOnMeshType, QuadratureType>::mesh();
}
  
} // namespace SpatialDiscretization