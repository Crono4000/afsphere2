
CREATE OR REPLACE PROCEDURE insert_info_bytea(titlee TEXT, info BYTEA, info_sizee BIGINT, is_filee BOOLEAN, OUT iid INT)
LANGUAGE plpgsql
AS $$
BEGIN
  INSERT INTO info(title, info_bytea, is_bytea, is_file, info_size) VALUES (titlee, info, TRUE, is_filee, info_sizee) 
  RETURNING info_id INTO iid;
END;
$$;

CREATE OR REPLACE PROCEDURE insert_info_oid(titlee TEXT, info_sizee BIGINT, is_filee BOOLEAN, OUT iid INT, OUT ooid OID)
LANGUAGE plpgsql
AS $$
BEGIN
  INSERT INTO info(title, info_oid, is_bytea, is_file, info_size) VALUES (titlee, lo_create(0), FALSE, is_filee, info_sizee) 
  RETURNING info_id, info_oid INTO iid, ooid;
END;
$$;

CREATE OR REPLACE PROCEDURE insert_info_tag(tagg TEXT, infoo INT)
LANGUAGE plpgsql
AS $$
DECLARE
	tag TEXT;
BEGIN
    IF NOT EXISTS(SELECT * FROM tag WHERE tag_name = tagg) THEN
		INSERT INTO tag(tag_name) VALUES (tagg);
	END IF;
	INSERT INTO info_tag(info_id, tag_name) VALUES (infoo, tagg);
END;
$$;

CREATE OR REPLACE PROCEDURE insert_info_tags(tags TEXT[], info INT)
LANGUAGE plpgsql
AS $$
DECLARE
	tag TEXT;
BEGIN
	FOREACH tag IN ARRAY tags
    LOOP
        IF NOT EXISTS(SELECT * FROM tag WHERE tag_name = tag) THEN
			INSERT INTO tag(tag_name) VALUES (tag);
		END IF;
		INSERT INTO info_tag(info_id, tag_name) VALUES (info, tag);
    END LOOP;
END;
$$;
