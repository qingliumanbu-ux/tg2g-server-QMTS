/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   syt
Version:    1.0
Date:     2022-10-14 17:13:56
Description: 供智慧质量获取工序成分
**************************************************/

#include "stdafx.h"

//程序用头文件

#include "epex.h"

/* ***** 外部函数申明 ***** */


BM2_FUNCTION_EXPORT
int f_qmts25_get_qx(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn)
{
	//APP_BEGIN()
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	/*程序用变量*/
	CString sqlstr("");
	int doFlag = 0;
	int  fetchRowCount = 0; 

	CString lpsz_srv_id = " ";
	CString	lpsz_sub_ename = " ";


 

	/* 实体类定义 */
	CModel tqmts24("TQMTS24");
	CModel tqmts25("TQMTS25");

	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq24(conn);
	CDbCommand cmd_inq25(conn);

	bcls_ret->Tables[0].Columns.Add(DT_STRING, "HEAT_NO");
	bcls_ret->Tables[0].Columns.Add(DT_STRING, "PONO");
	bcls_ret->Tables[0].Columns.Add(DT_STRING, "ST_NO");
	bcls_ret->Tables[0].Columns.Add(DT_STRING, "ITEM_CODE");
	bcls_ret->Tables[0].Columns.Add(DT_STRING, "ITEM_NAME");
	bcls_ret->Tables[0].Columns.Add(DT_STRING, "ITEM_VALUE_N");
	bcls_ret->Tables[0].Columns.Add(DT_STRING, "OK_FLAG");
	bcls_ret->Tables[0].Columns.Add(DT_STRING, "ST_SAMPLE_NO");
	bcls_ret->Tables[0].Columns.Add(DT_STRING, "SAMP_FLAG");
	bcls_ret->Tables[0].Columns.Add(DT_STRING, "ELM_TYPE");

	try
	{

		/* ***** 获取输入参数 ***** */
		tqmts24["PONO"] = bcls_rec->Tables[0].Rows[0]["PONO"].ToString().Trim();
		tqmts24["HEAT_NO"] = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().Trim();
		/*if (bcls_rec->Tables[0].Columns.Contains("DU_FLAG"))
		{
			tqmts29["ARCHIVE_FLAG"] = bcls_rec->Tables[0].Rows[0]["DU_FLAG"].ToString().Trim();
		}
		else
		{
			tqmts29["ARCHIVE_FLAG"] = "U";
		}*/

		/* ***** 打印输入参数 ***** */
		Log::Trace("", __FUNCTION__, "TQMTS24.PONO[{0}]", tqmts24["PONO"].ToString());
		Log::Trace("", __FUNCTION__, "TQMTS24.HEAT_NO[{0}]", tqmts24["HEAT_NO"].ToString());

		  
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:	        // MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default: // 所有数据库适用，通用SQL语句
			sqlstr = " SELECT ST_SAMPLE_NO,REP_ELM_SEL_FLAG,ST_SAMPLE_SEQ,ST_NO "
				" FROM TQMTS24 "
				" WHERE pono = @pono "
				"   AND WHOLE_BACKLOG_CODE = 'C'"
				" order by ST_SAMPLE_NO";
			break;
		}
		cmd_inq24.SetCommandText(sqlstr);
		//压入绑定参数，""包括的第一个参数的名称与SQL语句中@后对应的变量完全一致，大小写敏感
		cmd_inq24.Parameters.Set("pono", tqmts24["PONO"].ToString());
		cmd_inq24.ExecuteReader();
		fetchRowCount = 0;
		while (cmd_inq24.Read())
		{
			tqmts24["ST_SAMPLE_NO"] = cmd_inq24.GetString(1);
			tqmts24["REP_ELM_SEL_FLAG"] = cmd_inq24.GetString(2);
			tqmts24["ST_SAMPLE_SEQ"] = cmd_inq24.GetDecimal(3);
			tqmts24["ST_NO"] = cmd_inq24.GetString(4);

			Log::Trace("", __FUNCTION__, "ST_SAMPLE_NO[{0}]REP_ELM_SEL_FLAG[{1}]ST_SAMPLE_SEQ[{2}]ST_NO[{3}]"
				, tqmts24["ST_SAMPLE_NO"].ToString(), tqmts24["REP_ELM_SEL_FLAG"].ToString(), tqmts24["ST_SAMPLE_SEQ"].ToDecimal(), tqmts24["ST_NO"].ToString());
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:	        // MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default: // 所有数据库适用，通用SQL语句
				sqlstr = " SELECT elm_code,elm_name,elm_act,elm_ok "
					" FROM TQMTS25 "
					" WHERE pono = @PONO "
					"   AND ST_SAMPLE_NO = @ST_SAMPLE_NO "
					" ORDER BY ELM_CODE";
				break;
			}
			cmd_inq25.SetCommandText(sqlstr);
			//压入绑定参数，""包括的第一个参数的名称与SQL语句中@后对应的变量完全一致，大小写敏感
			cmd_inq25.Parameters.Set("PONO", tqmts24["PONO"].ToString());
			cmd_inq25.Parameters.Set("ST_SAMPLE_NO", tqmts24["ST_SAMPLE_NO"].ToString());
			cmd_inq25.ExecuteReader();
			while (cmd_inq25.Read())
			{
				tqmts25["ELM_CODE"] = cmd_inq25.GetString(1);
				tqmts25["ELM_NAME"] = cmd_inq25.GetString(2); 
				tqmts25["ELM_ACT"] = cmd_inq25.GetDecimal(3);
				tqmts25["ELM_OK"] = cmd_inq25.GetDecimal(4); 

				Log::Trace("", __FUNCTION__, "ST_SAMPLE_NO[{0}]ELM_CODE[{1}]fetchRowCount[{2}]"
					, tqmts24["ST_SAMPLE_NO"].ToString(), tqmts25["ELM_CODE"].ToString(), fetchRowCount);

				bcls_ret->Tables[0].Rows.Add();
				bcls_ret->Tables[0].Rows[fetchRowCount]["HEAT_NO"] = tqmts24["HEAT_NO"];
				bcls_ret->Tables[0].Rows[fetchRowCount]["PONO"] = tqmts24["PONO"];
				bcls_ret->Tables[0].Rows[fetchRowCount]["ST_NO"] = tqmts24["ST_NO"];
				bcls_ret->Tables[0].Rows[fetchRowCount]["ITEM_CODE"] = tqmts25["ELM_CODE"];
				bcls_ret->Tables[0].Rows[fetchRowCount]["ITEM_NAME"] = tqmts25["ELM_NAME"];
				bcls_ret->Tables[0].Rows[fetchRowCount]["ITEM_VALUE_N"] = tqmts25["ELM_ACT"];
				bcls_ret->Tables[0].Rows[fetchRowCount]["OK_FLAG"] = tqmts25["ELM_OK"];
				bcls_ret->Tables[0].Rows[fetchRowCount]["ST_SAMPLE_NO"] = tqmts24["ST_SAMPLE_NO"];
				bcls_ret->Tables[0].Rows[fetchRowCount]["SAMP_FLAG"] = tqmts24["REP_ELM_SEL_FLAG"];
				bcls_ret->Tables[0].Rows[fetchRowCount]["ELM_TYPE"] = tqmts24["ST_SAMPLE_SEQ"];

				fetchRowCount++;
			}
			cmd_inq25.Close();
		}
		//在函数退出前，统一Close()操作
		cmd_inq24.Close();
	 

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


