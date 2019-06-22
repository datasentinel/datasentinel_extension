MODULE_big = datasentinel
OBJS = datasentinel.o $(WIN32RES)

EXTENSION = datasentinel
DATA = datasentinel--1.0.sql
PGFILEDESC = "datasentinel - queryid function"

LDFLAGS_SL += $(filter -lm, $(LIBS))

REGRESS_OPTS = --temp-config ./datasentinel.conf
REGRESS = datasentinel

# Disabled because these tests require "shared_preload_libraries=datasentinel",
# which typical installcheck users do not have (e.g. buildfarm clients).
NO_INSTALLCHECK = 1

PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
