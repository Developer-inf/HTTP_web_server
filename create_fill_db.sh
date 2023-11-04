#!/bin/bash

PGPASSWORD='123' psql -h 127.0.0.1 -U postgres -d postgres -p 5433 -f ./migrations/create_table.sql
PGPASSWORD='123' psql -h 127.0.0.1 -U postgres -d postgres -p 5433 -f ./migrations/some_values.sql