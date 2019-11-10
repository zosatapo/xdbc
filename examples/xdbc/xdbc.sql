----------------------------------
--  ORACLE XDBC TEST
----------------------------------
create table xdbc_demo(dat_col date,ts_col timestamp,inv_ds_col interval day to second, inv_ym_col interval year to month,int_col number,int8_col number,flt_col float,dbl_col number(20,5),nc_col char(100),vc_col varchar(100),cbl_col clob,blb_clob blob,txt_col clob);

create table xdbc_decimal(col_decimal number,col_decimal_9 number(9),col_decimal_9_2  number(9,2),col_float float);

create or replace procedure proc_demo(vc_incol  IN varchar2,vc_ret1 OUT varchar2) is
begin
  vc_ret1:=' - vc_ret1';
end proc_demo;

create or replace procedure proc_demo_v2(vc_incol  IN varchar2,vc_ret1 OUT varchar2,vc_ret2 OUT varchar2) is
begin
  vc_ret1:=' - vc_ret1';
  vc_ret2:=' - vc_ret2';
end proc_demo_v2;

create or replace procedure proc_demo_v3(vc_incol  IN varchar2,vc_ret1 OUT varchar2,vc_ret2 OUT varchar2,vc_ret3 OUT varchar2) is
begin
  vc_ret1:=' - vc_ret1';
  vc_ret2:=' - vc_ret2';
  vc_ret3:=' - vc_ret3';
end proc_demo_v3;


CREATE OR REPLACE FUNCTION func_demo(aa in int, bb out int) RETURN NUMBER IS
tmpVar NUMBER;
BEGIN
   tmpVar := aa;
   bb :=aa;
   RETURN tmpVar;
END func_demo;

----------------------------------
--  INFORMIX XDBC TEST
----------------------------------
onspaces -c -S sbspace001 -p /data/ifxdata/sbspace001  -o 100 -s 200000 -Df "LOGGING=ON"

create table xdbc_demo(dat_col date,ts_col DATETIME YEAR TO SECOND ,inv_ds_col interval day to second,inv_ym_col interval year(4) to month,int_col integer,int8_col int8,flt_col float,dbl_col decimal,nc_col char(100),vc_col varchar(100) ,cbl_col clob,blb_col blob,txt_col text) put cbl_col in (sbspace001);

insert into xdbc_demo(dat_col,ts_col ,int_col,int8_col,flt_col,dbl_col,nc_col,vc_col,cbl_col,blb_col,txt_col) values(today,current,10,11,20.20,30.30,'nc_col','vc_col',NULL,NULL,'txt_col');

create table xdbc_decimal(col_decimal decimal,col_decimal_9 decimal(9),col_decimal_9_2  decimal(9,2),col_float float);

create procedure proc_demo(mac char(3)) returning decimal;
return 10;
end procedure; 

----------------------------------
--  TIMESTEN XDBC TEST
----------------------------------
CREATE TABLE XDBC_DEMO(DAT_COL DATE,TS_COL TIMESTAMP,INT_COL INTEGER,INT8_COL TT_BIGINT,FLT_COL FLOAT,DBL_COL DOUBLE,NC_COL CHAR(100),VC_COL VARCHAR(100));

----------------------------------
--  MYSQL XDBC TEST
----------------------------------
CREATE TABLE XDBC_DEMO(DAT_COL DATE,TS_COL TIMESTAMP,INT_COL INTEGER,INT8_COL BIGINT,FLT_COL FLOAT,DBL_COL DOUBLE(20,5),NC_COL CHAR(100),VC_COL VARCHAR(100),CBL_COL BLOB,BLB_CLOB BLOB,TXT_COL TEXT);

CREATE TABLE XDBC_DECIMAL(COL_DECIMAL DECIMAL,COL_DECIMAL_9 DECIMAL(9),COL_DECIMAL_9_2  DECIMAL(9,2),COL_FLOAT FLOAT);

CREATE TABLE SYS.DUAL(DAT_COL DATE);

DELIMITER //  

CREATE PROCEDURE proc_demo(IN vc_incol varchar(10),OUT vc_ret1 varchar(11))
BEGIN
 set vc_ret1 = 'vc_ret1'; 
END
//
DELIMITER ;

----------------------------------
--  ALTIBASE XDBC TEST
----------------------------------
CREATE TABLE XDBC_DEMO(DAT_COL DATE,TS_COL TIMESTAMP,INT_COL INTEGER,INT8_COL BIGINT,FLT_COL FLOAT,DBL_COL DOUBLE,NC_COL CHAR(100),VC_COL VARCHAR(1
00));

CREATE PROCEDURE proc_demo(vc_incol IN varchar(10),vc_ret1 OUT varchar(11))
AS
BEGIN
 set vc_ret1 = 'vc_ret1';
END;
