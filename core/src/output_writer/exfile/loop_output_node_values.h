#pragma once

#include "utility/type_utility.h"

#include <cstdlib>

/** The functions in this file model a loop over the elements of a tuple, as it occurs as OutputFieldVariablesType in all data_management classes.
 *  (Because the types inside the tuple are static and fixed at compile-time, a simple for loop c not work here.)
 *  The two functions starting with loop recursively emulate the loop. One method is the break condition and does nothing, the other method does the work and calls the method without loop in the name.
 *  OutputFieldVariablesType is assumed to be of type std::tuple<...>> where the types can be (mixed) std::shared_ptr<FieldVariable> or std::vector<std::shared_ptr<FieldVariable>>.
 */

namespace OutputWriter
{

namespace ExfileLoopOverTuple
{
 
 /** Static recursive loop from 0 to number of entries in the tuple
 *  Stopping criterion
 */
template<typename OutputFieldVariablesType, int i=0>
inline typename std::enable_if<i == std::tuple_size<OutputFieldVariablesType>::value, void>::type
loopOutputNodeValues(const OutputFieldVariablesType &fieldVariables, std::string meshName,
                                    std::ostream &stream, node_no_t nodeGlobalNo)
{}

 /** Static recursive loop from 0 to number of entries in the tuple
 * Loop body
 */
template<typename OutputFieldVariablesType, int i=0>
inline typename std::enable_if<i < std::tuple_size<OutputFieldVariablesType>::value, void>::type
loopOutputNodeValues(const OutputFieldVariablesType &fieldVariables, std::string meshName,
                                    std::ostream &stream, node_no_t nodeGlobalNo);

/** Loop body for a vector element
 */
template<typename VectorType>
typename std::enable_if<TypeUtility::isVector<VectorType>::value, bool>::type
outputNodeValues(VectorType currentFieldVariableVector, std::string meshName, 
                 std::ostream &stream, node_no_t nodeGlobalNo);

/** Loop body for a tuple element
 */
template<typename VectorType>
typename std::enable_if<TypeUtility::isTuple<VectorType>::value, bool>::type
outputNodeValues(VectorType currentFieldVariableVector, std::string meshName, 
                 std::ostream &stream, node_no_t nodeGlobalNo);

 /**  Loop body for a pointer element
 */
template<typename CurrentFieldVariableType>
typename std::enable_if<!TypeUtility::isTuple<CurrentFieldVariableType>::value && !TypeUtility::isVector<CurrentFieldVariableType>::value, bool>::type
outputNodeValues(CurrentFieldVariableType currentFieldVariable, std::string meshName, 
                 std::ostream &stream, node_no_t nodeGlobalNo);

};  //namespace ExfileLoopOverTuple

};  //namespace OutputWriter

#include "output_writer/exfile/loop_output_node_values.tpp"
