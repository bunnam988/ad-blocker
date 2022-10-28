***Implementation of Ad-Blokcer in RDK-B***
  <br ad-blocker.c will be running in the gateway and it will reach the server for the verdict./>
  <br Server is also a linux machine in the LAN./>
  <br we will load the ad-host file to the local mongodb database in the linux machine./>
  <br we need to have mongod.service running locally in the linux machine./>
  <br Then we will run our server.py script in that machine, to act as a server./>
  <br Socket connetion will be established between the gateway and the server./>
  <br If any query comes from the Ad-Blocker from gateway it will send the reply by checking the database./>
