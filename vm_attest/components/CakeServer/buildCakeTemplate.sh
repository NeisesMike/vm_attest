
echo "append the cakeml files for the cakeml_am" &&
(rm cake_server.cml || true) &&
touch cake_server.cml &&
cat cakeml_am/util/ByteString.sml >> cake_server.cml && 
cat cakeml_am/crypto/CryptoFFI.sml >> cake_server.cml && 
cat cakeml_am/crypto/Aes256.sml >> cake_server.cml &&
cat cakeml_am/crypto/Random.sml >> cake_server.cml && 
cat cakeml_am/sockets/SocketFFI.sml >> cake_server.cml && 
cat cakeml_am/util/CoqDefaults.sml >> cake_server.cml && 
cat cakeml_am/interp/CoplandLang.sml >> cake_server.cml && 
cat cakeml_am/am/Measurements.sml >> cake_server.cml && 
cat cakeml_am/am/CommTypes.sml >> cake_server.cml && 
cat cakeml_am/interp/Instr.sml >> cake_server.cml && 
cat cakeml_am/JSON/Json.sml >> cake_server.cml && 
cat cakeml_am/JSON/CoplandToJson.sml >> cake_server.cml && 
cat cakeml_am/JSON/JsonToCopland.sml >> cake_server.cml && 
cat cakeml_am/am/CommUtil.sml >> cake_server.cml && 
cat cakeml_am/interp/Eval.sml >> cake_server.cml && 
cat cakeml_am/interp/VM.sml >> cake_server.cml && 
cat destination.cml >> cake_server.cml && 

echo "compile the dest cakeml" &&
../../../cake32 --stack_size=1 --heap_size=1 < ./cake_server.cml > cakeServer.S &&
sed -i "s/cdecl(main)/cdecl(run)/g" cakeServer.S

