/* Copyright 2013-2016 Sathya Laufer
 *
 * libhomegear-base is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * libhomegear-base is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with libhomegear-base.  If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * In addition, as a special exception, the copyright holders give
 * permission to link the code of portions of this program with the
 * OpenSSL library under certain conditions as described in each
 * individual source file, and distribute linked combinations
 * including the two.
 * You must obey the GNU Lesser General Public License in all respects
 * for all of the code used other than OpenSSL.  If you modify
 * file(s) with this exception, you may extend this exception to your
 * version of the file(s), but you are not obligated to do so.  If you
 * do not wish to do so, delete this exception statement from your
 * version.  If you delete this exception statement from all source
 * files in the program, then also delete it here.
*/

#include <mutex>
#include <memory>
#include "homegear-base/BaseLib.h"

std::shared_ptr<BaseLib::Obj> _bl;
std::string _workingDirectory;
std::string _executablePath;

void getExecutablePath(int argc, char* argv[])
{
	char path[1024];
	if(!getcwd(path, sizeof(path)))
	{
		std::cerr << "Could not get working directory." << std::endl;
		exit(1);
	}
	_workingDirectory = std::string(path);
#ifdef KERN_PROC //BSD system
	int mib[4];
	mib[0] = CTL_KERN;
	mib[1] = KERN_PROC;
	mib[2] = KERN_PROC_PATHNAME;
	mib[3] = -1;
	size_t cb = sizeof(path);
	int result = sysctl(mib, 4, path, &cb, NULL, 0);
	if(result == -1)
	{
		std::cerr << "Could not get executable path." << std::endl;
		exit(1);
	}
	path[sizeof(path) - 1] = '\0';
	_executablePath = std::string(path);
	_executablePath = _executablePath.substr(0, _executablePath.find_last_of("/") + 1);
#else
	int length = readlink("/proc/self/exe", path, sizeof(path) - 1);
	if (length < 0)
	{
		std::cerr << "Could not get executable path." << std::endl;
		exit(1);
	}
	if((unsigned)length > sizeof(path))
	{
		std::cerr << "The path the homegear binary is in has more than 1024 characters." << std::endl;
		exit(1);
	}
	path[length] = '\0';
	_executablePath = std::string(path);
	_executablePath = _executablePath.substr(0, _executablePath.find_last_of("/") + 1);
#endif
}

