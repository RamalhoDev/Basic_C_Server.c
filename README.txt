Project Basic Server C

My goal with this project is to make a functional server using C language.

How it works ?

The server will receive a GET request from the browser, the request will contain a name of a file, that can be inside the folder files.
Then, the server should look for the file, if there is a file, the server should return a response with the file to the browser; if there
isn't a file, the server will respond with a 404 http status code and a html page saying the file wasn't found; if the request isn't a GET
request, the server should return 400 http status code and a html page saying that it was a bad request.

How to compile ?

gcc src/*.c -pthread -o ./bin/server

How to run ?

./bin/server

This server was also a project from my class called Programming Language 2 in Federal University of Paraiba.