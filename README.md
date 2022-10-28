****Implementation of Ad-Blokcer in RDK-B****

ad-blocker.c will be running in the gateway and it will reach the server for the verdict.\n
Server is also a linux machine in the LAN.
we will load the ad-host file to the local mongodb database in the linux machine.
we need to have mongod.service running locally in the linux machine.
Then we will run our server.py script in that machine, to act as a server.
Socket connetion will be established between the gateway and the server.
If any query comes from the Ad-Blocker from gateway it will send the reply by checking the database.
