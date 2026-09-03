/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   刘家岩
Version:    1.0
Date:     2012-02-16
Description: 工艺卡确认查询
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中



/*<remark>=========================================================
/// <summary>
/// 工艺卡确认查询
/// <para>
/// 获取输入参数：TQMTS01(工序制造标准表) ；
/// </para>
/// <para>数据库表：TQMTS01(工序制造标准表)); 
/// 前台QMTS01画面的F2(查询)调用    </para>
/// </summary>
/// <param name="TQMTS01">工序制造标准表    </param>
===========================================================</remark>*/  

// service入口
BM2F_ENTERACE(qmts01_inq)

int f_qmts01_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//程序用变量
	CString sqlstr("");
	int doFlag = 0;
	int i;
	int fetchRowCount = 0;
	CString QMTJBlock;
	int flag = 0;
	CString whole_backlog_code = "";
	CString base_code = "";
	CString q_st_no = " ";
	CString q_factory_div = " ";
	CString st_no = "";
	CString factory_div = "";

	CDecimal v_count;
	CString table_name;
	
	/* 实体类定义 */
	CModel tqmts01("TQMTS01");
	CModel tqmts0x("TQMTS0X");
	
	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_0x(conn);
	CDbCommand count_inq(conn);

	try
	{
		/*获得传入参数*/
		q_st_no = bcls_rec->Tables[0].Rows[0]["st_no"].ToString().Trim();
		q_factory_div = bcls_rec->Tables[0].Rows[0]["factory_div"].ToString().Trim();
		flag = (int)bcls_rec->Tables[1].Rows[0]["flag"];
		base_code = bcls_rec->Tables[1].Rows[0]["base_code"].ToString().Trim();

		Log::Trace("", "", "qmts01_inq IN: q_st_no[{0}]q_factory_div[{1}]flag[{2}]base_code[{3}]", (const char*)q_st_no, (const char*)q_factory_div, flag, (const char*)base_code);

		if (bcls_rec->Tables[1].Columns.Contains("st_no"))
		{
			st_no = bcls_rec->Tables[1].Rows[0]["st_no"].ToString().TrimOrBlank();
			Log::Trace("", "", "st_no[{0}]", (const char*)st_no);
		}
		if (bcls_rec->Tables[1].Columns.Contains("factory_div"))
		{
			factory_div = bcls_rec->Tables[1].Rows[0]["factory_div"].ToString().TrimOrBlank();
			Log::Trace("", "", "factory_div[{0}]", (const char*)factory_div);
		}
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "st_no");
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "factory_div");
		//	bcls_ret->Tables[0].Columns.Add(DT_STRING,"valid_flag");

		bcls_ret->Tables.Add();
		bcls_ret->Tables[1].Columns.Add(DT_STRING, "st_no");
		bcls_ret->Tables[1].Columns.Add(DT_STRING, "factory_div");

		bcls_ret->Tables.Add();

		if(flag == 0)
		{
			
			//定义数据库操作命令对象comm_inq执行sql语句，sql字符串用""包括，可以分行，但每行前后务必留出一个空格。
			switch(conn->DatabaseKind)
			{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:						// 所有数据库适用，通用SQL语句
					sqlstr = "SELECT  ST_NO,FACTORY_DIV "
							 "  FROM TQMTS01 "
							 " WHERE   "
							 "     ST_NO LIKE @st_no || '%' "
						     "     AND FACTORY_DIV LIKE @factory_div || '%'  "
						     "     AND base_code = @base_code "
						     "     AND WHOLE_BACKLOG_CODE ='' "
							 " ORDER BY ST_NO ASC ";
					break;
			}
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("st_no",q_st_no);
			cmd_inq.Parameters.Set("factory_div",q_factory_div);
			cmd_inq.Parameters.Set("base_code", base_code);
			cmd_inq.ExecuteReader();
			fetchRowCount = 0;
			while(cmd_inq.Read())
			{
				tqmts01["ST_NO"] = cmd_inq.GetString(1);
				tqmts01["FACTORY_DIV"] = cmd_inq.GetString(2);
			//	tqmts01["VALID_FLAG"] = cmd_inq.GetString(3);
				Log::Trace("", "","tqmts01.ST_NO[{0}],tqmts01.FACTORY_DIV[{1}]", (const char*)tqmts01["ST_NO"].ToString(),(const char*)tqmts01["FACTORY_DIV"].ToString());
				fetchRowCount++;
				bcls_ret->Tables[0].Rows.Add();
				bcls_ret->Tables[0].Rows[fetchRowCount - 1]["st_no"] = tqmts01["ST_NO"];
				bcls_ret->Tables[0].Rows[fetchRowCount - 1]["factory_div"] = tqmts01["FACTORY_DIV"];
			//	bcls_ret->Tables[0].Rows[fetchRowCount - 1]["base_code"] = tqmts01["BASE_CODE"];
			}
			cmd_inq.Close();
			Log::Trace("", "", "开始查询新内部钢种");
			switch(conn->DatabaseKind)
			{
					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:				// MS SQL Server数据库
					case DB_KIND_ORACLE:	        // Oracle 数据库
					default:						// 所有数据库适用，通用SQL语句
						sqlstr = "SELECT ST_NO,FACTORY_DIV "
								 "  FROM TQMTS0X "
								 " WHERE "
							     " (ST_NO,FACTORY_DIV,BASE_CODE) NOT IN(SELECT DISTINCT ST_NO,FACTORY_DIV,BASE_CODE FROM TQMTS01 WHERE WHOLE_BACKLOG_CODE ='') "
							     "     AND ST_NO LIKE @st_no || '%' "
							     "     AND FACTORY_DIV LIKE @factory_div || '%'  "
							     "     AND VALID_FLAG = '1' "
							     "     AND base_code = @base_code ";
						break;
			}
			cmd_inq_0x.SetCommandText(sqlstr);
			cmd_inq_0x.Parameters.Set("st_no", q_st_no);
			cmd_inq_0x.Parameters.Set("factory_div", q_factory_div);
			cmd_inq_0x.Parameters.Set("base_code", base_code);
			cmd_inq_0x.ExecuteReader();
			fetchRowCount = 0;
			while (cmd_inq_0x.Read())
			{
				tqmts0x["ST_NO"] = cmd_inq_0x.GetString(1);
				tqmts0x["FACTORY_DIV"] = cmd_inq_0x.GetString(2);

				Log::Trace("", "","tqmts0x.ST_NO[{0}], tqmts0x.FACTORY_DIV[{1}]",(const char*)tqmts0x["ST_NO"].ToString(),(const char*)tqmts0x["FACTORY_DIV"].ToString());
				fetchRowCount++;
				
				bcls_ret->Tables[1].Rows.Add();
				bcls_ret->Tables[1].Rows[fetchRowCount-1]["factory_div"] = tqmts0x["FACTORY_DIV"];
				bcls_ret->Tables[1].Rows[fetchRowCount-1]["st_no"] = tqmts0x["ST_NO"];
			}
			cmd_inq_0x.Close();
		}
		//工序制造系统管理表查询
		else //if (flag == 1)
		{
			//if(st_no.Trim() == "")
			//{
			//	strcpy(s.msg,_RES("QM00S0004171")/*出钢记号不可为空。*/);
			//	throw CApplicationException(-1, s.msg, log.Location);
			//}
			switch(conn->DatabaseKind)
			{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:						// 所有数据库适用，通用SQL语句
					sqlstr = "SELECT * "
							 "  FROM TQMTS01 "
							 " WHERE   "
							 " ST_NO = @st_no "
							 " AND FACTORY_DIV =@factory_div"
						     " AND base_code = @base_code "
						     " AND WHOLE_BACKLOG_CODE != ''"
							 " ORDER BY WHOLE_BACKLOG_SEQ ASC ";
					break;
			}
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("st_no",st_no);
			cmd_inq.Parameters.Set("factory_div",factory_div);
			cmd_inq.Parameters.Set("base_code", base_code);
			cmd_inq.ExecuteQuery(bcls_ret->Tables[2]);
			cmd_inq.Close();
		}
	}
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,  _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);
		s.flag = -1;
		doFlag = -1;      //数据库异常时返回-1，事务将被回滚
	}
	catch(CApplicationException& ex)  //捕获应用错误
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch(CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg)-1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}

	return doFlag;
}