void testBinaryRpc()
{
	std::cout << "Testing class BinaryRpc..." << std::endl;
	BaseLib::Rpc::BinaryRpc binaryRpc(_bl.get());
	std::vector<char> binaryRpcBuffer1 = _bl->hf.getBinary("42696E400000002F000000010000000D417574686F72697A6174696F6E0000001642617369632064326C72615470775A57527059513D3D000000250000001273797374656D2E6C6973744D6574686F6473000000010000000300000003426C61");
	int32_t bytesProcessed = binaryRpc.process(&binaryRpcBuffer1[0], binaryRpcBuffer1.size());
	if(!binaryRpc.isFinished() || bytesProcessed != 96 || binaryRpc.getData() != binaryRpcBuffer1)
	{
		std::cerr << "Binary RPC test failed (1)." << std::endl;
	}

	binaryRpc.reset();
	std::vector<char> binaryRpcBuffer2 = _bl->hf.getBinary("42696E01000000080000000300000003426C61");
	bytesProcessed = binaryRpc.process(&binaryRpcBuffer2[0], binaryRpcBuffer2.size());
	if(!binaryRpc.isFinished() || bytesProcessed != 16 || !std::equal(binaryRpc.getData().begin(), binaryRpc.getData().end(), binaryRpcBuffer2.begin()))
	{
		std::cerr << "Binary RPC test failed (2)." << std::endl;
	}

	binaryRpc.reset();
	binaryRpcBuffer2 = _bl->hf.getBinary("42696E010000000B0000000300000003426C61");
	bytesProcessed = binaryRpc.process(&binaryRpcBuffer2[0], binaryRpcBuffer2.size());
	if(!binaryRpc.isFinished() || bytesProcessed != 19 || binaryRpc.getData() != binaryRpcBuffer2)
	{
		std::cerr << "Binary RPC test failed (3)." << std::endl;
	}

	binaryRpc.reset();
	std::vector<char> binaryRpcBuffer3 = _bl->hf.getBinary("42696E400000002F000000010000000D417574686F72697A6174696F6E0000001642617369632064326C72615470775A57527059513D3D000000250000001273797374656D2E6C6973744D6574686F6473000000010000000300000003426C6142696E010000000B0000000300000003426C61");
	bytesProcessed = binaryRpc.process(&binaryRpcBuffer3[0], binaryRpcBuffer3.size());
	if(!binaryRpc.isFinished() || bytesProcessed != 96 || binaryRpc.getData() != binaryRpcBuffer1)
	{
		std::cerr << "Binary RPC test failed (4)." << std::endl;
	}
	binaryRpc.reset();
	bytesProcessed = binaryRpc.process(&binaryRpcBuffer3[bytesProcessed], binaryRpcBuffer3.size() - bytesProcessed);
	if(!binaryRpc.isFinished() || bytesProcessed != 19 || binaryRpc.getData() != binaryRpcBuffer2)
	{
		std::cerr << "Binary RPC test failed (5)." << std::endl;
	}

	binaryRpc.reset();
	for(uint32_t i = 0; i < binaryRpcBuffer3.size(); ++i)
	{
		bytesProcessed = binaryRpc.process(&binaryRpcBuffer3[i], 1);
		if(binaryRpc.isFinished())
		{
			if(i < 96)
			{
				if(binaryRpc.getData() != binaryRpcBuffer1)
				{
					std::cerr << "Binary RPC test failed (6)." << std::endl;
				}
				binaryRpc.reset();
				if(bytesProcessed == 0) binaryRpc.process(&binaryRpcBuffer3[i], 1);
			}
			else
			{
				if(binaryRpc.getData() != binaryRpcBuffer2)
				{
					std::cerr << "Binary RPC test failed (7)." << std::endl;
				}
			}
		}
	}

	std::vector<char> binaryRpcBuffer4;
	for(uint32_t i = 1; i < binaryRpcBuffer3.size(); ++i)
	{
		binaryRpc.reset();
		binaryRpcBuffer4 = std::vector<char>(&binaryRpcBuffer3[0], &binaryRpcBuffer3[0] + i);
		bytesProcessed = binaryRpc.process(&binaryRpcBuffer4[0], binaryRpcBuffer4.size());
		if(binaryRpc.isFinished())
		{
			//std::cout << std::setw(3) << std::to_string(i) << ' ' << _bl->hf.getHexString(binaryRpc.getData()) << std::endl;
			if(binaryRpc.getData() != binaryRpcBuffer1) std::cerr << "Binary RPC test failed (8)." << std::endl;
			binaryRpc.reset();
			if(bytesProcessed != (signed)binaryRpcBuffer4.size()) bytesProcessed = binaryRpc.process(&binaryRpcBuffer4[bytesProcessed], binaryRpcBuffer4.size() - bytesProcessed);
			binaryRpcBuffer4 = std::vector<char>(&binaryRpcBuffer3[0] + i, (&binaryRpcBuffer3[0] + i) + (binaryRpcBuffer3.size() - i));
			bytesProcessed = binaryRpc.process(&binaryRpcBuffer4[0], binaryRpcBuffer4.size());
			//std::cout << std::setw(3) << std::to_string(i) << ' ' << _bl->hf.getHexString(binaryRpc.getData()) << std::endl;
			if(!binaryRpc.isFinished() || binaryRpc.getData() != binaryRpcBuffer2) std::cerr << "Binary RPC test failed (9)." << std::endl;
		}
		else
		{
			binaryRpcBuffer4 = std::vector<char>(&binaryRpcBuffer3[0] + i, (&binaryRpcBuffer3[0] + i) + (binaryRpcBuffer3.size() - i));
			bytesProcessed = binaryRpc.process(&binaryRpcBuffer4[0], binaryRpcBuffer4.size());
			//std::cout << std::setw(3) << std::to_string(i) << ' ' << _bl->hf.getHexString(binaryRpc.getData()) << std::endl;
			if(!binaryRpc.isFinished() || binaryRpc.getData() != binaryRpcBuffer1) std::cerr << "Binary RPC test failed (10)." << std::endl;
			binaryRpc.reset();
			bytesProcessed = binaryRpc.process(&binaryRpcBuffer4[bytesProcessed], binaryRpcBuffer4.size() - bytesProcessed);
			//std::cout << std::setw(3) << std::to_string(i) << ' ' << _bl->hf.getHexString(binaryRpc.getData()) << std::endl;
			if(!binaryRpc.isFinished() || binaryRpc.getData() != binaryRpcBuffer2) std::cerr << "Binary RPC test failed (11)." << std::endl;
		}
	}

	binaryRpc.reset();
	binaryRpcBuffer3 = _bl->hf.getBinary("42");
	bytesProcessed = binaryRpc.process(&binaryRpcBuffer3[0], binaryRpcBuffer3.size());
	if(bytesProcessed != (signed)binaryRpcBuffer3.size() || binaryRpc.isFinished()) std::cerr << "Binary RPC test failed (8)." << std::endl;
	else
	{
		binaryRpcBuffer3 = _bl->hf.getBinary("696E400000002F000000010000000D417574686F72697A6174696F6E0000001642617369632064326C72615470775A57527059513D3D000000250000001273797374656D2E6C6973744D6574686F6473000000010000000300000003426C6142696E010000000B0000000300000003426C61");
		bytesProcessed = binaryRpc.process(&binaryRpcBuffer3[0], binaryRpcBuffer3.size());
		if(bytesProcessed != 95 || !binaryRpc.isFinished() || binaryRpc.getData() != binaryRpcBuffer1) std::cerr << "Binary RPC test failed (9)." << std::endl;
		binaryRpc.reset();
		bytesProcessed = binaryRpc.process(&binaryRpcBuffer3[bytesProcessed], binaryRpcBuffer3.size() - bytesProcessed);
		if(bytesProcessed != 19 || !binaryRpc.isFinished() || binaryRpc.getData() != binaryRpcBuffer2) std::cerr << "Binary RPC test failed (10)." << std::endl;
	}

	std::cout << "Finished testing class BinaryRpc..." << std::endl << std::endl;
}

