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

#include "BitReaderWriter.h"

#include <iostream>

void testBitReaderWriter(BaseLib::SharedObjects* bl)
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

    //{{{ Test setPositionLE with uint8_t target prefilled with 1s
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
            BaseLib::BitReaderWriter::setPositionLE(i, j, targetData, data);
            if(expectedData.size() != targetData.size() || !std::equal(targetData.begin(), targetData.end(), expectedData.begin()))
            {
                std::cerr << "Error writing " << j << " bits to index " << i << ": Got " << BaseLib::HelperFunctions::getHexString(targetData) << " but expected " << BaseLib::HelperFunctions::getHexString(expectedData) << std::endl;
            }
        }
    }
    //}}}

    //{{{ Test setPositionLE with uint8_t target prefilled with 0s
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
            BaseLib::BitReaderWriter::setPositionLE(i, j, targetData, data);
            if(expectedData.size() != targetData.size() || !std::equal(targetData.begin(), targetData.end(), expectedData.begin()))
            {
                std::cerr << "Error writing " << j << " bits to index " << i << ": Got " << BaseLib::HelperFunctions::getHexString(targetData) << " but expected " << BaseLib::HelperFunctions::getHexString(expectedData) << std::endl;
            }
        }
    }
    //}}}

    //{{{ Test setPositionLE with char target prefilled with 1s
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
            BaseLib::BitReaderWriter::setPositionLE(i, j, targetData, data);
            if(expectedData.size() != targetData.size() || !std::equal(targetData.begin(), targetData.end(), expectedData.begin()))
            {
                std::cerr << "Error writing " << j << " bits to index " << i << ": Got " << BaseLib::HelperFunctions::getHexString(targetData) << " but expected " << BaseLib::HelperFunctions::getHexString(expectedData) << std::endl;
            }
        }
    }
    //}}}

    //{{{ Test setPositionLE with char target prefilled with 0s
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
            BaseLib::BitReaderWriter::setPositionLE(i, j, targetData, data);
            if(expectedData.size() != targetData.size() || !std::equal(targetData.begin(), targetData.end(), expectedData.begin()))
            {
                std::cerr << "Error writing " << j << " bits to index " << i << ": Got " << BaseLib::HelperFunctions::getHexString(targetData) << " but expected " << BaseLib::HelperFunctions::getHexString(expectedData) << std::endl;
            }
        }
    }
    //}}}

    //{{{ Test setPositionBE with uint8_t target prefilled with 1s
    for(uint32_t i = 0; i < data.size() * 8; i++) // i is start position
    {
        for(uint32_t j = 0; i + j <= 64; j++) // j represents length
        {
            uint64_t resultData = intData << (64 - j) >> i;
            for(uint32_t k = 0; k < i; k++)
            {
                resultData |= (uint64_t)1 << (64 - k - 1);
            }
            for(uint32_t k = i + j; k < 64; k++)
            {
                resultData |= (uint64_t)1 << (64 - k - 1);
            }
            std::vector<uint8_t> expectedData{ (uint8_t)(resultData >> 56u), (uint8_t)(resultData >> 48u), (uint8_t)(resultData >> 40u), (uint8_t)(resultData >> 32u), (uint8_t)(resultData >> 24u), (uint8_t)(resultData >> 16u), (uint8_t)(resultData >> 8u), (uint8_t)resultData };

            std::vector<uint8_t> targetData(8, 0xFF);
            BaseLib::BitReaderWriter::setPositionBE(i, j, targetData, data);
            if(expectedData.size() != targetData.size() || !std::equal(targetData.begin(), targetData.end(), expectedData.begin()))
            {
                std::cerr << "Error writing " << j << " bits to index " << i << ": Got " << BaseLib::HelperFunctions::getHexString(targetData) << " but expected " << BaseLib::HelperFunctions::getHexString(expectedData) << std::endl;
            }
        }
    }
    //}}}

    //{{{ Test setPositionBE with uint8_t target prefilled with 0s
    for(uint32_t i = 0; i < data.size() * 8; i++) // i is start position
    {
        for(uint32_t j = 0; i + j <= 64; j++) // j represents length
        {
            uint64_t resultData = intData << (64 - j) >> i;
            for(uint32_t k = i + j; k < 64; k++)
            {
                resultData &= ~((uint64_t)1 << (64 - k - 1));
            }
            std::vector<uint8_t> expectedData{ (uint8_t)(resultData >> 56), (uint8_t)(resultData >> 48), (uint8_t)(resultData >> 40), (uint8_t)(resultData >> 32), (uint8_t)(resultData >> 24), (uint8_t)(resultData >> 16), (uint8_t)(resultData >> 8), (uint8_t)resultData };

            std::vector<uint8_t> targetData(8, 0);
            BaseLib::BitReaderWriter::setPositionBE(i, j, targetData, data);
            if(expectedData.size() != targetData.size() || !std::equal(targetData.begin(), targetData.end(), expectedData.begin()))
            {
                std::cerr << "Error writing " << j << " bits to index " << i << ": Got " << BaseLib::HelperFunctions::getHexString(targetData) << " but expected " << BaseLib::HelperFunctions::getHexString(expectedData) << std::endl;
            }
        }
    }
    //}}}

    //{{{ Test setPositionBE with char target prefilled with 1s
    for(uint32_t i = 0; i < data.size() * 8; i++) // i is start position
    {
        for(uint32_t j = 0; i + j <= 64; j++) // j represents length
        {
            uint64_t resultData = intData << (64 - j) >> i;
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
            BaseLib::BitReaderWriter::setPositionBE(i, j, targetData, data);
            if(expectedData.size() != targetData.size() || !std::equal(targetData.begin(), targetData.end(), expectedData.begin()))
            {
                std::cerr << "Error writing " << j << " bits to index " << i << ": Got " << BaseLib::HelperFunctions::getHexString(targetData) << " but expected " << BaseLib::HelperFunctions::getHexString(expectedData) << std::endl;
            }
        }
    }
    //}}}

    //{{{ Test setPositionBE with char target prefilled with 0s
    for(uint32_t i = 0; i < data.size() * 8; i++) // i is start position
    {
        for(uint32_t j = 0; i + j <= 64; j++) // j represents length
        {
            uint64_t resultData = intData << (64 - j) >> i;
            for(uint32_t k = i + j; k < 64; k++)
            {
                resultData &= ~((uint64_t)1 << (64 - k - 1));
            }
            std::vector<char> expectedData{ (char)(uint8_t)(resultData >> 56), (char)(uint8_t)(resultData >> 48), (char)(uint8_t)(resultData >> 40), (char)(uint8_t)(resultData >> 32), (char)(uint8_t)(resultData >> 24), (char)(uint8_t)(resultData >> 16), (char)(uint8_t)(resultData >> 8), (char)(uint8_t)resultData };

            std::vector<char> targetData(8, 0);
            BaseLib::BitReaderWriter::setPositionBE(i, j, targetData, data);
            if(expectedData.size() != targetData.size() || !std::equal(targetData.begin(), targetData.end(), expectedData.begin()))
            {
                std::cerr << "Error writing " << j << " bits to index " << i << ": Got " << BaseLib::HelperFunctions::getHexString(targetData) << " but expected " << BaseLib::HelperFunctions::getHexString(expectedData) << std::endl;
            }
        }
    }
    //}}}

    //{{{ Test setPositionBE with uint8_t target prefilled with 0s and source smaller than copied size.
    {
        std::vector<uint8_t> smallData{0xAB};
        std::vector<uint8_t> targetData(8, 0);
        std::vector<uint8_t> expectedData{0, 0, 0, 0, 0xAB, 0, 0, 0};
        BaseLib::BitReaderWriter::setPositionBE(8, 32, targetData, smallData);
        if(targetData.size() != expectedData.size() || !std::equal(targetData.begin(), targetData.end(), expectedData.begin()))
        {
            std::cerr << "Error writing 32 bits to index 8 with source array smaller than 32 bits." << std::endl;
        }
    }
    //}}}

    //{{{ Test setPositionBE with char target prefilled with 0s and source smaller than copied size.
    {
        std::vector<uint8_t> smallData{0xAB};
        std::vector<char> targetData(8, 0);
        std::vector<char> expectedData{0, 0, 0, 0, (char)(uint8_t)0xAB, 0, 0, 0};
        BaseLib::BitReaderWriter::setPositionBE(8, 32, targetData, smallData);
        if(targetData.size() != expectedData.size() || !std::equal(targetData.begin(), targetData.end(), expectedData.begin()))
        {
            std::cerr << "Error writing 32 bits to index 8 with source array smaller than 32 bits." << std::endl;
        }
    }
    //}}}

    std::cout << "Finished testing BitReaderWriter." << std::endl << std::endl;
}