/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      潘陈
Version:     1.0
Date:        2023-12-14 15:08:16
Description: 试样成分后备-试样信息查询
**************************************************/

#include "stdafx.h"

BM2F_ENTERACE(qmts25s_inqs)
int f_qmts25s_inqs(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	try
	{
		CDbCommand cmd(conn);
		CString heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"];
		sqlstr = "SELECT count(*) FROM TPSSM11 WHERE HEAT_NO =@heat_no";
		cmd.SetCommandText(sqlstr);
		cmd.Parameters.Set("heat_no", heat_no);
		cmd.ExecuteReader();
		CString table_name = "TPSSM11";
		if (cmd.Read())
		{
			if (cmd.GetDecimal(1).ToInt32() == 0)
			{
				table_name = "TPSSM41";
			}
		}
		sqlstr = " SELECT "+table_name+".HEAT_NO, "+table_name+".PONO, "+table_name+".ST_NO, TPSSM12.DEV_CODE, TPSSM12.CHARGE_NO, TPSSM12.SM_PLAN_NO AS PLAN_NO, TPSSM12.AREA_ID, TQMTS24.ST_SAMPLE_NO, TQMTS24.REP_ELM_SEL_FLAG, TQMTS24.JUDGE_CODE, TQMTS24.ANALYSE_TIME, TQMTS24.ST_SAMPLE_DIV, TPSSMD1.STATION_ID, TPSSMD1.STATION_NAME  FROM "+table_name+""
"			LEFT JOIN TPSSM12 ON "+table_name+".HEAT_NO = TPSSM12.HEAT_NO"
"			LEFT JOIN TQMTS24 ON "+table_name+".HEAT_NO = TQMTS24.HEAT_NO AND TPSSM12.DEV_CODE = TQMTS24.DEV_CODE"
"			LEFT JOIN TPSSMD1 ON TPSSM12.DEV_CODE = TPSSMD1.DEV_CODE AND TPSSM12.AREA_ID = TPSSMD1.AREA_ID"
"			WHERE TPSSM12.HEAT_NO = @heat_no ORDER BY CHARGE_NO ";
		cmd.SetCommandText(sqlstr);
		cmd.ExecuteQuery(bcls_ret->Tables[0]);
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
