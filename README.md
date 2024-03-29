# C-Server-Go-Client

## Server in C

## Requirements 
- sqlite3
- libmircohttpd
- jansson
- gnutls
- libcurl-gnutls
- zlib
- ulfius

## Running the server


### Build & Run

### Check if running
Go to `http://localhost:8080/api` and see if there is a response in browser

### Windows
Run `docker-compose up -d`

### Linux
Theres a sh file for linux to run a build based on the cmake


## Endpoint Curl Usage:
### Get all patients
`curl -X GET http://localhost:8080/api/patients`

### Get a specific patient (replace {patientID} with an actual patient ID)
`curl -X GET http://localhost:8080/api/patients/{patientID}`

### Create a new patient
`curl -X POST -H "Content-Type: application/json" -d '{"name": "John Doe"}' http://localhost:8080/api/patients`

### Update a specific patient (replace {patientID} with an actual patient ID)
`curl -X PUT -H "Content-Type: application/json" -d '{"name": "Updated Name"}' http://localhost:8080/api/patients/{patientID}`

### Delete a specific patient (replace {patientID} with an actual patient ID)
`curl -X DELETE http://localhost:8080/api/patients/{patientID}`


# Client in Go
Install golang 

Go into the client folder

`cd client`

Run client 
`go run main.go`

Go to http://localhost:8081/ in the browser



