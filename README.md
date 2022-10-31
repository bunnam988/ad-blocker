***Implementation of Ad-Blocker in RDK-B***
* Ad-blocker.c will be running in the gateway and it will reach the server for the verdict.
* Server code is python based and can be deployed within LAN or internet.
* we will load the ad-host file to the mongodb database in the server.
* we need to have mongod.service running in the server.
* Socket connection will be established between the gateway and the server.
* If any query comes from the Ad-Blocker to server,it will send the reply by checking the database.
