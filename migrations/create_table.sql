CREATE TABLE test (
    id SERIAL,
    str VARCHAR(30) NOT NULL,
    num INTEGER NOT NULL
);

CREATE TABLE users (
    id SERIAL,
    login varchar(20) NOT NULL,
    password varchar(10) NOT NULL
);