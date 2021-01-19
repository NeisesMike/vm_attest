
echo "append the cakeml files for the cakeml-am" &&
(rm cake_server.cml || true) &&
touch cake_server.cml &&

# util src
cat cakeml-am/util/ByteString.sml >> cake_server.cml && 
cat cakeml-am/util/CoqDefaults.sml >> cake_server.cml && 
cat cakeml-am/util/Json.sml >> cake_server.cml && 

# copland src
cat cakeml-am/copland/Instr.sml >> cake_server.cml && 
cat cakeml-am/copland/AM.sml >> cake_server.cml && 
cat cakeml-am/copland/VM.sml >> cake_server.cml && 
cat cakeml-am/copland/json/CoplandToJson.sml >> cake_server.cml && 
cat cakeml-am/copland/json/JsonToCopland.sml >> cake_server.cml && 

# crypto src
#cat cakeml-am/system/crypto/CryptoFFI.sml >> cake_server.cml && 

# socket src
cat cakeml-am/system/sockets/SocketFFI.sml >> cake_server.cml && 

# dataport src
cat cakeml-am/system/dataports/DataportFFI.sml >> cake_server.cml && 

# server_am src
cat cakeml-am/am/Measurements.sml >> cake_server.cml && 
cat cakeml-am/am/CommTypes.sml >> cake_server.cml && 
cat cakeml-am/am/ServerAm.sml >> cake_server.cml && 

# plat_am src
cat destination.cml >> cake_server.cml && 

echo "compile the dest cakeml" &&
../../../cake64 --target=arm8 --stack_size=1 --heap_size=1 < ./cake_server.cml > cakeServer.S &&
sed -i "s/cdecl(main)/cdecl(run)/g" cakeServer.S &&

