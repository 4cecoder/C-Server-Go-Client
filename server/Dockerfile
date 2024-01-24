# Use a minimal Linux distribution as the base image
FROM alpine:latest

# Install SQLite3 development libraries
RUN apk update && apk add sqlite-dev

# Install ulfius dependencies
RUN apk add gcc make automake autoconf libtool jansson-dev libmicrohttpd-dev libcurl curl-dev glib-dev

# Set the working directory
WORKDIR /app

# Copy your C code into the container
COPY main.c .

# Compile your C code
RUN gcc -o main main.c -ljansson -lmicrohttpd -lcurl -lglib-2.0 -lsqlite3

# Expose the port your application will listen on
EXPOSE 8080

# Set the entry point for your container
CMD ["./main"]
