Steps:

1. Create MongoDB account
2. Create a cluster and db user pw
3. Copy username password and connection string
4. Create a nodejs server to recieve requests and send db operations (replace your connection string)
5. Write ESP32 code, replace with your ssid, pw and local ip address using ipconfig
6. Upload the code
7. Make a new inbound rule in windows defender firewall to accept from port you want to use for POST request
8. Make the Wifi network private in settings
9. Upload your code and see in serial monito if it works!
10. Open MongoDB Database Explorer to find your newly created database with some entries that ESP32 sent!

commands: 

npm init -y
npm install mongodb
