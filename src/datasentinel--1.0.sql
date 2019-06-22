-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION datasentinel" to load this file. \quit

CREATE FUNCTION datasentinel_queryid(int)
RETURNS bigint
AS 'MODULE_PATHNAME'
LANGUAGE C
RETURNS NULL ON NULL INPUT;
