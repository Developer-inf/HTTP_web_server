DROP TABLE IF EXISTS person CASCADE;
DROP TABLE IF EXISTS record CASCADE;

CREATE TABLE person (
    person_id INT GENERATED ALWAYS AS IDENTITY,
    login varchar(20) NOT NULL UNIQUE,
    password varchar(10) NOT NULL,
    PRIMARY KEY(person_id)
);

CREATE TABLE record (
    record_id INT GENERATED ALWAYS AS IDENTITY,
    str VARCHAR(30) NOT NULL,
    num INTEGER NOT NULL,
    person_id INTEGER NOT NULL,
    PRIMARY KEY(record_id),
    CONSTRAINT fk_user FOREIGN KEY (person_id) REFERENCES person(person_id) ON DELETE CASCADE
);