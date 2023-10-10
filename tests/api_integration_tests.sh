#!/bin/bash

# Clean the test dir
rm -rf ../crud/test

echo '{"a":2}' > ../crud/test/1 

SERVER_PATH=../build/bin/server
CONFIG_PATH=../configs/api_test_config

# start server in the background
$SERVER_PATH $CONFIG_PATH &
sleep 1

SERVER_PID=$!

TIMEOUT=1
SERVER_IP=localhost
SERVER_PORT=8080

# POST Test
POST_REQ_SUCCESS=0 
echo "POST test"
timeout $TIMEOUT curl -s -i -X POST -H "Host:" -H "User-Agent:" $SERVER_IP:$SERVER_PORT/api/test -d '{"a":1}'
sleep 1
echo "Compare the result" ../
diff ../crud/test/2 POST_test #testing for persistent storage across server restarts
MATCHES=$?
echo $MATCHES
if [ $MATCHES -eq 1 ];
then
  echo "Test POST request Failed"
else 
  POST_REQ_SUCCESS=1 
  echo "Test POST Request Succeeded"
fi

DELETE_REQ_SUCCESS=0

# DELETE Test
timeout $TIMEOUT curl -s -i -X DELETE -H "Host:" -H "User-Agent:" $SERVER_IP:$SERVER_PORT/api/test/1

# Check if file exists or not
FILE=../crud/test/1

test -f "$FILE";
RESULT=$?

if [ $RESULT -ne 1 ]; 
then
    echo "Test DELETE request failed"
else 
    DELETE_REQ_SUCCESS=1
    echo "Test DELETE request succeeded"
fi

# GET Test
GET_REQ_SUCCESS=0 
# use POST to add file
timeout $TIMEOUT curl -s -i -X POST -H "Host:" -H "User-Agent:" $SERVER_IP:$SERVER_PORT/api/test -d '{"a":1}'
sleep 1
# store GET response for file just added
curl $SERVER_IP:$SERVER_PORT/api/test/1 > ../crud/test/getTest

# compare POST request file with GET response file
diff ../crud/test/1 ../crud/test/getTest
MATCHES2=$?
if [ $MATCHES2 -eq 1 ];
then
  echo "Test GET request Failed"
else 
  GET_REQ_SUCCESS=1 
  echo "Test GET Request Succeeded"
fi

# LIST Test
LIST_REQ_SUCCESS=0 
# use POST to add another file so we have 2 files in dir
timeout $TIMEOUT curl -s -i -X POST -H "Host:" -H "User-Agent:" $SERVER_IP:$SERVER_PORT/api/test -d '{"b":2}'
sleep 1
# store LIST response
curl $SERVER_IP:$SERVER_PORT/api/test > ../crud/test/getTest2

# compare LIST response to expected output
diff "[1,2]" ../crud/test/getTest2
MATCHES3=$?
if [ $MATCHES3 -eq 1 ];
then
  echo "Test LIST request Failed"
else 
  LIST_REQ_SUCCESS=1 
  echo "Test LIST Request Succeeded"
fi

# PUT Test
PUT_REQ_SUCCESS=0 
# store PUT response of modified file
timeout $TIMEOUT curl -s -i -X PUT -H "Host:" -H "User-Agent:" $SERVER_IP:$SERVER_PORT/api/test/1 -d '{"c":3}' > ../crud/test/putTest
sleep 1

# compare PUT response to expected output
diff '{"c":3}' ../crud/test/putTest
MATCHES4=$?
if [ $MATCHES4 -eq 1 ];
then
  echo "Test PUT request Failed"
else 
  PUT_REQ_SUCCESS=1 
  echo "Test PUT Request Succeeded"
fi

rm -rf ../crud/test
# kill server and return test results
kill -9 $SERVER_PID

if [ $POST_REQ_SUCCESS -eq 1 ] && [ $DELETE_REQ_SUCCESS -eq 1 ] && [ $GET_REQ_SUCCESS -eq 1 ] && [ $LIST_REQ_SUCCESS -eq 1 ] && [ $PUT_REQ_SUCCESS -eq 1 ];
then
  exit 0
fi

exit 1