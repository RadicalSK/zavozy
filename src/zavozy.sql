CREATE TABLE uzivatel (
	uzivJmeno TEXT PRIMARY KEY,
	heslo TEXT NOT NULL,
	celeJmeno TEXT NOT NULL,
	admin INTEGER DEFAULT 0,
	aktivni INTEGER DEFAULT 1
);

CREATE TABLE pacient (
	pacId INTEGER PRIMARY KEY,
	prijmeni TEXT NOT NULL,
	jmeno TEXT NOT NULL,
	ulice TEXT NOT NULL,
	obec TEXT NOT NULL,
	telefon1 TEXT,
	poznTel1 TEXT,
	telefon2 TEXT,
	poznTel2 TEXT,
	poznamka TEXT
);

CREATE TABLE pomucka (
	kodVzp INTEGER PRIMARY KEY,
	nazev TEXT NOT NULL,
	doplnek TEXT
);

CREATE TABLE zavoz (
	datumVychystani TEXT NOT NULL,
	poridil TEXT NOT NULL,
	adresa TEXT NOT NULL,
	pacId INTEGER REFERENCES pacient(pacId),
	datumZavozu TEXT,
	PRIMARY KEY (pacId, datumZavozu)
);

CREATE TABLE polozkaZavozu (
	pocet INTEGER NOT NULL,
	kodVzp INTEGER,
	pacId INTEGER,
	datumZavozu TEXT,
	FOREIGN KEY (kodVzp) REFERENCES pomucka(kodVzp),
	FOREIGN KEY (pacId, datumZavozu) REFERENCES zavoz(pacId, datumZavozu)
);

CREATE TABLE verzeCiselniku (
	verze TEXT PRIMARY KEY,
	datum TEXT NOT NULL,
	aktualizoval TEXT NOT NULL REFERENCES uzivatel(uzivJmeno)
);
