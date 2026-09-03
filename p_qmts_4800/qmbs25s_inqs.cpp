/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      潘陈
Version:     1.0
Date:        2023-11-23 15:08:16
Description: 炉次内试样信息查询
**************************************************/

#include "stdafx.h"

BM2F_ENTERACE(qmbs25s_inqs)
int f_qmbs25s_inqs(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CDbCommand cmd_inq(conn);
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CDbCommand cmd(conn);
	try
	{
		CDbCommand cmd(conn);
		CString heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"];
		Log::Trace("", "", "heat_no = {0}", heat_no);
		CString lian_zhu_no = bcls_rec->Tables[0].Rows[0]["LIAN_ZHU_NO"];
		Log::Trace("", "", "lian_zhu_no = {0}", lian_zhu_no);
		sqlstr = "SELECT count(*) FROM TPSSM11 WHERE HEAT_NO =@heat_no";
		cmd.SetCommandText(sqlstr);
		cmd.Parameters.Set("heat_no", heat_no);
		cmd.ExecuteReader();
		CString table_name = "TPSSM11";
		CString table_name2 = "TPSSM12";
		if (cmd.Read())
		{
			if (cmd.GetDecimal(1).ToInt32() == 0)
			{
				table_name = "TPSSM41";
				table_name2 = "TPSSM42";
			}
		}
		CString sqlstr1 = "SELECT COUNT(*) FROM TPSSM42 WHERE HEAT_NO = '" + heat_no + "' AND DEV_CODE ='C0'";
		cmd_inq.SetCommandText(sqlstr1);
		int count = cmd_inq.ExecuteScalar().ToInt32();
		if (count >0)
		{
			//2025.06.03 查询增加成分创建时间
			sqlstr = " SELECT " + table_name + ".HEAT_NO, " + table_name + ".PONO, " + table_name + ".ST_NO, " + table_name2 + ".DEV_CODE, " + table_name2 + ".CHARGE_NO, " + table_name2 + ".SM_PLAN_NO AS PLAN_NO, " + table_name2 + ".AREA_ID, TQMTS24.ST_SAMPLE_NO, TQMTS24.REP_ELM_SEL_FLAG, TQMTS24.JUDGE_CODE, TQMTS24.ANALYSE_TIME,TQMTS24.SAMPLE_TAKEN_TIME, TQMTS24.ST_SAMPLE_DIV,TQMTS24.ID_ELM,TQMTS24.REC_CREATE_TIME, TPSSMD1.STATION_ID, TPSSMD1.STATION_NAME  FROM " + table_name + ""   
				"			LEFT JOIN " + table_name2 + " ON " + table_name + ".HEAT_NO = " + table_name2 + ".HEAT_NO"
				"			LEFT JOIN TQMTS24 ON " + table_name + ".HEAT_NO = TQMTS24.HEAT_NO AND " + table_name2 + ".DEV_CODE = TQMTS24.DEV_CODE"
				"			LEFT JOIN TPSSMD1 ON " + table_name2 + ".DEV_CODE = TPSSMD1.DEV_CODE AND " + table_name2 + ".AREA_ID = TPSSMD1.AREA_ID";

			if (lian_zhu_no == "1")
			{
				sqlstr += "	 WHERE " + table_name2 + ".DEV_CODE LIKE '%C%' AND " + table_name2 + ".HEAT_NO = @heat_no ORDER BY CHARGE_NO ";
			}
			else
			{
				sqlstr += "	 WHERE " + table_name2 + ".HEAT_NO = @heat_no ORDER BY CHARGE_NO ";
			}
			Log::Trace("", "", "sqlstr = {0}", sqlstr);
			cmd.SetCommandText(sqlstr);
			cmd.ExecuteQuery(bcls_ret->Tables[0]);
		}
		else
		{
			//2025.06.03 查询增加成分创建时间
			sqlstr = " SELECT " + table_name + ".HEAT_NO, " + table_name + ".PONO, " + table_name + ".ST_NO, " + table_name2 + ".DEV_CODE, " + table_name2 + ".CHARGE_NO, " + table_name2 + ".SM_PLAN_NO AS PLAN_NO, " + table_name2 + ".AREA_ID, TQMTS24.ST_SAMPLE_NO, TQMTS24.REP_ELM_SEL_FLAG, TQMTS24.JUDGE_CODE, TQMTS24.ANALYSE_TIME,TQMTS24.SAMPLE_TAKEN_TIME, TQMTS24.ST_SAMPLE_DIV,TQMTS24.ID_ELM,TQMTS24.REC_CREATE_TIME, TPSSMD1.STATION_ID, TPSSMD1.STATION_NAME  FROM " + table_name + ""
				"			LEFT JOIN " + table_name2 + " ON " + table_name + ".HEAT_NO = " + table_name2 + ".HEAT_NO"
				"			LEFT JOIN TQMTS24 ON " + table_name + ".HEAT_NO = TQMTS24.HEAT_NO AND " + table_name2 + ".DEV_CODE = TQMTS24.DEV_CODE"
				"			LEFT JOIN TPSSMD1 ON " + table_name2 + ".DEV_CODE = TPSSMD1.DEV_CODE AND " + table_name2 + ".AREA_ID = TPSSMD1.AREA_ID";

			if (lian_zhu_no == "1")
			{
				sqlstr += "	 WHERE " + table_name2 + ".DEV_CODE LIKE '%C%' AND " + table_name2 + ".HEAT_NO = @heat_no ORDER BY CHARGE_NO ";
			}
			else
			{
				sqlstr += "	 WHERE " + table_name2 + ".HEAT_NO = @heat_no ORDER BY CHARGE_NO ";
			}
			Log::Trace("", "", "sqlstr = {0}", sqlstr);
			cmd.SetCommandText(sqlstr);
			cmd.ExecuteQuery(bcls_ret->Tables[0]);

			//查询是否有铸坯样
			sqlstr = " SELECT HEAT_NO, PONO, ST_NO, DEV_CODE, ST_SAMPLE_NO, REP_ELM_SEL_FLAG, JUDGE_CODE, ANALYSE_TIME, SAMPLE_TAKEN_TIME, ST_SAMPLE_DIV,ID_ELM "
				"			FROM TQMTS24 WHERE HEAT_NO = @heat_no AND DEV_CODE = 'C0' ";
			cmd.SetCommandText(sqlstr);
			cmd.Parameters.Set("heat_no", heat_no);
			cmd.ExecuteReader();
			if (cmd.Read())
			{
				CModel tqmts24("TQMTS24");
				cmd.Fetch(tqmts24);
				tqmts24.MergeTo(bcls_ret->Tables[0]);
			}
		}
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
