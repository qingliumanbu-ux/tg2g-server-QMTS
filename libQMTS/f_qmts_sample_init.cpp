/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      panchen
Version:     1.0
Date:        2024-01-08 19:54:28 
Description: 试样号生成
**************************************************/

#include "stdafx.h"

BM2_FUNCTION_EXPORT


int f_qmts_sample_init(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CDbCommand cmd(conn);
	CModel tqmts24("TQMTS24");
	try
	{
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "ST_SAMPLE_NO");

		//试样号生成
		CDecimal st_sample_seq = 1;
		sqlstr = " SELECT MAX(ST_SAMPLE_SEQ) FROM TQMTS24 WHERE HEAT_NO = @heat_no"
			"			AND DEV_CODE = @dev_code ";
		cmd.SetCommandText(sqlstr);
		cmd.Parameters.Set("heat_no", bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString());
		cmd.Parameters.Set("dev_code", bcls_rec->Tables[0].Rows[0]["DEV_CODE"].ToString());

		cmd.ExecuteReader();
		if (cmd.Read())
		{
			st_sample_seq = cmd.GetDecimal(1) + 1;
		}
		bcls_rec->Tables[0].Rows[0]["st_sample_no"] = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString() + "-" + bcls_rec->Tables[0].Rows[0]["DEV_CODE"].ToString() + bcls_rec->Tables[2].Rows[0]["ST_SAMPLE_DIV"].ToString() + "-" + bcls_rec->Tables[2].Rows[0]["ELM_TYPE_DIV"].ToString() + "-" + st_sample_seq.ToString();
		bcls_ret->Tables[0].Rows.Add();
		bcls_ret->Tables[0].Rows[0]["ST_SAMPLE_NO"] = bcls_rec->Tables[0].Rows[0]["st_sample_no"];
	}
	catch (CDbException& ex)
	{
		CFormattable arguments[] = { ex.GetCode(), ex.GetMsg() };
		CMessageFormat::Format(s.msg, "Database Error,sqlcode=[{0}],sqlmsg=[{1}]", arguments, 2);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;
	}
	catch (CApplicationException& ex)
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strcpy(s.msg, ex.GetMsg());
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	return doFlag;
}


