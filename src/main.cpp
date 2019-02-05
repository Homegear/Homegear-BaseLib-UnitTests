/* Copyright 2013-2019 Homegear GmbH
 *
 * homegear-baselib-unittests is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * homegear-baselib-unittests is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with homegear-baselib-unittests.  If not, see
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

#include "HelperFunctions.h"
#include "Json.h"
#include "WebSocket.h"
#include "Math.h"
#include "Http.h"

#include <mutex>
#include <memory>
#include "homegear-base/BaseLib.h"

std::shared_ptr<BaseLib::SharedObjects> _bl;
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

	std::cout << "Finished testing class BinaryRpc." << std::endl << std::endl;
}

void testAnsiConversion()
{
	std::cout << "Testing ANSI to UTF-8 and UTF-8 to ANSI conversion..." << std::endl;
	BaseLib::Ansi ansi(true, true);
	std::vector<uint8_t> utf8;

	//äüößÄÖÜ
	utf8 = std::vector<uint8_t>{ 0xC3, 0xA4, 0xC3, 0xBC, 0xC3, 0xB6, 0xC3, 0x9F, 0xC3, 0x84, 0xC3, 0x96, 0xC3, 0x9C };
	std::string result = ansi.toAnsi((char*)&utf8[0], utf8.size());
	if(BaseLib::HelperFunctions::getHexString(result) != "E4FCF6DFC4D6DC") std::cerr << "Conversion from UTF-8 to Ansi failed: Value: " << BaseLib::HelperFunctions::getHexString(utf8) << ", result: " << BaseLib::HelperFunctions::getHexString(result) << std::endl;
	result = ansi.toUtf8(result);
	if(BaseLib::HelperFunctions::getHexString(result) != "C3A4C3BCC3B6C39FC384C396C39C") std::cerr << "Conversion from Ansi back to UTF-8 failed: Result: " << BaseLib::HelperFunctions::getHexString(result) << std::endl;

	//{{{ Boundary condition test cases
		//{{{ First possible sequence of a certain length
			utf8 = std::vector<uint8_t>{ 0 };
			result = ansi.toAnsi((char*)&utf8[0], utf8.size());
			if(!result.empty()) std::cerr << "Conversion from UTF-8 to Ansi failed: Value: " << BaseLib::HelperFunctions::getHexString(utf8) << ", result: " << BaseLib::HelperFunctions::getHexString(result) << std::endl;

			utf8 = std::vector<uint8_t>{ 0xC2, 0x80 };
			result = ansi.toAnsi((char*)&utf8[0], utf8.size());
			if(result != "?") std::cerr << "Conversion from UTF-8 to Ansi failed: Value: " << BaseLib::HelperFunctions::getHexString(utf8) << ", result: " << BaseLib::HelperFunctions::getHexString(result) << std::endl;

			utf8 = std::vector<uint8_t>{ 0xE0, 0xA0, 0x80 };
			result = ansi.toAnsi((char*)&utf8[0], utf8.size());
			if(result != "?") std::cerr << "Conversion from UTF-8 to Ansi failed: Value: " << BaseLib::HelperFunctions::getHexString(utf8) << ", result: " << BaseLib::HelperFunctions::getHexString(result) << std::endl;

			utf8 = std::vector<uint8_t>{ 0xF0, 0x90, 0x80, 0x80 };
			result = ansi.toAnsi((char*)&utf8[0], utf8.size());
			if(result != "?") std::cerr << "Conversion from UTF-8 to Ansi failed: Value: " << BaseLib::HelperFunctions::getHexString(utf8) << ", result: " << BaseLib::HelperFunctions::getHexString(result) << std::endl;

			utf8 = std::vector<uint8_t>{ 0xF8, 0x88, 0x80, 0x80, 0x80 };
			result = ansi.toAnsi((char*)&utf8[0], utf8.size());
			if(!result.empty()) std::cerr << "Conversion from UTF-8 to Ansi failed: Value: " << BaseLib::HelperFunctions::getHexString(utf8) << ", result: " << BaseLib::HelperFunctions::getHexString(result) << std::endl;

			utf8 = std::vector<uint8_t>{ 0xFC, 0x84, 0x80, 0x80, 0x80, 0x80 };
			result = ansi.toAnsi((char*)&utf8[0], utf8.size());
			if(!result.empty()) std::cerr << "Conversion from UTF-8 to Ansi failed: Value: " << BaseLib::HelperFunctions::getHexString(utf8) << ", result: " << BaseLib::HelperFunctions::getHexString(result) << std::endl;
		//}}}

		//{{{ Last possible sequence of a certain length
			utf8 = std::vector<uint8_t>{ 0x7F };
			result = ansi.toAnsi((char*)&utf8[0], utf8.size());
			if(BaseLib::HelperFunctions::getHexString(result) != "7F") std::cerr << "Conversion from UTF-8 to Ansi failed: Value: " << BaseLib::HelperFunctions::getHexString(utf8) << ", result: " << BaseLib::HelperFunctions::getHexString(result) << std::endl;

			utf8 = std::vector<uint8_t>{ 0xDF, 0xBF };
			result = ansi.toAnsi((char*)&utf8[0], utf8.size());
			if(result != "?") std::cerr << "Conversion from UTF-8 to Ansi failed: Value: " << BaseLib::HelperFunctions::getHexString(utf8) << ", result: " << BaseLib::HelperFunctions::getHexString(result) << std::endl;

			utf8 = std::vector<uint8_t>{ 0xEF, 0xBF, 0xBF };
			result = ansi.toAnsi((char*)&utf8[0], utf8.size());
			if(result != "?") std::cerr << "Conversion from UTF-8 to Ansi failed: Value: " << BaseLib::HelperFunctions::getHexString(utf8) << ", result: " << BaseLib::HelperFunctions::getHexString(result) << std::endl;

			utf8 = std::vector<uint8_t>{ 0xF7, 0xBF, 0xBF, 0xBF };
			result = ansi.toAnsi((char*)&utf8[0], utf8.size());
			if(result != "?") std::cerr << "Conversion from UTF-8 to Ansi failed: Value: " << BaseLib::HelperFunctions::getHexString(utf8) << ", result: " << BaseLib::HelperFunctions::getHexString(result) << std::endl;

			utf8 = std::vector<uint8_t>{ 0xFB, 0xBF, 0xBF, 0xBF, 0xBF };
			result = ansi.toAnsi((char*)&utf8[0], utf8.size());
			if(!result.empty()) std::cerr << "Conversion from UTF-8 to Ansi failed: Value: " << BaseLib::HelperFunctions::getHexString(utf8) << ", result: " << BaseLib::HelperFunctions::getHexString(result) << std::endl;

			utf8 = std::vector<uint8_t>{ 0xFD, 0xBF, 0xBF, 0xBF, 0xBF, 0xBF };
			result = ansi.toAnsi((char*)&utf8[0], utf8.size());
			if(!result.empty()) std::cerr << "Conversion from UTF-8 to Ansi failed: Value: " << BaseLib::HelperFunctions::getHexString(utf8) << ", result: " << BaseLib::HelperFunctions::getHexString(result) << std::endl;
		//}}}

		//{{{ Other boundary conditions
			utf8 = std::vector<uint8_t>{ 0xED, 0x9F, 0xBF };
			result = ansi.toAnsi((char*)&utf8[0], utf8.size());
			if(result != "?") std::cerr << "Conversion from UTF-8 to Ansi failed: Value: " << BaseLib::HelperFunctions::getHexString(utf8) << ", result: " << BaseLib::HelperFunctions::getHexString(result) << std::endl;

			utf8 = std::vector<uint8_t>{ 0xEE, 0x80, 0x80 };
			result = ansi.toAnsi((char*)&utf8[0], utf8.size());
			if(result != "?") std::cerr << "Conversion from UTF-8 to Ansi failed: Value: " << BaseLib::HelperFunctions::getHexString(utf8) << ", result: " << BaseLib::HelperFunctions::getHexString(result) << std::endl;

			utf8 = std::vector<uint8_t>{ 0xEF, 0xBF, 0xBD };
			result = ansi.toAnsi((char*)&utf8[0], utf8.size());
			if(result != "?") std::cerr << "Conversion from UTF-8 to Ansi failed: Value: " << BaseLib::HelperFunctions::getHexString(utf8) << ", result: " << BaseLib::HelperFunctions::getHexString(result) << std::endl;

			utf8 = std::vector<uint8_t>{ 0xF4, 0x8F, 0xBF, 0xBF };
			result = ansi.toAnsi((char*)&utf8[0], utf8.size());
			if(result != "?") std::cerr << "Conversion from UTF-8 to Ansi failed: Value: " << BaseLib::HelperFunctions::getHexString(utf8) << ", result: " << BaseLib::HelperFunctions::getHexString(result) << std::endl;

			utf8 = std::vector<uint8_t>{ 0xF4, 0x90, 0x80, 0x80 };
			result = ansi.toAnsi((char*)&utf8[0], utf8.size());
			if(result != "?") std::cerr << "Conversion from UTF-8 to Ansi failed: Value: " << BaseLib::HelperFunctions::getHexString(utf8) << ", result: " << BaseLib::HelperFunctions::getHexString(result) << std::endl;
		//}}}

		//{{{ Malformed sequences
			//{{{ Unexpected continuation bytes
				utf8 = std::vector<uint8_t>{ 0x80 };
				result = ansi.toAnsi((char*)&utf8[0], utf8.size());
				if(!result.empty()) std::cerr << "Conversion from UTF-8 to Ansi failed: Value: " << BaseLib::HelperFunctions::getHexString(utf8) << ", result: " << BaseLib::HelperFunctions::getHexString(result) << std::endl;

				utf8 = std::vector<uint8_t>{ 0xBF };
				result = ansi.toAnsi((char*)&utf8[0], utf8.size());
				if(!result.empty()) std::cerr << "Conversion from UTF-8 to Ansi failed: Value: " << BaseLib::HelperFunctions::getHexString(utf8) << ", result: " << BaseLib::HelperFunctions::getHexString(result) << std::endl;

				utf8 = std::vector<uint8_t>{ 0x80, 0xBF };
				result = ansi.toAnsi((char*)&utf8[0], utf8.size());
				if(!result.empty()) std::cerr << "Conversion from UTF-8 to Ansi failed: Value: " << BaseLib::HelperFunctions::getHexString(utf8) << ", result: " << BaseLib::HelperFunctions::getHexString(result) << std::endl;

				utf8 = std::vector<uint8_t>{ 0x80, 0xBF, 0x80 };
				result = ansi.toAnsi((char*)&utf8[0], utf8.size());
				if(!result.empty()) std::cerr << "Conversion from UTF-8 to Ansi failed: Value: " << BaseLib::HelperFunctions::getHexString(utf8) << ", result: " << BaseLib::HelperFunctions::getHexString(result) << std::endl;

				utf8 = std::vector<uint8_t>{ 0x80, 0xBF, 0x80, 0xBF };
				result = ansi.toAnsi((char*)&utf8[0], utf8.size());
				if(!result.empty()) std::cerr << "Conversion from UTF-8 to Ansi failed: Value: " << BaseLib::HelperFunctions::getHexString(utf8) << ", result: " << BaseLib::HelperFunctions::getHexString(result) << std::endl;

				utf8 = std::vector<uint8_t>{ 0x80, 0xBF, 0x80, 0xBF, 0x80 };
				result = ansi.toAnsi((char*)&utf8[0], utf8.size());
				if(!result.empty()) std::cerr << "Conversion from UTF-8 to Ansi failed: Value: " << BaseLib::HelperFunctions::getHexString(utf8) << ", result: " << BaseLib::HelperFunctions::getHexString(result) << std::endl;

				utf8 = std::vector<uint8_t>{ 0x80, 0xBF, 0x80, 0xBF, 0x80, 0xBF };
				result = ansi.toAnsi((char*)&utf8[0], utf8.size());
				if(!result.empty()) std::cerr << "Conversion from UTF-8 to Ansi failed: Value: " << BaseLib::HelperFunctions::getHexString(utf8) << ", result: " << BaseLib::HelperFunctions::getHexString(result) << std::endl;

				utf8 = std::vector<uint8_t>{ 0x80, 0xBF, 0x80, 0xBF, 0x80, 0xBF, 0x80 };
				result = ansi.toAnsi((char*)&utf8[0], utf8.size());
				if(!result.empty()) std::cerr << "Conversion from UTF-8 to Ansi failed: Value: " << BaseLib::HelperFunctions::getHexString(utf8) << ", result: " << BaseLib::HelperFunctions::getHexString(result) << std::endl;
			//}}}

			//{{{ Loneley start characters
				utf8 = std::vector<uint8_t>{ 0xC0, 0x20, 0xC1, 0x20, 0xC2, 0x20, 0xC3, 0x20, 0xC4, 0x20, 0xC5, 0x20, 0xC6, 0x20, 0xC7, 0x20, 0xC8, 0x20, 0xC9, 0x20, 0xCA, 0x20, 0xCB, 0x20, 0xCC, 0x20, 0xCD, 0x20, 0xCE, 0x20, 0xCF, 0x20, 0xD0, 0x20, 0xD1, 0x20, 0xD2, 0x20, 0xD3, 0x20, 0xD4, 0x20, 0xD5, 0x20, 0xD6, 0x20, 0xD7, 0x20, 0xD8, 0x20, 0xD9, 0x20, 0xDA, 0x20, 0xDB, 0x20, 0xDC, 0x20, 0xDD, 0x20, 0xDE, 0x20, 0xDF, 0x20 };
				result = ansi.toAnsi((char*)&utf8[0], utf8.size());
				if(result != "????????????????????????????????") std::cerr << "Conversion from UTF-8 to Ansi failed: Value: " << BaseLib::HelperFunctions::getHexString(utf8) << ", result: " << BaseLib::HelperFunctions::getHexString(result) << std::endl;
			//}}}
		//}}}
	//}}}
	std::cout << "Finished testing UTF-8 to ANSI and ANSI to UTF-8 conversion." << std::endl << std::endl;
}

void testBitReaderWriter()
{
	std::cout << "Testing BitReaderWriter..." << std::endl;
	std::vector<uint8_t> data = { 0xAB, 0xC5, 0xD3, 0xB6, 0xD4, 0x5A, 0x73, 0x35 };
	std::vector<char> signedData = { (char)(uint8_t)0xAB, (char)(uint8_t)0xC5, (char)(uint8_t)0xD3, (char)(uint8_t)0xB6, (char)(uint8_t)0xD4, 0x5A, 0x73, 0x35 };
	uint64_t intData = 0xABC5D3B6D45A7335ll;
	uint64_t intDataReverse = 0x35735AD4B6D3C5ABll;

	for(uint32_t i = 0; i < data.size() * 8; i++) // i is start position
	{
		for(uint32_t j = 0; j <= 64; j++) // j represents length
		{
			int32_t rightShiftCount = 64 - j - i;
			uint64_t resultData = rightShiftCount >= 0 ? ((intData << i) >> i) >> rightShiftCount : ((intData << i) >> i) << (rightShiftCount * -1);
			std::vector<uint8_t> expectedResult((j / 8) + (j % 8 != 0 ? 1 : 0), 0);
			for(uint32_t k = 0; k < expectedResult.size(); k++)
			{
				expectedResult.at(k) = resultData >> ((expectedResult.size() - k - 1) * 8);
			}
			std::vector<uint8_t> result = BaseLib::BitReaderWriter::getPosition(data, i, j);
			if(result.size() != expectedResult.size() || !std::equal(result.begin(), result.end(), expectedResult.begin()))
			{
				std::cerr << "Error reading " << j << " bits from index " << i << ": Got " << BaseLib::HelperFunctions::getHexString(result) << " but expected " << BaseLib::HelperFunctions::getHexString(expectedResult) << std::endl;
			}
		}
	}

	for(uint32_t i = 0; i < data.size() * 8; i++) // i is start position
	{
		for(uint32_t j = 0; j <= 64; j++) // j represents length
		{
			int32_t rightShiftCount = 64 - j - i;
			uint64_t resultData = rightShiftCount >= 0 ? ((intData << i) >> i) >> rightShiftCount : ((intData << i) >> i) << (rightShiftCount * -1);
			std::vector<uint8_t> expectedResult((j / 8) + (j % 8 != 0 ? 1 : 0), 0);
			for(uint32_t k = 0; k < expectedResult.size(); k++)
			{
				expectedResult.at(k) = resultData >> ((expectedResult.size() - k - 1) * 8);
			}
			std::vector<uint8_t> result = BaseLib::BitReaderWriter::getPosition(signedData, i, j);
			if(result.size() != expectedResult.size() || !std::equal(result.begin(), result.end(), expectedResult.begin()))
			{
				std::cerr << "Error reading " << j << " bits from index " << i << ": Got " << BaseLib::HelperFunctions::getHexString(result) << " but expected " << BaseLib::HelperFunctions::getHexString(expectedResult) << std::endl;
			}
		}
	}

	for(uint32_t i = 0; i < data.size() * 8; i++) // i is start position
	{
		for(uint32_t j = 0; j <= 8; j++) // j represents length
		{
			int32_t rightShiftCount = 64 - j - i;
			uint64_t resultData = rightShiftCount >= 0 ? ((intData << i) >> i) >> rightShiftCount : ((intData << i) >> i) << (rightShiftCount * -1);
			if(rightShiftCount == 64) resultData = 0;
			uint8_t result = BaseLib::BitReaderWriter::getPosition8(data, i, j);
			if(result != resultData)
			{
				std::cerr << "Error reading " << j << " bits from index " << i << ": Got " << BaseLib::HelperFunctions::getHexString(result) << " but expected " << BaseLib::HelperFunctions::getHexString(resultData) << std::endl;
			}
		}
	}

	for(uint32_t i = 0; i < data.size() * 8; i++) // i is start position
	{
		for(uint32_t j = 0; j <= 16; j++) // j represents length
		{
			int32_t rightShiftCount = 64 - j - i;
			uint64_t resultData = rightShiftCount >= 0 ? ((intData << i) >> i) >> rightShiftCount : ((intData << i) >> i) << (rightShiftCount * -1);
			if(rightShiftCount == 64) resultData = 0;
			uint16_t result = BaseLib::BitReaderWriter::getPosition16(data, i, j);
			if(result != resultData)
			{
				std::cerr << "Error reading " << j << " bits from index " << i << ": Got " << BaseLib::HelperFunctions::getHexString(result) << " but expected " << BaseLib::HelperFunctions::getHexString(resultData) << std::endl;
			}
		}
	}

	for(uint32_t i = 0; i < data.size() * 8; i++) // i is start position
	{
		for(uint32_t j = 0; j <= 32; j++) // j represents length
		{
			int32_t rightShiftCount = 64 - j - i;
			uint64_t resultData = rightShiftCount >= 0 ? ((intData << i) >> i) >> rightShiftCount : ((intData << i) >> i) << (rightShiftCount * -1);
			if(rightShiftCount == 64) resultData = 0;
			uint32_t result = BaseLib::BitReaderWriter::getPosition32(data, i, j);
			if(result != resultData)
			{
				std::cerr << "Error reading " << j << " bits from index " << i << ": Got " << BaseLib::HelperFunctions::getHexString(result) << " but expected " << BaseLib::HelperFunctions::getHexString(resultData) << std::endl;
			}
		}
	}

	for(uint32_t i = 0; i < data.size() * 8; i++) // i is start position
	{
		for(uint32_t j = 0; j <= 64; j++) // j represents length
		{
			int32_t rightShiftCount = 64 - j - i;
			uint64_t resultData = rightShiftCount >= 0 ? ((intData << i) >> i) >> rightShiftCount : ((intData << i) >> i) << (rightShiftCount * -1);
			if(rightShiftCount == 64) resultData = 0;
			uint64_t result = BaseLib::BitReaderWriter::getPosition64(data, i, j);
			if(result != resultData)
			{
				std::cerr << "Error reading " << j << " bits from index " << i << ": Got " << BaseLib::HelperFunctions::getHexString(result) << " but expected " << BaseLib::HelperFunctions::getHexString(resultData) << std::endl;
			}
		}
	}

	for(uint32_t i = 0; i < data.size() * 8; i++) // i is start position
	{
		for(uint32_t j = 0; i + j <= 64; j++) // j represents length
		{
			uint64_t resultData = intDataReverse << (64 - j) >> i;
			for(uint32_t k = 0; k < i; k++)
			{
				resultData |= (uint64_t)1 << (64 - k - 1);
			}
			for(uint32_t k = i + j; k < 64; k++)
			{
				resultData |= (uint64_t)1 << (64 - k - 1);
			}
			std::vector<uint8_t> expectedData{ (uint8_t)(resultData >> 56), (uint8_t)(resultData >> 48), (uint8_t)(resultData >> 40), (uint8_t)(resultData >> 32), (uint8_t)(resultData >> 24), (uint8_t)(resultData >> 16), (uint8_t)(resultData >> 8), (uint8_t)resultData };

			std::vector<uint8_t> targetData(8, 0xFF);
			BaseLib::BitReaderWriter::setPosition(i, j, targetData, data);
			if(expectedData.size() != targetData.size() || !std::equal(targetData.begin(), targetData.end(), expectedData.begin()))
			{
				std::cerr << "Error writing " << j << " bits to index " << i << ": Got " << BaseLib::HelperFunctions::getHexString(targetData) << " but expected " << BaseLib::HelperFunctions::getHexString(expectedData) << std::endl;
			}
		}
	}

	for(uint32_t i = 0; i < data.size() * 8; i++) // i is start position
	{
		for(uint32_t j = 0; i + j <= 64; j++) // j represents length
		{
			uint64_t resultData = intDataReverse << (64 - j) >> i;
			for(uint32_t k = i + j; k < 64; k++)
			{
				resultData &= ~((uint64_t)1 << (64 - k - 1));
			}
			std::vector<uint8_t> expectedData{ (uint8_t)(resultData >> 56), (uint8_t)(resultData >> 48), (uint8_t)(resultData >> 40), (uint8_t)(resultData >> 32), (uint8_t)(resultData >> 24), (uint8_t)(resultData >> 16), (uint8_t)(resultData >> 8), (uint8_t)resultData };

			std::vector<uint8_t> targetData(8, 0);
			BaseLib::BitReaderWriter::setPosition(i, j, targetData, data);
			if(expectedData.size() != targetData.size() || !std::equal(targetData.begin(), targetData.end(), expectedData.begin()))
			{
				std::cerr << "Error writing " << j << " bits to index " << i << ": Got " << BaseLib::HelperFunctions::getHexString(targetData) << " but expected " << BaseLib::HelperFunctions::getHexString(expectedData) << std::endl;
			}
		}
	}

	for(uint32_t i = 0; i < data.size() * 8; i++) // i is start position
	{
		for(uint32_t j = 0; i + j <= 64; j++) // j represents length
		{
			uint64_t resultData = intDataReverse << (64 - j) >> i;
			for(uint32_t k = 0; k < i; k++)
			{
				resultData |= (uint64_t)1 << (64 - k - 1);
			}
			for(uint32_t k = i + j; k < 64; k++)
			{
				resultData |= (uint64_t)1 << (64 - k - 1);
			}
			std::vector<char> expectedData{ (char)(uint8_t)(resultData >> 56), (char)(uint8_t)(resultData >> 48), (char)(uint8_t)(resultData >> 40), (char)(uint8_t)(resultData >> 32), (char)(uint8_t)(resultData >> 24), (char)(uint8_t)(resultData >> 16), (char)(uint8_t)(resultData >> 8), (char)(uint8_t)resultData };

			std::vector<char> targetData(8, (char)(uint8_t)0xFF);
			BaseLib::BitReaderWriter::setPosition(i, j, targetData, data);
			if(expectedData.size() != targetData.size() || !std::equal(targetData.begin(), targetData.end(), expectedData.begin()))
			{
				std::cerr << "Error writing " << j << " bits to index " << i << ": Got " << BaseLib::HelperFunctions::getHexString(targetData) << " but expected " << BaseLib::HelperFunctions::getHexString(expectedData) << std::endl;
			}
		}
	}

	for(uint32_t i = 0; i < data.size() * 8; i++) // i is start position
	{
		for(uint32_t j = 0; i + j <= 64; j++) // j represents length
		{
			uint64_t resultData = intDataReverse << (64 - j) >> i;
			for(uint32_t k = i + j; k < 64; k++)
			{
				resultData &= ~((uint64_t)1 << (64 - k - 1));
			}
			std::vector<char> expectedData{ (char)(uint8_t)(resultData >> 56), (char)(uint8_t)(resultData >> 48), (char)(uint8_t)(resultData >> 40), (char)(uint8_t)(resultData >> 32), (char)(uint8_t)(resultData >> 24), (char)(uint8_t)(resultData >> 16), (char)(uint8_t)(resultData >> 8), (char)(uint8_t)resultData };

			std::vector<char> targetData(8, 0);
			BaseLib::BitReaderWriter::setPosition(i, j, targetData, data);
			if(expectedData.size() != targetData.size() || !std::equal(targetData.begin(), targetData.end(), expectedData.begin()))
			{
				std::cerr << "Error writing " << j << " bits to index " << i << ": Got " << BaseLib::HelperFunctions::getHexString(targetData) << " but expected " << BaseLib::HelperFunctions::getHexString(expectedData) << std::endl;
			}
		}
	}

	std::cout << "Finished testing BitReaderWriter." << std::endl << std::endl;
}

int main(int argc, char* argv[])
{
	_bl.reset(new BaseLib::SharedObjects(false));

	testJson(_bl.get());
	testBinaryRpc();
	testAnsiConversion();
	testBitReaderWriter();
	testWebSocket(_bl.get());
	testMath(_bl.get());
	testHelperFunctions(_bl.get());
	testCliCommandParsing(_bl.get());
	testHttp(_bl.get());

	return 0;
}
