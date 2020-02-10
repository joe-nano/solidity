/*
	This file is part of solidity.

	solidity is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	solidity is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with solidity.  If not, see <http://www.gnu.org/licenses/>.
*/
/**
 * Component that translates Solidity code into Yul at statement level and below.
 */
#pragma once

#include <liblangutil/EVMVersion.h>
#include <libsolidity/ast/Types.h>

#include <functional>
#include <string>

namespace solidity::frontend
{

class FunctionCall;

/// Holds a set of properties of an external function call's return values.
struct ReturnInfo
{
	/// Related function call this ReturnInfo was generated from.
	FunctionCall const* functionCall = nullptr;

	/// Name of the yul variable pointing at the memory location of the (undecoded) return data.
	std::string returndataVariable = {};

	/// Vector of TypePointer, for each return variable. Dynamic types are already replaced if required.
	TypePointers returnTypes = {};

	/// Boolean, indicating whether or not return size is only known at runtime.
	bool dynamicReturnSize = false;

	/// Contains the at compile time estimated return size.
	unsigned estimatedReturnSize = 0;
};

/// Helper-class used to construct ReturnInfo.
class ReturnInfoCollector
{
public:
	explicit ReturnInfoCollector(langutil::EVMVersion const& _evmVersion);

	/// Assembles some information about the return types and related.
	///
	/// @param _functionCall the FunctionCall to extract the return-data information from.
	/// @param _returndataYulVariable Used by the caller to remember the corresponding Yul-variable used for storing undecoded return values.
	///
	/// @returns an info-struct containing the collected information including the passed input variables.
	[[nodiscard]] ReturnInfo collect(FunctionCall const& _functionCall, std::string _returndataYulVariable);

	/// Assembles some information about the return types and related.
	///
	/// @param _functionType the actual function type information to compute the return-info from.
	/// @param _returndataYulVariable Can be used by the caller to remember the corresponding Yul-variable used for storing undecoded return values.
	/// @param _functionCall Can be used by the caller to remember the corresponding FunctionCall.
	///
	/// @returns an info-struct containing the collected information, optionally also with the input
	///          avariables @p _returndataYulVariable and @p _functionCall.
	[[nodiscard]] ReturnInfo collect(
		FunctionType const& _functionType,
		std::string _returndataYulVariable = {},
		FunctionCall const* _functionCall = nullptr
	);

private:
	langutil::EVMVersion m_evmVersion;
};

} // end namespace
