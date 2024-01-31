# Use Ubuntu as the base image
FROM ubuntu:latest

# Update the package list
RUN apt-get update

# Install necessary packages including build tools and libraries
RUN apt-get install -y build-essential git libgnutls28-dev libjansson-dev libmicrohttpd-dev libcurl4-gnutls-dev libglib2.0-dev sqlite3 libsqlite3-dev libsystemd-dev


# Clone and build Orcania
RUN git clone https://github.com/babelouest/orcania.git
WORKDIR /orcania
RUN make && make install

# Clone and build Yder
WORKDIR /
RUN git clone https://github.com/babelouest/yder.git
WORKDIR /yder
RUN make && make install

# Clone and build Ulfius
WORKDIR /
RUN git clone https://github.com/babelouest/ulfius.git
WORKDIR /ulfius
RUN make && make install

# Set the working directory back to /app
WORKDIR /app

# Copy your C code into the container
COPY main.c .

# Compile your C code
RUN gcc -o main main.c -ljansson -lmicrohttpd -lcurl -lglib-2.0 -lsqlite3 -lulfius -lorcania -lyder

# Expose the port your application will listen on
EXPOSE 8080

# Set the entry point for your container
CMD ["./main"]

