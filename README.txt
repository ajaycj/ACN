##########  WEB PROXY IMPLMENTATION #########

SERVER
  

1.To compile proxy server code and to create the executable: gcc workon_server.c -o server -lpthread


2.To run executable:
  "./server"




CLIENT
 

1.To compile client code:
 gcc client.c -o client 
2.To run client code:
  ./client <local host ip> 
  
e.g., 
    ./client 129.110.92.16
   
         www.google.com:80:index.html

