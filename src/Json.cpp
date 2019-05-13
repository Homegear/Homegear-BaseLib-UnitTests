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

#include "Json.h"

#include <iostream>
#include <iomanip>

void testJson(BaseLib::SharedObjects* bl)
{
    std::cout << "Testing JSON en-/decoding..." << std::endl;
    try
    {
        {
            std::string jsonInput("{\"on\":true, \"off\":    false, \"bla\":null, \"blupp\":  [5.643,false , null ,true ],\"blupp2\":[ 5.643,false,null,true], \"s\\u00e4t\":255.63456, \"bri\":-255,\"hue\":10000, \"bli\":{\"a\": 2,\"b\":false}    ,     \"e\"  :   -34785326,\"f\":-0.547887237, \"g\":{},\"h\":[], \"i\" : {    }  , \"j\" : [    ] , \"k\": {} , \"l\": [], \"m\": \"\\\"H\\u00e4ll\\u00fc\\\" W\\u00f6lt \\\"Hallo\\\" Welt\\\"\" }");
            BaseLib::Rpc::JsonDecoder jsonDecoder(bl);
            BaseLib::PVariable variable = jsonDecoder.decode(jsonInput);
            if(variable->type != BaseLib::VariableType::tStruct)
            {
                std::cerr << "JSON decoding test failed: Container is not of type struct." << std::endl;
            }
            else
            {
                if(variable->structValue->size() != 18)
                {
                    std::cerr << "JSON decoding test failed: Struct size is not 18." << std::endl;
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
                       variable->structValue->find("m") == variable->structValue->end() ||
                       variable->structValue->find("off") == variable->structValue->end() ||
                       variable->structValue->find("on") == variable->structValue->end() ||
                       variable->structValue->find("sät") == variable->structValue->end()
                       )
                    {
                        std::cerr << "JSON decoding test failed: Couldn't find all variable names." << std::endl;
                    }

                    BaseLib::PVariable structValue = variable->structValue->at("bla");
                    if(structValue->type != BaseLib::VariableType::tVoid)
                        std::cerr << "JSON decoding test failed: Variable \"bla\" has wrong type." << std::endl;

                    structValue = variable->structValue->at("bli");
                    if(structValue->type != BaseLib::VariableType::tStruct)
                        std::cerr << "JSON decoding test failed: Variable \"bli\" has wrong type." << std::endl;
                    else if(structValue->structValue->size() != 2)
                        std::cerr << "JSON decoding test failed: Variable \"bli\" has wrong size." << std::endl;
                    else if(structValue->structValue->find("a") == structValue->structValue->end() || structValue->structValue->find("b") == structValue->structValue->end())
                        std::cerr << "JSON decoding test failed: Couldn't find all variable names in \"bli\"."
                                  << std::endl;
                    else if(structValue->structValue->at("a")->type != BaseLib::VariableType::tInteger || structValue->structValue->at("a")->integerValue != 2 ||
                            structValue->structValue->at("b")->type != BaseLib::VariableType::tBoolean || structValue->structValue->at("b")->booleanValue != false)
                    {
                        std::cerr << "JSON decoding test failed: Variables in \"bli\" have wrong type or value."
                                  << std::endl;
                    }

                    structValue = variable->structValue->at("blupp");
                    if(structValue->type != BaseLib::VariableType::tArray)
                        std::cerr << "JSON decoding test failed: Variable \"blupp\" has wrong type." << std::endl;
                    else if(structValue->arrayValue->size() != 4)
                        std::cerr << "JSON decoding test failed: Variable \"blupp\" has wrong size." << std::endl;
                    else if(structValue->arrayValue->at(0)->type != BaseLib::VariableType::tFloat || structValue->arrayValue->at(0)->floatValue != 5.643 ||
                            structValue->arrayValue->at(1)->type != BaseLib::VariableType::tBoolean || structValue->arrayValue->at(1)->booleanValue != false ||
                            structValue->arrayValue->at(2)->type != BaseLib::VariableType::tVoid ||
                            structValue->arrayValue->at(3)->type != BaseLib::VariableType::tBoolean || structValue->arrayValue->at(3)->booleanValue != true
                            )
                    {
                        std::cerr << "JSON decoding test failed: Variables in \"blupp\" have wrong type or value."
                                  << std::endl;
                    }

                    structValue = variable->structValue->at("blupp2");
                    if(structValue->type != BaseLib::VariableType::tArray)
                        std::cerr << "JSON decoding test failed: Variable \"blupp2\" has wrong type." << std::endl;
                    else if(structValue->arrayValue->size() != 4)
                        std::cerr << "JSON decoding test failed: Variable \"blupp2\" has wrong size." << std::endl;
                    else if(structValue->arrayValue->at(0)->type != BaseLib::VariableType::tFloat || structValue->arrayValue->at(0)->floatValue != 5.643 ||
                            structValue->arrayValue->at(1)->type != BaseLib::VariableType::tBoolean || structValue->arrayValue->at(1)->booleanValue != false ||
                            structValue->arrayValue->at(2)->type != BaseLib::VariableType::tVoid ||
                            structValue->arrayValue->at(3)->type != BaseLib::VariableType::tBoolean || structValue->arrayValue->at(3)->booleanValue != true
                            )
                    {
                        std::cerr << "JSON decoding test failed: Variables in \"blupp2\" have wrong type or value."
                                  << std::endl;
                    }

                    structValue = variable->structValue->at("bri");
                    if(structValue->type != BaseLib::VariableType::tInteger || structValue->integerValue != -255)
                        std::cerr << "JSON decoding test failed: Variable \"bri\" has wrong type or value."
                                  << std::endl;

                    structValue = variable->structValue->at("e");
                    if(structValue->type != BaseLib::VariableType::tInteger || structValue->integerValue != -34785326)
                        std::cerr << "JSON decoding test failed: Variable \"e\" has wrong type or value." << std::endl;

                    structValue = variable->structValue->at("f");
                    if(structValue->type != BaseLib::VariableType::tFloat || structValue->floatValue != -0.547887237)
                        std::cerr << "JSON decoding test failed: Variable \"f\" has wrong type or value. Value is: "
                                  << std::fixed << std::setprecision(15) << structValue->floatValue << std::endl;

                    structValue = variable->structValue->at("g");
                    if(structValue->type != BaseLib::VariableType::tStruct || structValue->structValue->size() != 0)
                        std::cerr << "JSON decoding test failed: Variable \"g\" has wrong type or value." << std::endl;

                    structValue = variable->structValue->at("h");
                    if(structValue->type != BaseLib::VariableType::tArray || structValue->arrayValue->size() != 0)
                        std::cerr << "JSON decoding test failed: Variable \"h\" has wrong type or value." << std::endl;

                    structValue = variable->structValue->at("hue");
                    if(structValue->type != BaseLib::VariableType::tInteger || structValue->integerValue != 10000)
                        std::cerr << "JSON decoding test failed: Variable \"hue\" has wrong type or value."
                                  << std::endl;

                    structValue = variable->structValue->at("i");
                    if(structValue->type != BaseLib::VariableType::tStruct || structValue->structValue->size() != 0)
                        std::cerr << "JSON decoding test failed: Variable \"i\" has wrong type or value." << std::endl;

                    structValue = variable->structValue->at("j");
                    if(structValue->type != BaseLib::VariableType::tArray || structValue->arrayValue->size() != 0)
                        std::cerr << "JSON decoding test failed: Variable \"j\" has wrong type or value." << std::endl;

                    structValue = variable->structValue->at("k");
                    if(structValue->type != BaseLib::VariableType::tStruct || structValue->structValue->size() != 0)
                        std::cerr << "JSON decoding test failed: Variable \"k\" has wrong type or value." << std::endl;

                    structValue = variable->structValue->at("l");
                    if(structValue->type != BaseLib::VariableType::tArray || structValue->arrayValue->size() != 0)
                        std::cerr << "JSON decoding test failed: Variable \"l\" has wrong type or value." << std::endl;

                    structValue = variable->structValue->at("m");
                    if(structValue->type != BaseLib::VariableType::tString || structValue->stringValue != "\"Hällü\" Wölt \"Hallo\" Welt\"")
                        std::cerr << "JSON decoding test failed: Variable \"m\" has wrong type or value." << std::endl;

                    structValue = variable->structValue->at("off");
                    if(structValue->type != BaseLib::VariableType::tBoolean || structValue->booleanValue != false)
                        std::cerr << "JSON decoding test failed: Variable \"off\" has wrong type or value."
                                  << std::endl;

                    structValue = variable->structValue->at("on");
                    if(structValue->type != BaseLib::VariableType::tBoolean || structValue->booleanValue != true)
                        std::cerr << "JSON decoding test failed: Variable \"on\" has wrong type or value." << std::endl;

                    structValue = variable->structValue->at("sät");
                    if(structValue->type != BaseLib::VariableType::tFloat || (structValue->floatValue != 255.63456 && structValue->floatValue != 255.634559999999993))
                        std::cerr << "JSON decoding test failed: Variable \"sät\" has wrong type or value. Value is: "
                                  << std::fixed << std::setprecision(15) << structValue->floatValue << std::endl;
                }
            }

            BaseLib::Rpc::JsonEncoder jsonEncoder(bl);
            std::string jsonOutput;
            jsonEncoder.encode(variable, jsonOutput);
            if(jsonOutput != "{\"bla\":null,\"bli\":{\"a\":2,\"b\":false},\"blupp\":[5.643000000000000,false,null,true],\"blupp2\":[5.643000000000000,false,null,true],\"bri\":-255,\"e\":-34785326,\"f\":-0.547887237000000,\"g\":{},\"h\":[],\"hue\":10000,\"i\":{},\"j\":[],\"k\":{},\"l\":[],\"m\":\"\\\"H\\u00E4ll\\u00FC\\\" W\\u00F6lt \\\"Hallo\\\" Welt\\\"\",\"off\":false,\"on\":true,\"s\\u00E4t\":255.634559999999993}")
            {
                std::cerr << "JSON encoding test failed." << std::endl;
            }
        }

        {
            auto variable = std::make_shared<BaseLib::Variable>(BaseLib::VariableType::tArray);
            variable->arrayValue->push_back(std::make_shared<BaseLib::Variable>(u8"z\u00df\u6c34\U0001d10b")); //u8"zß水𝄋"
            BaseLib::Rpc::JsonEncoder jsonEncoder(bl);
            std::string jsonOutput;
            jsonEncoder.encode(variable, jsonOutput);
            if(jsonOutput != "[\"z\\u00DF\\u6C34\\uD834\\uDD0B\"]")
            {
                std::cerr << "JSON encoding test of unicode characters failed." << std::endl;
            }
            std::vector<char> jsonOutput2;
            jsonEncoder.encode(variable, jsonOutput2);
            std::string jsonOutput3(jsonOutput2.data(), jsonOutput2.size());
            if(jsonOutput3 != "[\"z\\u00DF\\u6C34\\uD834\\uDD0B\"]")
            {
                std::cerr << "JSON encoding test of unicode characters failed (2)." << std::endl;
            }

            BaseLib::Rpc::JsonDecoder jsonDecoder(bl);
            variable = jsonDecoder.decode(jsonOutput);
            if(variable->arrayValue->empty() || variable->arrayValue->at(0)->stringValue != u8"z\u00df\u6c34\U0001d10b")
            {
                std::cerr << "JSON decoding test of unicode characters failed." << std::endl;
            }

            variable = jsonDecoder.decode(jsonOutput2);
            if(variable->arrayValue->empty() || variable->arrayValue->at(0)->stringValue != u8"z\u00df\u6c34\U0001d10b")
            {
                std::cerr << "JSON decoding test of unicode characters failed (2)." << std::endl;
            }

            std::string jsonString = "z\\u00DF\\u6C34\\uD834\\uDD0B";
            auto result = jsonDecoder.decodeString(jsonString);
            if(result != u8"z\u00df\u6c34\U0001d10b")
            {
                std::cerr << "JSON decoding test of unicode characters failed (3)." << std::endl;
            }
        }

        { //Encode ANSI and invalid Strings
            BaseLib::Rpc::JsonEncoder jsonEncoder(bl);

            std::string ansiString = "Temperature: 22.5";
            ansiString.push_back(0xB0); //0xB0 is an invalid UTF-8 character.
            ansiString.push_back('C');
            BaseLib::PVariable jsonArray = std::make_shared<BaseLib::Variable>(BaseLib::VariableType::tArray);
            jsonArray->arrayValue->emplace_back(std::make_shared<BaseLib::Variable>(ansiString));
            std::string result;
            jsonEncoder.encode(jsonArray, result);
            if(result != R"(["Temperature: 22.5\u00B0C"])")
            {
                std::cerr << "JSON encoding test of invalid characters failed (1)." << std::endl;
            }

            std::vector<char> result2;
            jsonEncoder.encode(jsonArray, result2);
            if(std::string(result2.begin(), result2.end()) != R"(["Temperature: 22.5\u00B0C"])")
            {
                std::cerr << "JSON encoding test of invalid characters failed (2)." << std::endl;
            }

            std::string result3 = jsonEncoder.encodeString(ansiString);
            if(result3 != R"(Temperature: 22.5\u00B0C)")
            {
                std::cerr << "JSON encoding test of invalid characters failed (3)." << std::endl;
            }

            ansiString = bl->hf.getBinaryString("47E4737465776F686E756E67"); //The "ä" (E4) is a valid UTF-8 start byte for a three byte sequence.
            std::string result4 = jsonEncoder.encodeString(ansiString);
            if(result4 != R"(G\u00E4stewohnung)")
            {
                std::cerr << "JSON encoding test of invalid characters failed (4)." << std::endl;
            }
        }

        { //Decode large numbers
            BaseLib::Rpc::JsonDecoder jsonDecoder(bl);
            std::string largeNumber = R"([2147483648])";
            std::string largeNumber2 = R"([-2147483648])";
            std::string largeNumber3 = R"([4294967295])";
            std::string largeNumber4 = R"([922337203685477580])";
            std::string largeNumber5 = R"([9223372036854775807])";
            std::string largeNumber6 = R"([9223372036854775808])";
            std::string largeNumber7 = R"([-922337203685477580])";
            std::string largeNumber8 = R"([18446744073709551615])";
            std::string largeNumber9 = R"([184467440737095516159])";

            auto result = jsonDecoder.decode(largeNumber);
            if(result->arrayValue->at(0)->type != BaseLib::VariableType::tInteger64 || result->arrayValue->at(0)->integerValue64 != 2147483648ll)
            {
                std::cerr << "JSON decoding test of 64 bit integer failed." << std::endl;
            }

            result = jsonDecoder.decode(largeNumber2);
            if(result->arrayValue->at(0)->type != BaseLib::VariableType::tInteger || result->arrayValue->at(0)->integerValue != -2147483648ll)
            {
                std::cerr << "JSON decoding test of 32 bit negative integer failed." << std::endl;
            }

            result = jsonDecoder.decode(largeNumber3);
            if(result->arrayValue->at(0)->type != BaseLib::VariableType::tInteger64 || result->arrayValue->at(0)->integerValue64 != 4294967295ll)
            {
                std::cerr << "JSON decoding test of 64 bit integer failed." << std::endl;
            }

            result = jsonDecoder.decode(largeNumber4);
            if(result->arrayValue->at(0)->type != BaseLib::VariableType::tInteger64 || result->arrayValue->at(0)->integerValue64 != 922337203685477580ll)
            {
                std::cerr << "JSON decoding test of 64 bit integer failed." << std::endl;
            }

            result = jsonDecoder.decode(largeNumber5);
            if(result->arrayValue->at(0)->type != BaseLib::VariableType::tFloat || result->arrayValue->at(0)->floatValue != 9223372036854775807.0)
            {
                std::cerr << "JSON decoding test of very large integer failed (1)." << std::endl;
            }

            result = jsonDecoder.decode(largeNumber6);
            if(result->arrayValue->at(0)->type != BaseLib::VariableType::tFloat || result->arrayValue->at(0)->floatValue != 9223372036854775808.0)
            {
                std::cerr << "JSON decoding test of very large integer failed (2)." << std::endl;
            }

            result = jsonDecoder.decode(largeNumber7);
            if(result->arrayValue->at(0)->type != BaseLib::VariableType::tInteger64 || result->arrayValue->at(0)->integerValue64 != -922337203685477580ll)
            {
                std::cerr << "JSON decoding test of 64 bit negative integer failed." << std::endl;
            }

            result = jsonDecoder.decode(largeNumber8);
            if(result->arrayValue->at(0)->type != BaseLib::VariableType::tFloat || result->arrayValue->at(0)->floatValue != 18446744073709551615.0)
            {
                std::cerr << "JSON decoding test of very large integer failed (3)." << std::endl;
            }

            result = jsonDecoder.decode(largeNumber9);
            if(result->arrayValue->at(0)->type != BaseLib::VariableType::tFloat || result->arrayValue->at(0)->floatValue != 184467440737095516159.0)
            {
                std::cerr << "JSON decoding test of very large integer failed (4)." << std::endl;
            }
        }

        { //Load and decode large JSON file
            std::string rawFlows;
            std::string flowsFile = "data/flows.json";
            if(!bl->io.fileExists(flowsFile)) std::cerr << "Warning: flows.json doesn't exist in \"data\"" << std::endl;
            else rawFlows = bl->io.getFileContent(flowsFile);
            BaseLib::HelperFunctions::trim(rawFlows);

            BaseLib::Rpc::JsonDecoder jsonDecoder;
            auto flows = jsonDecoder.decode(rawFlows);

            BaseLib::Rpc::JsonEncoder jsonEncoder;
            std::string rawFlows2;
            jsonEncoder.encode(flows, rawFlows2);

            if(rawFlows2 != rawFlows)
            {
                std::cerr << "Decoding and encoding of large JSON file (flows.json) failed." << std::endl;
            }
        }
    }
    catch(BaseLib::Rpc::JsonDecoderException& ex)
    {
        std::cerr << "Error: " << ex.what() << std::endl;
    }
    std::cout << "Finished testing JSON en-/decoding." << std::endl << std::endl;
}