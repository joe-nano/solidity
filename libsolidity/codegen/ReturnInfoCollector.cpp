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

#include <libsolidity/codegen/ReturnInfoCollector.h>
#include <libsolidity/ast/Types.h>
#include <libsolidity/ast/AST.h>

namespace solidity::frontend {

ReturnInfoCollector::ReturnInfoCollector(langutil::EVMVersion const& _evmVersion): m_evmVersion{_evmVersion}
{
}

ReturnInfo ReturnInfoCollector::collect(FunctionCall const& _functionCall, std::string _newYulVariable)
{
	solAssert(_functionCall.expression().annotation().type, "Type of expression not set.");
	FunctionType const& funType = dynamic_cast<FunctionType const&>(*_functionCall.expression().annotation().type);

	return collect(funType, _newYulVariable, &_functionCall);
}

ReturnInfo ReturnInfoCollector::collect(FunctionType const& _functionType, std::string _newYulVariable, FunctionCall const* _functionCall)
{
	FunctionType::Kind const funKind = _functionType.kind();
	bool const haveReturndatacopy = m_evmVersion.supportsReturndata();
	bool const returnSuccessConditionAndReturndata =
		funKind == FunctionType::Kind::BareCall ||
		funKind == FunctionType::Kind::BareDelegateCall ||
		funKind == FunctionType::Kind::BareStaticCall;

	TypePointers returnTypes{};
	bool dynamicReturnSize = false;
	unsigned estimatedReturnSize = 0;

	if (!returnSuccessConditionAndReturndata)
	{
		if (haveReturndatacopy)
			returnTypes = _functionType.returnParameterTypes();
		else
			returnTypes = _functionType.returnParameterTypesWithoutDynamicTypes();

		for (auto const& retType: returnTypes)
			if (retType->isDynamicallyEncoded())
			{
				solAssert(haveReturndatacopy, "");
				dynamicReturnSize = true;
				estimatedReturnSize = 0;
				break;
			}
			else if (retType->decodingType())
				estimatedReturnSize += retType->decodingType()->calldataEncodedSize();
			else
				estimatedReturnSize += retType->calldataEncodedSize();
	}

	return ReturnInfo{
		_functionCall,
		_newYulVariable,
		move(returnTypes),
		dynamicReturnSize,
		estimatedReturnSize
	};
}

} // end namespace
