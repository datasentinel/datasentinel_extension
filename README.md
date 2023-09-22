
![Datasentinel](https://www.datasentinel.io/images/head.jpg)

# This extension is deprecated since the version 14 of PostgreSQL
<hr>

### This extension gets the queryid (pg_stat_statements) from backends (pg_stat_activity) running queries
<br>

#### The extension is OPTIONAL.

The role of this extension is to get the queryid (pg_stat_statements) from active sessions (pg_stat_activity)
Warning

To be installed in the internal database named **postgres**

<br>


#### Installation

Example with Version 11
```
yum install -y https://download.postgresql.org/pub/repos/yum/11/redhat/rhel-7-x86_64/pgdg-redhat-repo-latest.noarch.rpm
yum install -y centos-release-scl
yum install -y postgresql11-devel postgresql11-llvmjit llvm-toolset-7
yum install -y gcc

```


1. Compile

```
export PATH=/usr/pgsql-{{PostgreSQL-version}}/bin:$PATH
cd datasentinel_extension/src
make
make install
```

2. Deploy

Example done with a standard postgresql 10 installed version
```
/usr/bin/mkdir -p /usr/pgsql-10/lib
/usr/bin/mkdir -p /usr/pgsql-10/share/extension
cp datasentinel.so /usr/pgsql-10/lib/
chmod 755 /usr/pgsql-10/lib/datasentinel.so
cp datasentinel.control /usr/pgsql-10/share/extension/
chmod 644 /usr/pgsql-10/share/extension/datasentinel.control
cp datasentinel--1.0.sql /usr/pgsql-10/share/extension/
chmod 644 /usr/pgsql-10/share/extension/datasentinel--1.0.sql
```

4. Modify postgresql.conf
```
shared_preload_libraries = 'pg_stat_statements,datasentinel'
track_activity_query_size = 65536
pg_stat_statements.track = all
```

5. Restart postgresql
   
You need to restart the cluster

7. Create the extensions
   
Connect as a superuser in the postgres database

```
CREATE EXTENSION IF NOT EXISTS pg_stat_statements;
CREATE EXTENSION datasentinel;
```

To check the extension is correctly installed, you can execute the following sql
```
select query, pid, datasentinel_queryid(pid) from pg_stat_activity
```
