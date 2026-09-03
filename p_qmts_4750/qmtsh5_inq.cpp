/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      wsl
Version:     1.0
Date:        2023-11-30 14:48:51
Description: H5静态标准查询
**************************************************/

#include "stdafx.h"
BM2F_ENTERACE(qmtsh5_inq)

int f_qmtsh5_inq(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq1(conn);
	CString MAT_NO = "";
	CString SLAB_CUT_TIME_TO = "";
	CString SLAB_CUT_TIME_FROM = "";
	CString ST_NO = "";
	CString HEAT_NO = "";
	CString PONO = "";
	CString  SLAB_NO = "";
	CString  ISE_TEST_FLAG = "";
	CString tableName = "";
	CString sqlstr1 = "";
	CDecimal cd_count = 0;
	int	record_count_per_page = 0; /* 每页记录数 */
	int	current_page_no = 0; /* 需查询的页号,从0开始计数 */
	int	start_row = 0; /* 将要压入outBlock的起始行 */
	try
	{
		/*获得传入参数*/
		if (bcls_rec->Tables.Contains("PAGEINFO"))
		{
			tableName = bcls_rec->Tables["PAGEINFO"].Rows[0]["tableName"].ToString().Trim();
			Log::Trace("", "", "qmtsh5_inq IN:---tableName = [{0}]", tableName);
			if (bcls_rec->Tables["PAGEINFO"].Columns.Contains("PAGE_NUM") && bcls_rec->Tables["PAGEINFO"].Columns.Contains("PAGE_SIZE"))
			{
				current_page_no = bcls_rec->Tables["PAGEINFO"].Rows[0]["PAGE_NUM"].ToDecimal().ToInt32() + 1;
				record_count_per_page = bcls_rec->Tables["PAGEINFO"].Rows[0]["PAGE_SIZE"];
			}
			else {
				record_count_per_page = bcls_rec->Tables[0].Rows[0]["RECORD_COUNT_PER_PAGE"];
				current_page_no = bcls_rec->Tables[0].Rows[0]["CURRENT_PAGE_NO"];
			}
		}
		else {
			record_count_per_page = bcls_rec->Tables[0].Rows[0]["RECORD_COUNT_PER_PAGE"];
			current_page_no = bcls_rec->Tables[0].Rows[0]["CURRENT_PAGE_NO"];
		}


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
		//if (tableName == "VMMSM01")
		//{
			if (!bcls_rec->Tables[0].Columns.Contains("SLAB_CUT_TIME_TO"))
			{
				bcls_rec->Tables[0].Columns.Add(DT_STRING, "SLAB_CUT_TIME_TO");
			}
			if (!bcls_rec->Tables[0].Columns.Contains("SLAB_CUT_TIME_FROM"))
			{
				bcls_rec->Tables[0].Columns.Add(DT_STRING, "SLAB_CUT_TIME_FROM");
			}
		//}
		MAT_NO = bcls_rec->Tables[0].Rows[0]["MAT_NO"].ToString().Trim();
		Log::Trace("", "", "qmtsh5_inq IN:---MAT_NO = [{0}]", MAT_NO);

		ST_NO = bcls_rec->Tables[0].Rows[0]["ST_NO"].ToString().Trim();
		Log::Trace("", "", "qmtsh5_inq IN:---ST_NO = [{0}]", ST_NO);

		HEAT_NO = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().Trim();
		Log::Trace("", "", "qmtsh5_inq IN:---HEAT_NO = [{0}]", HEAT_NO);

		PONO = bcls_rec->Tables[0].Rows[0]["PONO"].ToString().Trim();
		Log::Trace("", "", "qmtsh5_inq IN:---PONO = [{0}]", PONO);

		SLAB_NO = bcls_rec->Tables[0].Rows[0]["SLAB_NO"].ToString().Trim();
		Log::Trace("", "", "qmtsh5_inq IN:---SLAB_NO = [{0}]", SLAB_NO);

		ISE_TEST_FLAG = bcls_rec->Tables[0].Rows[0]["ISE_TEST_FLAG"].ToString().Trim();
		Log::Trace("", "", "qmtsh5_inq IN:---ISE_TEST_FLAG = [{0}]", ISE_TEST_FLAG);
		//if (tableName == "VMMSM01")
		//{
			SLAB_CUT_TIME_TO = bcls_rec->Tables[0].Rows[0]["SLAB_CUT_TIME_TO"].ToString().Trim();
			Log::Trace("", "", "qmtsh5_inq IN:---SLAB_CUT_TIME_TO = [{0}]", SLAB_CUT_TIME_TO);

			SLAB_CUT_TIME_FROM = bcls_rec->Tables[0].Rows[0]["SLAB_CUT_TIME_FROM"].ToString().Trim();
			Log::Trace("", "", "qmtsh5_inq IN:---SLAB_CUT_TIME_FROM = [{0}]", SLAB_CUT_TIME_FROM);
		//}

		/*拼接查询sql条数语句*/
		CString sql_count = "SELECT COUNT(*) FROM ( ";

		sqlstr = " SELECT * "
			"   FROM  " + tableName +
			"  WHERE 1=1 ";
		if (MAT_NO.Trim() != "")
			sqlstr += " AND  MAT_NO= '" + MAT_NO.Trim() + "'";
		if (ST_NO.Trim() != "")
			sqlstr += " AND ST_NO like '%'||'" + ST_NO.Trim() + "'||'%' ";
		if (HEAT_NO.Trim() != "")
			sqlstr += " AND  HEAT_NO= '" + HEAT_NO.Trim() + "'";
		if (PONO.Trim() != "")
			sqlstr += " AND  PONO= '" + PONO.Trim() + "'";
		if (SLAB_NO.Trim() != "")
			sqlstr += " AND SLAB_NO = '" + SLAB_NO.Trim() + "'";
		if (ISE_TEST_FLAG.Trim() != "")
			sqlstr += " AND ISE_TEST_FLAG = '" + ISE_TEST_FLAG.Trim() + "'";
		if (tableName == "VMMSM01")
		{
			if (SLAB_CUT_TIME_FROM.Trim() != "")
			{
				sqlstr += " AND 	SLAB_CUT_TIME		>= '" + SLAB_CUT_TIME_FROM.Trim().Substring(0, 8) + "'";
			}
			if (SLAB_CUT_TIME_TO.Trim() != "")
			{
				sqlstr += " AND 	SLAB_CUT_TIME		<= '" + SLAB_CUT_TIME_TO.Trim().Substring(0, 8) + "'";
			}
			sqlstr += " ORDER BY  SLAB_CUT_TIME DESC";
		}
		if (tableName == "TQMTS27") //低倍信息查询条件加时间 2025.04.29
		{
			if (SLAB_CUT_TIME_FROM.Trim() != "")
			{
				sqlstr += " AND 	REC_CREATE_TIME		>= '" + SLAB_CUT_TIME_FROM.Trim().Substring(0, 8) + "'";
			}
			if (SLAB_CUT_TIME_TO.Trim() != "")
			{
				sqlstr += " AND 	REC_CREATE_TIME		<= '" + SLAB_CUT_TIME_TO.Trim().Substring(0, 8) + "'";
			}
			sqlstr += " ORDER BY  REC_CREATE_TIME DESC";
		}


		Log::Trace("", "", "qmtsh5_inq IN:---sqlstr = [{0}]", sqlstr);


		/*连接sql语句*/
		sqlstr1 = sql_count + sqlstr + ")";

		cmd_inq1.SetCommandText(sqlstr1);
		cd_count = cmd_inq1.ExecuteScalar();
		start_row = record_count_per_page * (current_page_no - 1);
		if (start_row > cd_count.ToDouble())
		{
			start_row = 0;
		}

		/*完成拼接查询sql*/
		Log::Trace("", "", "条件查询sql[{0}],", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0], start_row, record_count_per_page);
		cmd_inq.Close();


		//返回分页信息
		bcls_ret->Tables.Add("PAGEINFO");	//增加块
		bcls_ret->Tables["PAGEINFO"].Columns.Add(DT_DECIMAL, "TOTAL_RECORD");						//总记录数
		bcls_ret->Tables["PAGEINFO"].Rows.Add();
		bcls_ret->Tables["PAGEINFO"].Rows[0][0] = cd_count.ToInt32();
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
