# Inito-Assignment

Dockerfile that you can use to containerize the C++ in-memory file system implementation. This Dockerfile assumes you have Docker installed on your machine.
# Use an official Ubuntu runtime as a base image
FROM ubuntu:latest

# Set the working directory to /app
WORKDIR /app

# Copy the current directory contents into the container at /app
COPY . /app

# Install necessary dependencies
RUN apt-get update && \
    apt-get install -y build-essential

# Compile the C++ code
RUN g++ -o filesystem main.cpp

# Set up entry point
CMD ["./filesystem"]

To build the Docker image, save the Dockerfile in the same directory as your C++ source code, and run the following command in the terminal:
docker build -t in-memory-file-system .
This will build a Docker image named "in-memory-file-system."

To run the Docker container:
docker run -it in-memory-file-system
This assumes that your C++ source code is in a file named main.cpp. If your source code file has a different name, make sure to update the Dockerfile accordingly.

Note: This Dockerfile assumes a simple scenario and might need adjustments based on the specific setup and dependencies of your C++ code. If you have additional dependencies or specific requirements, you may need to customize the Dockerfile further.
