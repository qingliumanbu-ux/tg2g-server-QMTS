/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:      gx
Version:     1.0
Date:        2015-06-29
Description: 炉次质量信息查询 TQMTS23
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中

// service入口
BM2F_ENTERACE(qmts21_inq_23)


int f_qmts21_inq_23(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//程序用变量
	int doFlag = 0;
	int i = 0;
	int fetchRowCount = 0;

	CString s_heat_no = "";
	CString s_judge_code = "";	
	CString s_pono = "";
	CString s_prod_time_from = "";
	CString s_prod_time_to = "";

	CString sqlstr = "";

	/* 实体类定义 */
	CModel tqmts23("TQMTS23");
	
	CDbCommand cmd_inq(conn);

	try
	{
		/*获得传入参数*/
		s_heat_no = bcls_rec->Tables[0].Rows[0]["heat_no"].ToString().Trim();
		s_judge_code = bcls_rec->Tables[0].Rows[0]["judge_code"].ToString().Trim();

		if (bcls_rec->Tables[0].Columns.Contains("pono"))
			s_pono = bcls_rec->Tables[0].Rows[0]["pono"].ToString().Trim();		
		if (bcls_rec->Tables[0].Columns.Contains("rec_create_time_from"))
			s_prod_time_from = bcls_rec->Tables[0].Rows[0]["rec_create_time_from"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("rec_create_time_to"))
			s_prod_time_to = bcls_rec->Tables[0].Rows[0]["rec_create_time_to"].ToString().Trim();

		Log::Trace("", "", "qmts21_inq_23 IN:---s_heat_no = [{0}]", s_heat_no);
		Log::Trace("", "", "qmts21_inq_23 IN:---s_judge_code = [{0}]", s_judge_code);
		Log::Trace("", "", "qmts21_inq_23 IN:---s_pono = [{0}]", s_pono);
		Log::Trace("", "", "qmts21_inq_23 IN:---s_prod_time_from = [{0}]", s_prod_time_from);
		Log::Trace("", "", "qmts21_inq_23 IN:---s_prod_time_to = [{0}]", s_prod_time_to);
		 
		switch(conn->DatabaseKind)
		{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				sqlstr = " SELECT * "
							"   FROM TQMTS23 "
							"  WHERE HEAT_NO like '%'||@heat_no||'%' " ;
				if (s_pono.Trim() != "")  sqlstr = sqlstr + " AND PONO  like '%' || @s_pono || '%' ";
				if (s_judge_code.Trim() != "")  sqlstr = sqlstr + " AND JUDGE_CODE = @judge_code ";
				if (s_prod_time_from.Trim() != "")
					sqlstr = sqlstr + " AND REC_CREATE_TIME >= @s_prod_time_from ";
				if (s_prod_time_to.Trim() != "")
					sqlstr = sqlstr + " AND REC_CREATE_TIME <= @s_prod_time_to ";
				sqlstr = sqlstr + "  ORDER BY HEAT_NO ASC ";
				break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("heat_no", s_heat_no.Trim());
		cmd_inq.Parameters.Set("judge_code", s_judge_code.Trim());
		cmd_inq.Parameters.Set("s_pono", s_pono.Trim());
		cmd_inq.Parameters.Set("s_prod_time_from", s_prod_time_from.Trim());
		cmd_inq.Parameters.Set("s_prod_time_to", s_prod_time_to.Trim());
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		cmd_inq.Close();
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

