#./SQLSource_demo -Dxdbc.isTracingOn=TRUE
#./DataSource_demo -Dxdbc.dbname=ORACLE -Dopencode.debug=TRUE -Dxdbc.isTracingOn=TRUE
./xdbc_pstmt_demo -Dxdbc.dbname=MYSQL -Dxdbc.isTracingOn=TRUE
#./xdbc_arr_demo -Dxdbc.dbname=ORACLE -Dxdbc.isTracingOn=TRUE
#./xdbc_stmt_demo -Dxdbc.dbname=ORACLE2 -Dopencode.debug=TRUE -Dxdbc.isTracingOn=TRUE
#./xdbc_proc_demo -Dxdbc.dbname=ORACLE -Dxdbc.isTracingOn=TRUE  -Dopencode.debug=TRUE
#./xdbc_perf_demo -Dxdbc.dbname=ORACLE  #-Dxdbc.isTracingOn=TRUE
#valgrind --tool=memcheck  --leak-check=yes ./.libs/xdbc_stmt_demo -Dxdbc.dbname=ORACLE -Dxdbc.isTracingOn=TRUE >> test.txt

