CREATE TABLE users (
	code varchar(20) NOT NULL DEFAULT '',
	password varchar(32) DEFAULT '',
	surname varchar(100) DEFAULT '',
	name varchar(100) DEFAULT '',
	enabled bool,
	password_expiration time,
	notes text DEFAULT '',
	status varchar(1) DEFAULT '',
	CONSTRAINT users_pkey PRIMARY KEY (code)
);

/* initial user with password 'root' */
INSERT INTO users (code, password, surname, enabled, status)
VALUES ('root', '63a9f0ea7bb98050796b649e85481845', 'Root', TRUE, '');
