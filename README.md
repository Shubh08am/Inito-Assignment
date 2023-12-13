# Inito-Assignment

Dockerfile that you can use to containerize the C++ in-memory file system implementation. This Dockerfile assumes you have Docker installed on your machine.
# Use an official Ubuntu runtime as a base image
FROM ubuntu:latest

This line specifies the base image for the Docker container. In this case, it uses the latest version of the official Ubuntu image as the starting point.

# Set the working directory to /app
WORKDIR /app

This line sets the working directory inside the container to /app. All subsequent commands will be executed in this directory.

# Copy the current directory contents into the container at /app
COPY . /app

This line copies all the files and directories from the current directory on your host machine to the /app directory inside the container.

# Install necessary dependencies
RUN apt-get update && \
    apt-get install -y build-essential

These lines update the package list and install the build-essential package, which includes essential tools such as gcc and g++ needed for compiling C++ code.

# Compile the C++ code
RUN g++ -o filesystem main.cpp

This line compiles the C++ code (main.cpp) and generates an executable named filesystem. The resulting executable will be located in the /app directory inside the container.

# Set up entry point
CMD ["./filesystem"]

This line sets the default command to run when the container starts. In this case, it specifies that the filesystem executable should be executed.

# How to Use:
1. Save the Dockerfile in the same directory as your C++ source code (main.cpp).

2. Open a terminal in the same directory as the Dockerfile.

3. Run the following command to build the Docker image:
docker build -t in-memory-file-system .

4. After the build is complete, run the Docker container:
 docker run -it in-memory-file-system

This Dockerfile simplifies the setup process by encapsulating the C++ compilation and execution within a Docker container. 
It ensures that the containerized environment is isolated and contains all the necessary dependencies for running the C++ program.
