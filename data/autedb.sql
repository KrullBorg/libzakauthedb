CREATE TABLE utenti
(
  codice varchar(20) NOT NULL DEFAULT '',
  "password" varchar(32) DEFAULT '',
  status char(1) DEFAULT '',
  CONSTRAINT utenti_pkey PRIMARY KEY (codice)
);

/* utente iniziale con password 'root' */
INSERT INTO utenti VALUES ('root', '63a9f0ea7bb98050796b649e85481845', '');
