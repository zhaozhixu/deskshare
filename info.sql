create table user (
id integer primary key AUTO_INCREMENT,
name varchar(32) not null,
password varchar(32),
host varchar(64),
port_vrecv varchar(64),
port_vrecv_c varchar(64),
port_arecv varchar(64),
port_arecv_c varchar(64),
port_vsend varchar(64),
port_vsend_c varchar(64),
port_asend varchar(64),
port_asend_c varchar(64),
port_toserver varchar(64)
);

