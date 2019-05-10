CREATE TABLE IF NOT EXISTS points (
    id integer PRIMARY KEY AUTOINCREMENT, 
    type_id integer,
    frame_id integer,
    frame_index integer,
    x float,
    y float,
    recv_time integer,
    UNIQUE(frame_id, frame_index)
);

-- insert into points(frame_id, frame_index, x, y, recv_time) values(
--     0,0,1,1,0
-- );