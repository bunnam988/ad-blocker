****Implementation of Ad-Blokcer in RDK-B****
1.ad-blocker.c will be running in the gateway and it will reach the server for the verdict.
2.Server is also a linux machine in the LAN.
3.we will load the ad-host file to the local mongodb database in the linux machine.
4.we need to have mongod.service running locally in the linux machine.
5.Then we will run our server.py script in that machine, to act as a server.
6.Socket connetion will be established between the gateway and the server.
7.If any query comes from the Ad-Blocker from gateway it will send the reply by checking the database.
