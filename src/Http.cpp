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

#include "Http.h"

void testHttp(BaseLib::SharedObjects* bl)
{
    std::cout << "Testing HTTP parsing..." << std::endl;
    try
    {
        {
            auto packetWithInvalidContentLength = bl->hf.getBinary("504F5354202F20485454502F312E310A557365725F4167656E743A20484D2D584D4C5250432D436C69656E740A486F73743A2052617370692D48470A436F6E6E656374696F6E3A204B6565702D416C6976650A436F6E74656E742D547970653A20746578742F786D6C0A436F6E74656E742D4C656E6774683A203330320A0A3C3F786D6C2076657273696F6E3D22312E302220656E636F64696E673D2269736F2D383835392D31223F3E0A3C6D6574686F6443616C6C3E0A3C6D6574686F644E616D653E67657456616C75653C2F6D6574686F644E616D653E0A3C706172616D733E0A203C706172616D3E0A20203C76616C75653E0A2020203C696E743E343C2F696E743E0A20203C2F76616C75653E0A203C2F706172616D3E0A203C706172616D3E0A20203C76616C75653E0A2020203C696E743E313C2F696E743E0A20203C2F76616C75653E0A203C2F706172616D3E0A203C706172616D3E0A20203C76616C75653E0A2020203C737472696E673E53544154453C2F737472696E673E0A20203C2F76616C75653E0A203C2F706172616D3E0A3C2F706172616D733E0A3C2F6D6574686F6443616C6C3E0A0A");
            BaseLib::Http http;

            { //Test too small content length
                auto processedBytes = http.process(packetWithInvalidContentLength.data(), packetWithInvalidContentLength.size());
                if(!http.isFinished() || processedBytes != (signed)packetWithInvalidContentLength.size())
                {
                    std::cerr << "Error processing HTTP packet (1)." << std::endl;
                }
            }

            { //Test reset
                http.reset();
                http.process(packetWithInvalidContentLength.data(), packetWithInvalidContentLength.size());
                if(!http.isFinished())
                {
                    std::cerr << "Error processing HTTP packet (2)." << std::endl;
                }
            }

            auto packet2 = bl->hf.getBinary("504f5354202f20485454502f312e310d0a557365725f4167656e743a20484d2d584d4c5250432d436c69656e740d0a486f73743a2052617370692d48470d0a436f6e6e656374696f6e3a204b6565702d416c6976650d0a436f6e74656e742d547970653a20746578742f706c61696e0d0a436f6e74656e742d4c656e6774683a20350d0a0d0a546573740a");
            { //Process again without reset
                http.process(packet2.data(), packet2.size());
                std::string content(http.getContent().data(), http.getContentSize());
                if(content != "Test\n")
                {
                    std::cerr << "Error processing HTTP packet (3)." << std::endl;
                }
            }
        }
    }
    catch(BaseLib::Rpc::JsonDecoderException& ex)
    {
        std::cerr << "Error: " << ex.what() << std::endl;
    }
    std::cout << "Finished testing HTTP parsing." << std::endl << std::endl;
}