.PHONY: build start stop restart exec clean help

# Default target
help:
	@echo "Available commands:"
	@echo "  make build    - Rebuild containers"
	@echo "  make start    - Start containers and attach to knowledgebase"
	@echo "  make stop     - Stop and remove containers"
	@echo "  make restart  - Restart containers and attach to knowledgebase"
	@echo "  make exec     - Attach to knowledgebase container"
	@echo "  make clean    - Stop containers and remove volumes (cleanup)"
	@echo "  make help     - Show this help message"

build:
	cd build && cmake --build .

server:
	cd build && ./server

client:
	cd build && ./client

# Rebuild containers (replaces build_container.sh)
build_container:
	docker-compose down
	docker-compose build

# Start containers and attach (replaces start.sh)
start:
	docker-compose up -d
	docker exec -it knowledgebase bash

# Stop containers (replaces stop.sh)
stop:
	docker-compose down

# Restart everything (replaces restart.sh)
restart:
	docker-compose down
	docker-compose build
	docker-compose up -d
	docker exec -it knowledgebase bash

# Attach to container (replaces exec.sh)
exec:
	docker exec -it knowledgebase bash

# Clean up - stop and remove volumes
clean:
	rm cookies.txt 
