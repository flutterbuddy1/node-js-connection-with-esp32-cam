## For Node Server

Step 1:
```shell
npm install
```

Step 2:
```shell
npm start
```

## ESP32

Steps:

Change with your host and port
```c
  socketIO.begin("HOST", PORT, "/socket.io/?EIO=4");
```

Change Wifi Details
```c
  WiFiMulti.addAP("SSID", "PASSWORD");
```