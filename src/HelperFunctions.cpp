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

void testHelperFunctions(BaseLib::SharedObjects* bl)
{
    std::cout << "Testing HelperFunctions..." << std::endl;

    { //getTimeUuid()
        auto timeUuid = BaseLib::HelperFunctions::getTimeUuid();
        auto time = BaseLib::HelperFunctions::getTimeMicroseconds();
        if(timeUuid.size() != 53)
        {
            std::cerr << "Error creating time UUID (1)." << std::endl;
        }
        auto diff = std::abs(time - BaseLib::Math::getNumber64(timeUuid.substr(0, 16), true));
        if(diff > 1000000)
        {
            std::cerr << "Error creating time UUID (2)." << std::endl;
        }
    }

    std::cout << "Finished testing HelperFunctions." << std::endl << std::endl;
}

void testCliCommandParsing(BaseLib::SharedObjects* bl)
{
    std::cout << "Testing CLI command parsing..." << std::endl;
    try
    {
        {
            std::string command("pc USB300 0xF60201 0xFFB39400");
            std::vector<std::string> arguments;
            bool showHelp = false;
            if(BaseLib::HelperFunctions::checkCliCommand(command, "peers create", "pc", "", 3, arguments, showHelp))
            {
                if(arguments.size() != 3 || arguments.at(0) != "USB300" || arguments.at(1) != "0xF60201" || arguments.at(2) != "0xFFB39400")
                {
                    std::cerr << "Error parsing arguments." << std::endl;
                }
            }
            else std::cerr << "Error parsing arguments." << std::endl;
        }
    }
    catch(BaseLib::Rpc::JsonDecoderException& ex)
    {
        std::cerr << "Error: " << ex.what() << std::endl;
    }
    std::cout << "Finished testing CLI command parsing." << std::endl << std::endl;
}