void testJson()
{
	std::cout << "Testing JSON en-/decoding..." << std::endl;
	std::string jsonInput("{\"on\":true, \"off\":    false, \"bla\":null, \"blupp\":  [5.643,false , null ,true ],\"blupp2\":[ 5.643,false,null,true], \"sät\":255.63456, \"bri\":-255,\"hue\":10000, \"bli\":{\"a\": 2,\"b\":false}    ,     \"e\"  :   -34785326,\"f\":-0.547887237, \"g\":{},\"h\":[], \"i\" : {    }  , \"j\" : [    ] , \"k\": {} , \"l\": [] }");
	BaseLib::RPC::JsonDecoder jsonDecoder(_bl.get());
	BaseLib::PVariable variable = jsonDecoder.decode(jsonInput);
	if(variable->type != BaseLib::VariableType::tStruct)
	{
		std::cerr << "JSON decoding test failed: Container is not of type struct." << std::endl;
	}
	else
	{
		if(variable->structValue->size() != 17)
		{
			std::cerr << "JSON decoding test failed: Struct size is not 17." << std::endl;
		}
		else
		{
			if(variable->structValue->find("bla") == variable->structValue->end() ||
			   variable->structValue->find("bli") == variable->structValue->end() ||
			   variable->structValue->find("blupp") == variable->structValue->end() ||
			   variable->structValue->find("blupp2") == variable->structValue->end() ||
			   variable->structValue->find("bri") == variable->structValue->end() ||
			   variable->structValue->find("e") == variable->structValue->end() ||
			   variable->structValue->find("f") == variable->structValue->end() ||
			   variable->structValue->find("g") == variable->structValue->end() ||
			   variable->structValue->find("h") == variable->structValue->end() ||
			   variable->structValue->find("hue") == variable->structValue->end() ||
			   variable->structValue->find("i") == variable->structValue->end() ||
			   variable->structValue->find("j") == variable->structValue->end() ||
			   variable->structValue->find("k") == variable->structValue->end() ||
			   variable->structValue->find("l") == variable->structValue->end() ||
			   variable->structValue->find("off") == variable->structValue->end() ||
			   variable->structValue->find("on") == variable->structValue->end() ||
			   variable->structValue->find("sät") == variable->structValue->end()
			)
			{
				std::cerr << "JSON decoding test failed: Couldn't find all variable names." << std::endl;
			}

			BaseLib::PVariable structValue = variable->structValue->at("bla");
			if(structValue->type != BaseLib::VariableType::tVoid) std::cerr << "JSON decoding test failed: Variable \"bla\" has wrong type." << std::endl;

			structValue = variable->structValue->at("bli");
			if(structValue->type != BaseLib::VariableType::tStruct) std::cerr << "JSON decoding test failed: Variable \"bli\" has wrong type." << std::endl;
			else if(structValue->structValue->size() != 2) std::cerr << "JSON decoding test failed: Variable \"bli\" has wrong size." << std::endl;
			else if(structValue->structValue->find("a") == structValue->structValue->end() || structValue->structValue->find("b") == structValue->structValue->end()) std::cerr << "JSON decoding test failed: Couldn't find all variable names in \"bli\"." << std::endl;
			else if(structValue->structValue->at("a")->type != BaseLib::VariableType::tInteger || structValue->structValue->at("a")->integerValue != 2 ||
					structValue->structValue->at("b")->type != BaseLib::VariableType::tBoolean || structValue->structValue->at("b")->booleanValue != false)
			{
				std::cerr << "JSON decoding test failed: Variables in \"bli\" have wrong type or value." << std::endl;
			}

			structValue = variable->structValue->at("blupp");
			if(structValue->type != BaseLib::VariableType::tArray) std::cerr << "JSON decoding test failed: Variable \"blupp\" has wrong type." << std::endl;
			else if(structValue->arrayValue->size() != 4) std::cerr << "JSON decoding test failed: Variable \"blupp\" has wrong size." << std::endl;
			else if(structValue->arrayValue->at(0)->type != BaseLib::VariableType::tFloat || structValue->arrayValue->at(0)->floatValue != 5.643 ||
					structValue->arrayValue->at(1)->type != BaseLib::VariableType::tBoolean || structValue->arrayValue->at(1)->booleanValue != false ||
					structValue->arrayValue->at(2)->type != BaseLib::VariableType::tVoid ||
					structValue->arrayValue->at(3)->type != BaseLib::VariableType::tBoolean || structValue->arrayValue->at(3)->booleanValue != true
			)
			{
				std::cerr << "JSON decoding test failed: Variables in \"blupp\" have wrong type or value." << std::endl;
			}

			structValue = variable->structValue->at("blupp2");
			if(structValue->type != BaseLib::VariableType::tArray) std::cerr << "JSON decoding test failed: Variable \"blupp2\" has wrong type." << std::endl;
			else if(structValue->arrayValue->size() != 4) std::cerr << "JSON decoding test failed: Variable \"blupp2\" has wrong size." << std::endl;
			else if(structValue->arrayValue->at(0)->type != BaseLib::VariableType::tFloat || structValue->arrayValue->at(0)->floatValue != 5.643 ||
					structValue->arrayValue->at(1)->type != BaseLib::VariableType::tBoolean || structValue->arrayValue->at(1)->booleanValue != false ||
					structValue->arrayValue->at(2)->type != BaseLib::VariableType::tVoid ||
					structValue->arrayValue->at(3)->type != BaseLib::VariableType::tBoolean || structValue->arrayValue->at(3)->booleanValue != true
			)
			{
				std::cerr << "JSON decoding test failed: Variables in \"blupp2\" have wrong type or value." << std::endl;
			}

			structValue = variable->structValue->at("bri");
			if(structValue->type != BaseLib::VariableType::tInteger || structValue->integerValue != -255) std::cerr << "JSON decoding test failed: Variable \"bri\" has wrong type or value." << std::endl;

			structValue = variable->structValue->at("e");
			if(structValue->type != BaseLib::VariableType::tInteger || structValue->integerValue != -34785326) std::cerr << "JSON decoding test failed: Variable \"e\" has wrong type or value." << std::endl;

			structValue = variable->structValue->at("f");
			if(structValue->type != BaseLib::VariableType::tFloat || structValue->floatValue != -0.547887237) std::cerr << "JSON decoding test failed: Variable \"f\" has wrong type or value. Value is: " << std::fixed << std::setprecision(15) << structValue->floatValue << std::endl;

			structValue = variable->structValue->at("g");
			if(structValue->type != BaseLib::VariableType::tStruct || structValue->structValue->size() != 0) std::cerr << "JSON decoding test failed: Variable \"g\" has wrong type or value." << std::endl;

			structValue = variable->structValue->at("h");
			if(structValue->type != BaseLib::VariableType::tArray || structValue->arrayValue->size() != 0) std::cerr << "JSON decoding test failed: Variable \"h\" has wrong type or value." << std::endl;

			structValue = variable->structValue->at("hue");
			if(structValue->type != BaseLib::VariableType::tInteger || structValue->integerValue != 10000) std::cerr << "JSON decoding test failed: Variable \"hue\" has wrong type or value." << std::endl;

			structValue = variable->structValue->at("i");
			if(structValue->type != BaseLib::VariableType::tStruct || structValue->structValue->size() != 0) std::cerr << "JSON decoding test failed: Variable \"i\" has wrong type or value." << std::endl;

			structValue = variable->structValue->at("j");
			if(structValue->type != BaseLib::VariableType::tArray || structValue->arrayValue->size() != 0) std::cerr << "JSON decoding test failed: Variable \"j\" has wrong type or value." << std::endl;

			structValue = variable->structValue->at("k");
			if(structValue->type != BaseLib::VariableType::tStruct || structValue->structValue->size() != 0) std::cerr << "JSON decoding test failed: Variable \"k\" has wrong type or value." << std::endl;

			structValue = variable->structValue->at("l");
			if(structValue->type != BaseLib::VariableType::tArray || structValue->arrayValue->size() != 0) std::cerr << "JSON decoding test failed: Variable \"l\" has wrong type or value." << std::endl;

			structValue = variable->structValue->at("off");
			if(structValue->type != BaseLib::VariableType::tBoolean || structValue->booleanValue != false) std::cerr << "JSON decoding test failed: Variable \"off\" has wrong type or value." << std::endl;

			structValue = variable->structValue->at("on");
			if(structValue->type != BaseLib::VariableType::tBoolean || structValue->booleanValue != true) std::cerr << "JSON decoding test failed: Variable \"on\" has wrong type or value." << std::endl;

			structValue = variable->structValue->at("sät");
			if(structValue->type != BaseLib::VariableType::tFloat || (structValue->floatValue != 255.63456 && structValue->floatValue != 255.634559999999993)) std::cerr << "JSON decoding test failed: Variable \"sät\" has wrong type or value. Value is: " << std::fixed << std::setprecision(15) << structValue->floatValue << std::endl;
		}
	}

	BaseLib::RPC::JsonEncoder jsonEncoder(_bl.get());
	std::string jsonOutput;
	jsonEncoder.encode(variable, jsonOutput);
	if(jsonOutput != "{\"bla\":null,\"bli\":{\"a\":2,\"b\":false},\"blupp\":[5.643000000000000,false,null,true],\"blupp2\":[5.643000000000000,false,null,true],\"bri\":-255,\"e\":-34785326,\"f\":-0.547887237000000,\"g\":{},\"h\":[],\"hue\":10000,\"i\":{},\"j\":[],\"k\":{},\"l\":[],\"off\":false,\"on\":true,\"sät\":255.634559999999993}")
	{
		std::cerr << "JSON encoding test failed." << std::endl;
	}
	std::cout << "Finished testing JSON en-/decoding..." << std::endl << std::endl;
}

int main(int argc, char* argv[])
{
	_bl.reset(new BaseLib::Obj(_executablePath, nullptr, false));

	testJson();
	testBinaryRpc();

	return 0;
}
