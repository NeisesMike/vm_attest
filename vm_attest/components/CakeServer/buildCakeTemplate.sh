
echo "append the cakeml files for the cakeml_am" &&
(rm cake_server.cml || true) &&
touch cake_server.cml &&

# util src
cat cakeml_am/util/ByteString.sml >> cake_server.cml && 
cat cakeml_am/util/CoqDefaults.sml >> cake_server.cml && 
cat cakeml_am/JSON/Json.sml >> cake_server.cml && 

# copland src
cat cakeml_am/copland/Instr.sml >> cake_server.cml && 
cat cakeml_am/copland/AM.sml >> cake_server.cml && 
cat cakeml_am/copland/VM.sml >> cake_server.cml && 
cat cakeml_am/copland/json/CoplandToJson.sml >> cake_server.cml && 
cat cakeml_am/copland/json/JsonToCopland.sml >> cake_server.cml && 

# crypto src
cat cakeml_am/system/crypto/CryptoFFI.sml >> cake_server.cml && 

# socket src
cat cakeml_am/system/sockets/SocketFFI.sml >> cake_server.cml && 

# dataport src
cat cakeml_am/system/dataports/DataportFFI.sml >> cake_server.cml && 

# server_am src
cat cakeml_am/am/Measurements.sml >> cake_server.cml && 
cat cakeml_am/am/CommTypes.sml >> cake_server.cml && 
cat cakeml_am/am/CommUtil.sml >> cake_server.cml && 
cat cakeml_am/am/ServerAm.sml >> cake_server.cml && 

# plat_am src
cat destination.cml >> cake_server.cml && 

echo "compile the dest cakeml" &&
../../../cake32 --stack_size=1 --heap_size=1 < ./cake_server.cml > cakeServer.S &&
sed -i "s/cdecl(main)/cdecl(run)/g" cakeServer.S

