FROM gcc:latest
WORKDIR /app
COPY ./ ./
RUN apt-get update && \
    apt-get install libpqxx-dev libpqxx-doc libpqxx-6.4 && \
    gcc -std=c++20 server.cpp -lpqxx -lpq -lstdc++
CMD [ "./a.out" ]