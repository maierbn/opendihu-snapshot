#pragma once

#include <Python.h>  // has to be the first included header
#include <petscmat.h>
#include "utility/petsc_utility.h"
#include <memory>

#include "control/types.h"
#include "mesh/mesh.h"
#include "data_management/data.h"
#include "field_variable/field_variable.h"
#include "partition/partitioned_petsc_vec/partitioned_petsc_vec.h"
#include "partition/partitioned_petsc_mat/partitioned_petsc_mat.h"

namespace Data
{

/**  The datastructures used for timestepping schemes, essentially stores a solution and an increment vector.
 *   nComponents is a constant that determines the number of components of each field variable.
 *   The time stepping works independent of that constant, it is only important for the output files, how many values there are associated with a single node.
 *   E.g. for a cellml model the nComponents should be set to the number of components of this model.
  */
template<typename FunctionSpaceType, int nComponents>
class TimeStepping : public Data<FunctionSpaceType>
{
public:

  typedef FieldVariable::FieldVariable<FunctionSpaceType,nComponents> FieldVariableType;
  typedef std::tuple<std::shared_ptr<FieldVariableType>,int,double> TransferableSolutionDataType;  // <field variable, output component no., prefactor>

  //! constructor
  TimeStepping(DihuContext context);

  //! destructor
  ~TimeStepping();

  //! return a reference to the solution vector, the PETSc Vec can be obtained via fieldVariable->valuesGlobal()
  std::shared_ptr<FieldVariableType> solution();

  //! return a reference to the increment vector, the PETSc Vec can be obtained via fieldVariable->valuesGlobal()
  std::shared_ptr<FieldVariableType> increment();
  
  //! set the names of the components for the solution field variable
  void setComponentNames(std::vector<std::string> componentNames); 
  
  //! set the value of outputComponentNo, i.e. the component no. of the component of the field variable that will be transferred to the other part of the operator when an operator splitting is used
  void setOutputComponentNo(int outputComponentNo);

  //! set the value of outputComponentNo, i.e. the component no. of the component of the field variable that will be transferred to the other part of the operator when an operator splitting is used
  void setPrefactor(double prefactor);

  //! print all stored data to stdout
  virtual void print();

  //! return the number of degrees of freedom per mesh node
  static constexpr int getNDofsPerNode();

  //! return the total number of degrees of freedom, this can be a multiple of the number of nodes of the mesh
  virtual dof_no_t nUnknownsLocalWithGhosts();
  
  //! return the total number of degrees of freedom, this can be a multiple of the number of nodes of the mesh
  virtual dof_no_t nUnknownsLocalWithoutGhosts();

  //! get the data that will be transferred in the operator splitting to the other term of the splitting
  //! the transfer is done by the solution_vector_mapping class
  TransferableSolutionDataType getSolutionForTransferInOperatorSplitting();

  //! field variables that will be output by outputWriters
  typedef std::tuple<
    std::shared_ptr<FieldVariable::FieldVariable<FunctionSpaceType,3>>,  // geometry
    std::shared_ptr<FieldVariableType>  // solution
  > OutputFieldVariables;

  //! get pointers to all field variables that can be written by output writers
  OutputFieldVariables getOutputFieldVariables();

protected:

  //! initializes the vectors with size
  virtual void createPetscObjects();

  std::shared_ptr<FieldVariableType> solution_;            ///< the vector of the variable of interest
  std::shared_ptr<FieldVariableType> increment_;        ///< the vector for delta u, (note, this might be reduced in future to only a sub-part of the whole data vector if memory consumption is a problem)
  std::vector<std::string> componentNames_;      ///< names of the components of the solution and increment variables
  
  int outputComponentNo_;   ///< the component no. of the component of the field variable that will be transferred to the other part of the operator when an operator splitting is used
  double prefactor_;        ///< a factor with which the solution will be scaled when transferred in an operator splitting

private:
  //! get maximum number of expected non-zeros in the system matrix
  void getPetscMemoryParameters(int &diagonalNonZeros, int &offdiagonalNonZeros);

};

} // namespace Data

#include "data_management/time_stepping/time_stepping.tpp"
