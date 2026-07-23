
CREATE OR REPLACE PROCEDURE insert_file(ficheiro_nome TEXT, siz BIGINT, OUT file_ud INT)
LANGUAGE plpgsql
AS $$
DECLARE
    disk_idd INTEGER;
    disk_pathh TEXT;
    disk_cur INTEGER;
BEGIN
  SELECT disk_id, disk_path, disk_current INTO disk_idd, disk_pathh, disk_cur FROM disk WHERE disk_used + siz < disk_limit ORDER BY disk_priority DESC LIMIT 1;
  IF disk_idd IS NULL THEN
    RAISE EXCEPTION 'There are no disks available';
  END IF;

  INSERT INTO file(file_name, file_path, disk_id, file_size) VALUES (ficheiro_nome,  CONCAT_WS('/', disk_pathh, disk_cur), disk_idd, siz) RETURNING file_id INTO file_ud;
  CALL update_disk_size(NULL, NULL, disk_idd);
END;
$$;

CREATE OR REPLACE PROCEDURE transfer_sphere_to_sphere(sphere_from text, sphere_destiny text)
LANGUAGE plpgsql
AS $$
DECLARE
  macaco INTEGER DEFAULT NULL;
BEGIN
  SELECT sphere_id FROM sphere WHERE sphere_name = sphere_destiny INTO macaco;
  IF macaco IS NULL THEN
    INSERT INTO sphere(sphere_name) VALUES (sphere_destiny);
  END IF;

  INSERT INTO connection(file_id, sphere_id)
  SELECT file_id, macaco
  FROM connection, sphere
  WHERE connection.sphere_id = sphere.sphere_id AND sphere_name = sphere_from
  ON CONFLICT(file_id, sphere_id) DO NOTHING;
END;
$$;

CREATE OR REPLACE PROCEDURE update_disk_size(t_file_id INT, t_file_name TEXT, t_disk_id INT)
LANGUAGE plpgsql
AS $$
BEGIN
  IF t_file_id IS NOT NULL THEN
    SELECT disk_id INTO t_disk_id FROM file WHERE file_id = t_file_id LIMIT 1;
  ELSE
    IF t_file_name IS NOT NULL THEN
      SELECT disk_id INTO t_disk_id FROM file WHERE file_name = t_file_name LIMIT 1;
    END IF;
  END IF;

  UPDATE disk SET disk_used = (SELECT SUM(file_size) FROM file WHERE disk_id = t_disk_id AND active = TRUE),
  disk_current = (SELECT COUNT(*) FROM file WHERE disk_id = t_disk_id)
  WHERE disk_id = t_disk_id;
END;
$$;

CREATE OR REPLACE PROCEDURE connect_file_tag_by_name(file_n TEXT, tag_n TEXT)
LANGUAGE plpgsql
AS $$
BEGIN
  INSERT INTO file_tag(file_id, sphere_id)
  SELECT file.file_id, tag.tag_id
  FROM file, sphere
  WHERE tag_name = tag_n AND file_name = file_n
  ON CONFLICT(file_id, tag_id) DO NOTHING;
END;
$$;

CREATE OR REPLACE PROCEDURE add_permission(usern text, permission_n text)
LANGUAGE plpgsql
AS $$
BEGIN
  IF NOT EXISTS(SELECT * FROM permission WHERE permission_name = permission_n) THEN
    INSERT INTO permission(permission_name) VALUES(permission_n); 
  END IF;

  INSERT INTO user_permission(user_id, permission_id)
  SELECT user_id, permission_id
  FROM app_user, permission
  WHERE permission_name = permission_n AND username = usern
  ON CONFLICT(user_id, permission_id) DO NOTHING;
END;
$$;

CREATE OR REPLACE PROCEDURE delete_file(file_nm TEXT)
LANGUAGE plpgsql
AS $$
DECLARE
  macaco INTEGER DEFAULT NULL;
  disk_ff INTEGER DEFAULT NULL;
BEGIN
  SELECT file_id, disk_id INTO macaco, disk_ff FROM file WHERE file_name = file_nm;

  UPDATE file SET active = FALSE WHERE file_id = macaco;
  DELETE FROM connection WHERE file_id = macaco;

  CALL update_disk_size(NULL, NULL, disk_ff);
END;
$$;
