/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      wsl
Version:     1.0
Date:        2023-11-30 14:48:51
Description: H5静态标准查询
**************************************************/

#include "stdafx.h"
BM2F_ENTERACE(qmts27_inqv)

int f_qmts27_inqv(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CDbCommand cmd_inq(conn);
	CString MAT_NO = "";
	CString ST_NO = "";
	CString HEAT_NO = "";
	CString PONO = "";
	CString  SLAB_NO = "";
	CString  ISE_TEST_FLAG = "";
	try
	{
		/*获得传入参数*/
		CString tableName = bcls_rec->Tables[1].Rows[0]["tableName"].ToString().Trim();
		Log::Trace("", "", "qmts27_inqv IN:---tableName = [{0}]", tableName);
		if (!bcls_rec->Tables[0].Columns.Contains("MAT_NO"))
		{
			bcls_rec->Tables[0].Columns.Add(DT_STRING, "MAT_NO");
		}
		if (!bcls_rec->Tables[0].Columns.Contains("ST_NO"))
		{
			bcls_rec->Tables[0].Columns.Add(DT_STRING, "ST_NO");
		}
		if (!bcls_rec->Tables[0].Columns.Contains("HEAT_NO"))
		{
			bcls_rec->Tables[0].Columns.Add(DT_STRING, "HEAT_NO");
		}
		if (!bcls_rec->Tables[0].Columns.Contains("PONO"))
		{
			bcls_rec->Tables[0].Columns.Add(DT_STRING, "PONO");
		}

		if (!bcls_rec->Tables[0].Columns.Contains("SLAB_NO"))
		{
			bcls_rec->Tables[0].Columns.Add(DT_STRING, "SLAB_NO");
		}
		if (!bcls_rec->Tables[0].Columns.Contains("ISE_TEST_FLAG"))
		{
			bcls_rec->Tables[0].Columns.Add(DT_STRING, "ISE_TEST_FLAG");
		}
		MAT_NO = bcls_rec->Tables[0].Rows[0]["MAT_NO"].ToString().Trim();
		Log::Trace("", "", "qmts27_inqv IN:---MAT_NO = [{0}]", MAT_NO);
		ST_NO = bcls_rec->Tables[0].Rows[0]["ST_NO"].ToString().Trim();
		Log::Trace("", "", "qmts27_inqv IN:---ST_NO = [{0}]", ST_NO);
		HEAT_NO = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().Trim();
		Log::Trace("", "", "qmts27_inqv IN:---HEAT_NO = [{0}]", HEAT_NO);
		PONO = bcls_rec->Tables[0].Rows[0]["PONO"].ToString().Trim();
		Log::Trace("", "", "qmts27_inqv IN:---PONO = [{0}]", PONO);
		SLAB_NO = bcls_rec->Tables[0].Rows[0]["SLAB_NO"].ToString().Trim();
		Log::Trace("", "", "qmts27_inqv IN:---SLAB_NO = [{0}]", SLAB_NO);
		ISE_TEST_FLAG = bcls_rec->Tables[0].Rows[0]["ISE_TEST_FLAG"].ToString().Trim();
		Log::Trace("", "", "qmts27_inqv IN:---ISE_TEST_FLAG = [{0}]", ISE_TEST_FLAG);


		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			sqlstr = " SELECT * "
				"   FROM  " + tableName +
				"  WHERE 1=1 ";
			if (MAT_NO.Trim() != "")
				sqlstr += " AND  MAT_NO= @MAT_NO";
			if (ST_NO.Trim() != "")
				sqlstr += " AND ST_NO like '%'||@ST_NO||'%' ";
			if (HEAT_NO.Trim() != "")
				sqlstr += " AND  HEAT_NO= @HEAT_NO";
			if (PONO.Trim() != "")
				sqlstr += " AND  PONO= @PONO";
			if (SLAB_NO.Trim() != "")
				sqlstr += " AND SLAB_NO = @SLAB_NO";
			if (ISE_TEST_FLAG.Trim() != "")
				sqlstr += " AND ISE_TEST_FLAG = @ISE_TEST_FLAG";
			break;
		}
		Log::Trace("", "", "qmts27_inqv IN:---sqlstr = [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("MAT_NO", MAT_NO.Trim());
		cmd_inq.Parameters.Set("ST_NO", ST_NO.Trim());
		cmd_inq.Parameters.Set("HEAT_NO", HEAT_NO.Trim());
		cmd_inq.Parameters.Set("PONO", PONO.Trim());
		cmd_inq.Parameters.Set("SLAB_NO", SLAB_NO.Trim());
		cmd_inq.Parameters.Set("ISE_TEST_FLAG", ISE_TEST_FLAG.Trim());
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		cmd_inq.Close();
	}
	catch (CDbException &ex)
	{
		CFormattable arguments[] = { ex.GetCode(), ex.GetMsg() };
		CMessageFormat::Format(s.msg, "Database Error,sqlcode=[{0}],sqlmsg=[{1}]", arguments, 2);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char *)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;
	}
	catch (CApplicationException &ex)
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException &ex)
	{
		strcpy(s.msg, ex.GetMsg());
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	return doFlag;
}
