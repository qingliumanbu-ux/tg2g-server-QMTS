/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      潘陈
Version:     1.0
Date:        2023-09-14 19:54:28
Description: 太钢选择代表成分
**************************************************/

#include "stdafx.h"
#include "math.h"

BM2_FUNCTION_EXPORT


int f_qmbs_sample_select(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CDbCommand cmd(conn);
	EIClass sample_ret;
	bcls_ret->Tables[0].Rows.Add();
	bcls_ret->Tables[0].Columns.Add(DT_STRING, "ST_SAMPLE_NO");
	bcls_ret->Tables[0].Columns.Add(DT_STRING, "HEAT_NO");
	CModel tqmts24("TQMTS24");
	try
	{
		//查询所有试样 
		CString heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString();

		//sqlstr = " SELECT * FROM TQMTS24 WHERE   WHOLE_BACKLOG_CODE = 'C' AND HEAT_NO = @heat_no AND ELM_001 != 0 ORDER BY  JUDGE_CODE ASC ,REC_CREATE_TIME DESC";
		sqlstr = " SELECT * FROM( "
			" (SELECT * FROM TQMTS24 WHERE   WHOLE_BACKLOG_CODE = 'C' AND HEAT_NO = @heat_no AND ELM_001 != 0 "
			" AND (ST_SAMPLE_DIV = 'T' OR ST_SAMPLE_DIV = 'P' OR ST_SAMPLE_DIV = '1') "
			" AND JUDGE_CODE = '1') ORDER BY  REC_CREATE_TIME DESC) "
			" UNION  ALL "
			" SELECT * FROM( "
			" (SELECT * FROM TQMTS24 WHERE   WHOLE_BACKLOG_CODE = 'C' AND HEAT_NO = @heat_no AND ELM_001 != 0 "
			" AND (ST_SAMPLE_DIV = 'T' OR ST_SAMPLE_DIV = 'P' OR ST_SAMPLE_DIV = '1') "
			" AND JUDGE_CODE = '2') ORDER BY REC_CREATE_TIME ASC) ";
		cmd.SetCommandText(sqlstr);
		cmd.Parameters.Set("heat_no", heat_no);
		cmd.ExecuteQuery(sample_ret.Tables[0]);
		cmd.Close();
		if (sample_ret.Tables[0].Rows.get_Count() == 0)//
		{
			return 0;
		}
		//2025.02.14   合格按时间倒序取最新一条，不合格按时间正序取最旧一条
		if (sample_ret.Tables[0].Rows.get_Count() >= 1)//选第一个样，如果有合格的就选，只有一个不合格也选。
		{
			bcls_ret->Tables[0].Rows[0]["ST_SAMPLE_NO"] = sample_ret.Tables[0].Rows[0]["ST_SAMPLE_NO"];
			bcls_ret->Tables[0].Rows[0]["HEAT_NO"] = sample_ret.Tables[0].Rows[0]["HEAT_NO"];
			return 0;
		}
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


