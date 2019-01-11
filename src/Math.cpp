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

#include "Math.h"

void testMath(BaseLib::SharedObjects* bl)
{
    std::cout << "Testing Math..." << std::endl;
    try
    {
        {
            std::string numberString = "0xFFB39400";
            auto number1 = BaseLib::Math::getNumber(numberString, true);
            auto number2 = BaseLib::Math::getNumber(numberString, false);
            if((uint32_t)number1 != 0xFFB39400 || (uint32_t)number2 != 0xFFB39400)
            {
                std::cerr << "Error parsing number (1)." << std::endl;
            }
        }
        {
            std::string numberString = "0xFFB39400FFB39400";
            auto number1 = BaseLib::Math::getUnsignedNumber64(numberString, true);
            auto number2 = BaseLib::Math::getUnsignedNumber64(numberString, false);
            if((uint64_t)number1 != 0xFFB39400FFB39400ull || (uint64_t)number2 != 0xFFB39400FFB39400ull)
            {
                std::cerr << "Error parsing number (2)." << std::endl;
            }
        }
    }
    catch(BaseLib::Rpc::JsonDecoderException& ex)
    {
        std::cerr << "Error: " << ex.what() << std::endl;
    }
    std::cout << "Finished testing Math." << std::endl << std::endl;
}